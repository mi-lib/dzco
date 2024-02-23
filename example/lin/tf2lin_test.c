#include <dzco/dz_lin.h>

void test(dzTF *tf, int i)
{
  dzLin lin;

  printf( "+++ test %d +++\n", i );
  printf( "[controllable canonical form]\n" );
  dzTF2LinCtrlCanon( tf, &lin );
  dzLinFPrintZTK( stdout, &lin );
  dzLinDestroy( &lin );
  printf( "[observable canonical form]\n" );
  dzTF2LinObsCanon( tf, &lin );
  dzLinFPrintZTK( stdout, &lin );
  dzLinDestroy( &lin );
  printf( "\n" );
}

int main(int argc, char *argv[])
{
  dzTF tf;

  dzTFAlloc( &tf, 1, 2 );
  dzTFSetNumList( &tf, 5.0, 4.0 );
  dzTFSetDenList( &tf, 1.0, 3.0, 2.0 );
  test( &tf, 1 );
  dzTFDestroy( &tf );

  dzTFAlloc( &tf, 2, 2 );
  dzTFSetNumList( &tf, 6.0, 7.0, 2.0 );
  dzTFSetDenList( &tf, 1.0, 3.0, 2.0 );
  test( &tf, 2 );
  dzTFDestroy( &tf );

  return 0;
}
