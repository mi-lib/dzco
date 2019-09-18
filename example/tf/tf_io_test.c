#include <dzco/dz_tf.h>

int main(int argc, char *argv[])
{
  dzTF tf;

  dzTFScanZTK( &tf, "tf_test.ztk" );
  dzTFFPrintZTK( stdout, &tf );
  dzTFExpr( &tf );
  dzTFDestroy( &tf );
  return 0;
}
