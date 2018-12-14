#include <dzco/dz_lin.h>

void test(dzPex *pex, int i)
{
  dzLin lin;

  printf( "+++ test %d +++\n", i );
  printf( "[controllable canonical form]\n" );
  dzPex2LinCtrlCanon( pex, &lin );
  dzLinWrite( &lin );
  dzLinDestroy( &lin );
  printf( "[observable canonical form]\n" );
  dzPex2LinObsCanon( pex, &lin );
  dzLinWrite( &lin );
  dzLinDestroy( &lin );
  printf( "\n" );
}

int main(int argc, char *argv[])
{
  dzPex pex;

  dzPexAlloc( &pex, 1, 2 );
  dzPexSetNumList( &pex, 5.0, 4.0 );
  dzPexSetDenList( &pex, 1.0, 3.0, 2.0 );
  test( &pex, 1 );
  dzPexDestroy( &pex );

  dzPexAlloc( &pex, 2, 2 );
  dzPexSetNumList( &pex, 6.0, 7.0, 2.0 );
  dzPexSetDenList( &pex, 1.0, 3.0, 2.0 );
  test( &pex, 2 );
  dzPexDestroy( &pex );

  return 0;
}
