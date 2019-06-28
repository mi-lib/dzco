#include <dzco/dz_pex.h>

bool dz_bode_parse_range(char *str, double *from, double *to, double *d)
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

void dz_bode_output(FILE *fp, dzPex *pex, double from, double to, double d)
{
  double frq;
  zComplex c;

  for( frq=from; frq<to; frq*=d ){
    dzPexFreqRes( pex, 2*zPI*frq, &c );
    fprintf( fp, "%f %f %f\n", frq,
      20*log10(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
  }
}

void dz_bode_script(FILE *fp, char *logfile, double from, double to)
{
  fprintf( fp, "clear\n" );
  fprintf( fp, "set grid\n" );
  fprintf( fp, "unset key\n" );
  fprintf( fp, "set logscale x\n" );
  fprintf( fp, "set multiplot\n" );
  fprintf( fp, "set size ratio 0.3\n" );
  fprintf( fp, "set format x \"%%2.0tx10^%%.0T\"\n" );
  fprintf( fp, "set origin 0,0.25\n" );
  fprintf( fp, "plot [%f:%f] '%s' u 1:2 w l\n", from, to, logfile );
  fprintf( fp, "set origin 0,-0.25\n" );
  fprintf( fp, "plot [%f:%f] '%s' u 1:3 w l\n", from, to, logfile );
  fprintf( fp, "unset multiplot\n" );
}

enum{
  OPT_PEXFILE=0, OPT_OUTPUTFILE, OPT_SCRIPTFILE,
  OPT_RANGE,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "p", "pex", "<.dzp file>", "transfer function definition file", NULL, false },
  { "o", "out", "<output file>", "result output file", "bode_out", false },
  { "s", "script", "<script file>", "script file to plot the result on gnuplot", "bode.plot", false },
  { "r", "range", "<string>", "output range in logarism scale", "-3:3:.001", false },
  { "h", "help", NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

void dz_bode_usage(char *arg)
{
  eprintf( "Usage: %s [-range <string>] <.dzp file> [output file] [script file]\n", arg );
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

bool dz_bode_commandarg(int argc, char *argv[])
{
  zStrAddrList arglist;

  if( !zOptionRead( opt, argv, &arglist ) ) return false;
  if( opt[OPT_HELP].flag ) dz_bode_usage( "dz_bode" );
  if( !zListIsEmpty(&arglist) ){
    opt[OPT_PEXFILE].flag = true;
    opt[OPT_PEXFILE].arg  = zListTail(&arglist)->data;
  }
  if( !opt[OPT_PEXFILE].flag ){
    ZRUNERROR( "transfer function not specified" );
    return false;
  }
  zStrAddrListDestroy( &arglist );
  return true;
}

int main(int argc, char *argv[])
{
  dzPex pex;
  double from, to, d;
  FILE *fp;

  if( argc < 2 ) dz_bode_usage( argv[0] );
  if( !dz_bode_commandarg( argc, argv+1 ) ) return 1;
  if( !( fp = fopen( opt[OPT_PEXFILE].arg, "r" ) ) ){
    ZOPENERROR( opt[OPT_PEXFILE].arg );
    return 1;
  }
  if( !dzPexFScan( fp, &pex ) ) return 1;
  fclose( fp );
  dz_bode_parse_range( opt[OPT_RANGE].arg, &from, &to, &d );

  fp = fopen( opt[OPT_OUTPUTFILE].arg, "w" );
  dz_bode_output( fp, &pex, from, to, d );
  fclose( fp );

  dzPexDestroy( &pex );
  fp = fopen( opt[OPT_SCRIPTFILE].arg, "w" );
  dz_bode_script( fp, opt[OPT_OUTPUTFILE].arg, from, to );
  fclose( fp );
  return 0;
}
