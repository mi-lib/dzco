#include <dzco/dz_tf.h>

int main(int argc, char *argv[])
{
  dzTF tf;

  if( argc < 2 ){
    eprintf( "specify a transfer function file.\n" );
    return 1;
  }
  dzTFReadZTK( &tf, argv[1] );
  printf( "polynomial rational transfer function\n" );
  dzTFExpr( &tf );
  dzTFZeroPole( &tf );
  printf( "zeros: " ); zCVecPrint( dzTFZero(&tf) );
  printf( "poles: " ); zCVecPrint( dzTFPole(&tf) );
  dzTFDestroy( &tf );
  return 0;
}
