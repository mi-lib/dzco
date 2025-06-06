#include <dzco/dz_tf.h>

bool dz_nyq_parse_range(char *str, double *from, double *to, double *d)
{
  double exp_from, exp_to, exp_d;

  if( sscanf( str, "%lf:%lf:%lf", &exp_from, &exp_to, &exp_d ) < 3 ){
    eprintf( "invalid range specified: %s\n", str );
    return false;
  }
  *from = pow( 10, exp_from );
  *to   = pow( 10, exp_to );
  *d    = pow( 10, exp_d );
  return true;
}

void dz_nyq_output(FILE *fp, dzTF *tf, double from, double to, double d)
{
  double frq;
  zComplex c;

  for( frq=to; frq>from; frq/=d ){
    dzTFToComplex( tf, -2*zPI*frq, &c );
    fprintf( fp, "%f %f\n", c.re, c.im );
  }
  for( frq=from; frq<to; frq*=d ){
    dzTFToComplex( tf, 2*zPI*frq, &c );
    fprintf( fp, "%f %f\n", c.re, c.im );
  }
}

void dz_nyq_script(FILE *fp, char *logfile)
{
  fprintf( fp, "clear\n" );
  fprintf( fp, "set grid\n" );
  fprintf( fp, "unset key\n" );
  fprintf( fp, "set size square\n" );
  fprintf( fp, "set multiplot\n" );
  fprintf( fp, "set origin 0,0\n" );
  fprintf( fp, "plot [-3:3][-3:3] '%s' w l 1\n", logfile );
  fprintf( fp, "set parametric\n" );
  fprintf( fp, "plot [][-3:3][-3:3] cos(t),sin(t) w l 2\n" );
  fprintf( fp, "unset multiplot\n" );
  fprintf( fp, "unset parametric\n" );
}

enum{
  OPT_TFFILE=0, OPT_OUTPUTFILE, OPT_SCRIPTFILE,
  OPT_RANGE,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "f", "tf", "<.ztk file>", "transfer function definition file", NULL, false },
  { "o", "out", "<output file>", "result output file", (char *)"nyq_out", false },
  { "s", "script", "<script file>", "script file to plot the result on gnuplot", (char *)"nyq.plot", false },
  { "r", "range", "<string>", "output range in logarism scale", (char *)"-3:3:.001", false },
  { "h", "help", NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

void dz_nyq_usage(const char *arg)
{
  eprintf( "Usage: %s [-range <string>] <.ztk file> [output file] [script file]\n", arg );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  eprintf( "string format:\n" );
  eprintf( "f1:f2:s\n" );
  eprintf( " f1 ... lowest frequency 10^?\n" );
  eprintf( " f2 ... highest frequency 10^?\n" );
  eprintf( " s  ... logalism step\n\n" );
  eprintf( "In order to plot the result, execute gnuplot and load the script file.\n" );
  exit( 0 );
}

bool dz_nyq_commandarg(int argc, char *argv[])
{
  zStrAddrList arglist;

  if( !zOptionRead( opt, argv, &arglist ) ) return false;
  if( opt[OPT_HELP].flag ) dz_nyq_usage( "dz_nyquist" );
  if( !zListIsEmpty(&arglist) ){
    opt[OPT_TFFILE].flag = true;
    opt[OPT_TFFILE].arg  = zListTail(&arglist)->data;
  }
  if( !opt[OPT_TFFILE].flag ){
    ZRUNERROR( "transfer function not specified" );
    return false;
  }
  zStrAddrListDestroy( &arglist );
  return true;
}

int main(int argc, char *argv[])
{
  dzTF tf;
  double from, to, d;
  FILE *fp;

  if( argc < 2 ) dz_nyq_usage( argv[0] );
  if( !dz_nyq_commandarg( argc, argv+1 ) ) return 1;
  if( !dzTFReadZTK( &tf, opt[OPT_TFFILE].arg ) ) return 1;
  dz_nyq_parse_range( opt[OPT_RANGE].arg, &from, &to, &d );

  fp = fopen( opt[OPT_OUTPUTFILE].arg, "w" );
  dz_nyq_output( fp, &tf, from, to, d );
  fclose( fp );

  dzTFDestroy( &tf );
  fp = fopen( opt[OPT_SCRIPTFILE].arg, "w" );
  dz_nyq_script( fp, opt[OPT_OUTPUTFILE].arg );
  fclose( fp );
  return 0;
}
