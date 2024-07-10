#include <dzco/dz_sys.h>

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

#define N 100

void assert_lqr(void)
{
  dzSys sys;
  dzLin *lin;
  double input;
  zVec q, opt_gain;
  double q2, r;
  int i;
  bool result = true;

  zRandInit();
  /* create a linear system */
  lin = zAlloc( dzLin, 1 );
  dzLinAlloc( lin, 2 );
  zMatSetElem( lin->a, 0, 1, 1 ); /* transition matrix */
  zVecSetElem( lin->b, 1, 1 ); /* input coefficient vector */
  zVecSetElem( lin->c, 0, 1 ); /* output coefficient vector */
  lin->d = 0; /* output transfer coefficient */
  dzSysLinCreate( &sys, lin );
  dzSysInputPtr(&sys,0) = &input;
  q = zVecAlloc( 2 );
  opt_gain = zVecAlloc( 2 );

  for( i=0; i<N; i++ ){
    q2 = zRandF( zTOL, 10 );
    r = zRandF( zTOL, 10 );
    zVecSetElemList( q, 1.0, q2 );
    if( !dzLinLQR( dzSysLin(&sys), q, r, opt_gain ) ) exit( 1 );
    /* compare with analytical solution */
    if( !zIsTiny( zVecElemNC(opt_gain,0) - 1.0/sqrt(r) ) ||
        !zIsTiny( zVecElemNC(opt_gain,1) - sqrt(2*sqrt(r)+q2)/sqrt(r) ) ){
      result = false;
      break;
    }
  }
  zAssert( dzLinLQR, result );

  zVecFree( q );
  zVecFree( opt_gain );
  dzSysDestroy( &sys );
}

int main(void)
{
  zRandInit();
  assert_co();
  assert_lqr();
  return EXIT_SUCCESS;
}
