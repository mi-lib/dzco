#include <dzco/dz_lin.h>

void linsys_xfer(dzLin *lin, zMat a, zVec b, zVec c, zMat t)
{
  zMat at;

  at = zMatAllocSqr( dzLinDim(lin) );
  zMulMatMat( a, t, at );
  zMulInvMatMat( t, at, lin->a );
  zLESolveGauss( t, b, lin->b );
  zMulMatTVec( t, c, lin->c );
  zMatFree( at );
}

void assert_co(void)
{
  const int dim = 5;
  dzLin lin;
  zMat a, t;
  zVec b, c;
  double tmp;
  register int i;
  bool result_ctl, result_obs;

  dzLinAlloc( &lin, dim );
  a = zMatAllocSqr( dim );
  b = zVecAlloc( dim );
  c = zVecAlloc( dim );
  t = zMatAllocSqr( dim );
  for( i=0; i<dim; i++ ){
    zMatSetElemNC( a, i, i, zRandF(-10,10) );
    zVecSetElemNC( b, i, zRandF(-10,10) );
    if( zIsTiny(zVecElemNC(b,i)) ) zVecElemNC(b,i) += 1.0;
    zVecSetElemNC( c, i, zRandF(-10,10) ); 
   if( zIsTiny(zVecElemNC(c,i)) ) zVecElemNC(c,i) += 1.0;
  }
  lin.d = 0;
  zMatIdent( t );
  zMatRandUniform( t, -1, 1 );

  result_ctl = result_obs = true;
  linsys_xfer( &lin, a, b, c, t );
  if( !dzLinIsCtrl( &lin ) ) result_ctl = false;
  if( !dzLinIsObs( &lin ) ) result_obs = false;
  tmp = zVecElemNC(b,1); zVecElemNC(b,1) = 0;
  linsys_xfer( &lin, a, b, c, t );
  if( dzLinIsCtrl( &lin ) ) result_ctl = false;
  if( !dzLinIsObs( &lin ) ) result_obs = false;
  zVecElemNC(b,1) = tmp;
  zVecElemNC(c,1) = 0;
  linsys_xfer( &lin, a, b, c, t );
  if( !dzLinIsCtrl( &lin ) ) result_ctl = false;
  if( dzLinIsObs( &lin ) ) result_obs = false;
  zVecElemNC(b,1) = 0;
  linsys_xfer( &lin, a, b, c, t );
  if( dzLinIsCtrl( &lin ) ) result_ctl = false;
  if( dzLinIsObs( &lin ) ) result_obs = false;
  zAssert( dzLinIsCtrl, result_ctl );
  zAssert( dzLinIsObs, result_obs );

  dzLinDestroy( &lin );
  zMatFree( a );
  zVecFree( b );
  zVecFree( c );
  zMatFree( t );
}

int main(void)
{
  zRandInit();
  assert_co();
  return EXIT_SUCCESS;
}
