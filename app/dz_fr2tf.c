#include <dzco/dz_tf.h>

enum{
  OPT_FRFILE=0,
  OPT_TFFILE,
  OPT_RANGE,
  OPT_ITER,
  OPT_DIM_NUM,
  OPT_DIM_DEN,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "i", "input", "<data file>", "input frequency response file", NULL, false },
  { "o", "output", "<TF file>", "output transfer function file", "tf", false },
  { "r", "range", "<string>", "range for identification in logarism scale", "10:1000", false },
  { "", "iter", "<number>", "maximum iteration number", "2", false },
  { "n", "dn", "<number>", "dimension of numerator", "2", false },
  { "d", "dd", "<number>", "dimension of denominator", "2", false },
  { "h", "help", NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

void dz_fr2tf_usage(char *arg)
{
  eprintf( "Usage: %s [option] <data file> [output file]\n", arg );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  eprintf( "string format:\n" );
  eprintf( "f1:f2\n" );
  eprintf( " f1 ... lowest frequency [Hz]\n" );
  eprintf( " f2 ... highest frequency [Hz]\n" );
  exit( 0 );
}

bool dz_fr2tf_parse_range(char *str, double *from, double *to)
{
  if( sscanf( str, "%lf:%lf", from, to ) < 2 ){
    eprintf( "invalid range specified: %s\n", str );
    return false;
  }
  return true;
}

bool dz_fr2tf_command_arg(int argc, char *argv[])
{
  zStrAddrList arglist;

  if( !zOptionRead( opt, argv+1, &arglist ) ) return false;
  if( opt[OPT_HELP].flag ) dz_fr2tf_usage( argv[0] );
  if( !zListIsEmpty(&arglist) ){
    opt[OPT_FRFILE].flag = true;
    opt[OPT_FRFILE].arg  = zListTail(&arglist)->data;
  }
  if( !opt[OPT_FRFILE].flag ){
    ZRUNERROR( "transfer function not specified" );
    return false;
  }
  zStrAddrListDestroy( &arglist );
  return true;
}

int main(int argc, char *argv[])
{
  dzFreqResList freq_res_list;
  dzTF tf;
  double fmin, fmax;

  if( argc < 2 ) dz_fr2tf_usage( argv[0] );
  if( !dz_fr2tf_command_arg( argc, argv ) ) return EXIT_FAILURE;
  dz_fr2tf_parse_range( opt[OPT_RANGE].arg, &fmin, &fmax );

  if( dzFreqResListScanFile( &freq_res_list, opt[OPT_FRFILE].arg, fmin, fmax ) == 0 )
    return EXIT_FAILURE;
  dzTFIdentFromFreqRes( &tf, &freq_res_list, atoi(opt[OPT_DIM_NUM].arg), atoi(opt[OPT_DIM_DEN].arg), atoi(opt[OPT_ITER].arg) );
  dzTFWriteZTK( &tf, opt[OPT_TFFILE].arg );
  dzTFDestroy( &tf );
  dzFreqResListDestroy( &freq_res_list );
  return EXIT_SUCCESS;
}
