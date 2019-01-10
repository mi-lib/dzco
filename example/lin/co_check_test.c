#include <dzco/dz_lin.h>

#define TEST 2

int main(void)
{
  dzLin lin;
#if TEST == 1
  double a[] = { 0, 1, -1, -1 };
  double b[] = { 0, 1 };
  double c[] = { 1, 0 };
  int n = 2;
#else
  double a[] = { 0, 1, 0, 1, 0, -1, 0, 0, 0 };
  double b[] = { 0, 0, 1 };
  double c[] = { 0, 0, 1 };
  int n = 3;
#endif
  zMat u;

  dzLinAlloc( &lin, n );
  zMatCopyArray( a, n, n, lin.a );
  zVecCopyArray( b, n, lin.b );
  zVecCopyArray( c, n, lin.c );

  printf( ">>A<<\n" );
  zMatWrite( lin.a );
  printf( ">>b<<\n" );
  zVecWrite( lin.b );
  printf( ">>c<<\n" );
  zVecWrite( lin.c );
  u = zMatAllocSqr( n );
  printf( ">>Uc<<\n" );
  zMatWrite( dzLinCtrlMat( &lin, u ) );
  printf( ">>Uo<<\n" );
  zMatWrite( dzLinObsMat( &lin, u ) );
  zMatFree( u );
  eprintf( "controllable -> %s\n", zBoolExpr(dzLinIsCtrl(&lin)) );
  eprintf( "observable -> %s\n", zBoolExpr(dzLinIsObs(&lin)) );
  dzLinDestroy( &lin );
  return 0;
}
