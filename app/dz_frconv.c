#include <dzco/dz_tf.h>

enum{
  OPT_INFILE=0,
  OPT_OUTFILE,
  OPT_RANGE,
  OPT_OPERATE,
  OPT_TF,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "i", "input", "<data file>", "input frequency response file", NULL, false },
  { "o", "output", "<data file>", "output frequency response file", (char *)"fr_out", false },
  { "r", "range", "<string>", "range for identification in logarism scale", (char *)"0:10e16", false },
  { "x", "operate", "open/close/connect", "convert frequency response to that of closed loop system (default)", (char *)"close", true },
  { "t", "tf", "<TF file>", "connect a transfer function", NULL, false },
  { "h", "help", NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

void dz_frconv_usage(char *arg)
{
  eprintf( "Usage: %s [option] <data file> [output file]\n", arg );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  eprintf( "string format:\n" );
  eprintf( "f1:f2:s\n" );
  eprintf( " f1 ... lowest frequency 10^?\n" );
  eprintf( " f2 ... highest frequency 10^?\n" );
  eprintf( " s  ... logalism step\n\n" );
  exit( 0 );
}

bool dz_frconv_parse_range(char *str, double *from, double *to)
{
  if( sscanf( str, "%lf:%lf", from, to ) < 2 ){
    eprintf( "invalid range specified: %s\n", str );
    return false;
  }
  return true;
}

bool dz_frconv_command_arg(int argc, char *argv[])
{
  zStrAddrList arglist;

  if( !zOptionRead( opt, argv+1, &arglist ) ) return false;
  if( opt[OPT_HELP].flag ) dz_frconv_usage( argv[0] );
  if( !zListIsEmpty(&arglist) ){
    opt[OPT_INFILE].flag = true;
    opt[OPT_INFILE].arg  = zListTail(&arglist)->data;
  }
  if( !opt[OPT_INFILE].flag ){
    ZRUNERROR( "transfer function not specified" );
    return false;
  }
  zStrAddrListDestroy( &arglist );
  return true;
}

void dz_frconv_operate(dzFreqResList *inlist, dzFreqResList *outlist, dzTF *tf)
{
  if( strcmp( opt[OPT_OPERATE].arg, "open" ) == 0 ){
    dzFreqResList2Open( inlist, outlist );
  } else
  if( strcmp( opt[OPT_OPERATE].arg, "close" ) == 0 ){
    dzFreqResList2Closed( inlist, outlist );
  } else
  if( strcmp( opt[OPT_OPERATE].arg, "connect" ) == 0 ){
    if( !opt[OPT_TF].flag ){
      ZRUNERROR( "transfer function not specified." );
    } else{
      dzFreqResListConnectTF( inlist, tf, outlist );
    }
  }
}

int main(int argc, char *argv[])
{
  dzFreqResList fr_in_list, fr_out_list;
  dzTF tf;
  double fmin, fmax;

  if( argc < 2 ) dz_frconv_usage( argv[0] );
  if( !dz_frconv_command_arg( argc, argv ) ) return EXIT_FAILURE;
  dz_frconv_parse_range( opt[OPT_RANGE].arg, &fmin, &fmax );

  if( dzFreqResListScanFile( &fr_in_list, opt[OPT_INFILE].arg, fmin, fmax ) == 0 )
    return EXIT_FAILURE;
  if( opt[OPT_TF].flag && !dzTFReadZTK( &tf, opt[OPT_TF].arg ) ) return EXIT_FAILURE;
  if( opt[OPT_OPERATE].flag )
    dz_frconv_operate( &fr_in_list, &fr_out_list, &tf );
  if( opt[OPT_TF].flag ) dzTFDestroy( &tf );
  if( dzFreqResListPrintFile( &fr_out_list, opt[OPT_OUTFILE].arg, fmin, fmax ) == 0 )
    return EXIT_FAILURE;
  dzFreqResListDestroy( &fr_in_list );
  dzFreqResListDestroy( &fr_out_list );
  return EXIT_SUCCESS;
}
