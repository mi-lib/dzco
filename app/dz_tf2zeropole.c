#include <dzco/dz_tf.h>

enum{
  OPT_TFFILE=0,
  OPT_ZEROFILE,
  OPT_POLEFILE,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "i", "input", "<TF file>", "input transfer function file", NULL, false },
  { "z", "zero file", "<data file>", "output file to store zeros", "zero.dat", false },
  { "p", "pole file", "<data file>", "output file to store poles", "pole.dat", false },
  { "h", "help", NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

void dz_tf2zeropole_usage(char *arg)
{
  eprintf( "Usage: %s [option] <TF file> [output file]\n", arg );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  exit( 0 );
}

bool dz_tf2zeropole_command_arg(int argc, char *argv[])
{
  zStrAddrList arglist;

  if( !zOptionRead( opt, argv+1, &arglist ) ) return false;
  if( opt[OPT_HELP].flag ) dz_tf2zeropole_usage( argv[0] );
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

void dz_tf2zeropole_output_one(FILE *fp, zCVec v)
{
  register int i;

  for( i=0; i<zCVecSizeNC(v); i++ )
    fprintf( fp, "%.10g %.10g\n", zCVecElemNC(v,i)->re, zCVecElemNC(v,i)->im );
}

void dz_tf2zeropole_output(zCVec zero, zCVec pole)
{
  FILE *fp;

  if( !( fp = fopen( opt[OPT_ZEROFILE].arg, "w" ) ) ){
    ZOPENERROR( opt[OPT_ZEROFILE].arg );
    return;
  }
  dz_tf2zeropole_output_one( fp, zero );
  fclose( fp );

  if( !( fp = fopen( opt[OPT_POLEFILE].arg, "w" ) ) ){
    ZOPENERROR( opt[OPT_POLEFILE].arg );
    return;
  }
  dz_tf2zeropole_output_one( fp, pole );
  fclose( fp );
}

int main(int argc, char *argv[])
{
  dzTF tf;
  zCVec zero, pole;

  if( argc < 2 ) dz_tf2zeropole_usage( argv[0] );
  if( !dz_tf2zeropole_command_arg( argc, argv ) ) return EXIT_FAILURE;
  if( !dzTFReadZTK( &tf, opt[OPT_TFFILE].arg ) ) return EXIT_FAILURE;
  if( !dzTFZeroPole( &tf, &zero, &pole ) ) return EXIT_FAILURE;
  dz_tf2zeropole_output( zero, pole );
  zCVecFree( zero );
  zCVecFree( pole );
  dzTFDestroy( &tf );
  return EXIT_SUCCESS;
}
