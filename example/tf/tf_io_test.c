#include <dzco/dz_tf.h>

int main(int argc, char *argv[])
{
  dzTF tf;
  zCVec zero, pole;

  if( argc < 2 ){
    eprintf( "specify a transfer function file.\n" );
    return 1;
  }
  dzTFReadZTK( &tf, argv[1] );
  printf( "polynomial rational transfer function\n" );
  dzTFExpr( &tf );
  dzTFZeroPole( &tf, &zero, &pole );
  printf( "zeros: " ); zCVecPrint( zero );
  printf( "poles: " ); zCVecPrint( pole );
  zCVecFree( zero );
  zCVecFree( pole );
  dzTFDestroy( &tf );
  return 0;
}
