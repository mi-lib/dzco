/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_lin - linear system
 */

#include <dzco/dz_lin.h>

/* compute state velocity. */
static zVec __dz_lin_state_dif(double t, zVec x, void *sys, zVec dx)
{
  dzLin *lin;

  lin = (dzLin *)sys;
  zMulMatVec( lin->a, x, lin->_ax );
  return zVecAdd( lin->_ax, lin->_bu, dx );
}

/* initialize a linear system. */
dzLin *dzLinInit(dzLin *lin)
{
  lin->a = NULL;
  lin->b = lin->c = lin->x = NULL;
  lin->d = 0;
  lin->_ax = lin->_bu = NULL;
  return lin;
}

/* destroy internal working space of a linear system. */
static void _dzLinDestroyODE(dzLin *lin)
{
  zVecFree( lin->_ax );
  zVecFree( lin->_bu );
  zODEDestroy( &lin->_ode );
}

/* destroy working space a linear system. */
static void _dzLinDestroy(dzLin *lin)
{
  zMatFree( lin->a );
  zVecFree( lin->b );
  zVecFree( lin->c );
  zVecFree( lin->x );
}

/* destroy general linear system. */
void dzLinDestroy(dzLin *lin)
{
  _dzLinDestroy( lin );
  _dzLinDestroyODE( lin );
}

/* allocate internal working space of a linear system. */
static bool _dzLinAllocODE(dzLin *lin)
{
  int dim;

  dim = dzLinDim( lin );
  lin->_ax = zVecAlloc( dim );
  lin->_bu = zVecAlloc( dim );
  zODEAssign( &lin->_ode, RKG, NULL, NULL ); /* Runge-Kutta-Gill's method */
  if( !lin->_ax || !lin->_bu ||
      !zODEInit( &lin->_ode, dim, 0, __dz_lin_state_dif ) ){
    _dzLinDestroyODE( lin );
    return false;
  }
  return true;
}

/* allocate working space of a linear system. */
bool dzLinAlloc(dzLin *lin, int dim)
{
  lin->a = zMatAllocSqr( dim );
  lin->b = zVecAlloc( dim );
  lin->c = zVecAlloc( dim );
  lin->d = 0;
  lin->x = zVecAlloc( dim );
  if( !lin->a || !lin->b || !lin->c || !lin->x || !_dzLinAllocODE( lin ) ){
    _dzLinDestroy( lin );
    return false;
  }
  return true;
}

/* check size consistency of a linear system. */
static bool _dzLinCheckSize(dzLin *lin)
{
  return zMatIsSqr( lin->a ) &&
         zMatColVecSizeEqual( lin->a, lin->b ) &&
         zMatRowVecSizeEqual( lin->a, lin->c ) ? true : false;
}

/* update the inner state of linear system. */
void dzLinStateUpdate(dzLin *c, double input, double dt)
{
  zVecMul( c->b, input, c->_bu );
  zODEUpdate( &c->_ode, 0, c->x, dt, c );
}

/* update the inner state of linear observer. */
void dzLinObsUpdate(dzLin *c, zVec k, double input, double error, double dt)
{
  zVecMul( c->b, input, c->_bu );
  zVecCatDRC( c->_bu, -error, k );
  zODEUpdate( &c->_ode, 0, c->x, dt, c );
}

/* calculation of the output of linear system. */
double dzLinOutput(dzLin *c, double input)
{
  return zVecInnerProd( c->c, c->x ) + c->d * input;
}

/* state feedback for linear system. */
double dzLinStateFeedback(dzLin *c, zVec ref, zVec f)
{
  return ( ref ? zVecInnerProd(f,ref) : 0 ) - zVecInnerProd(f,c->x);
}

/* preparation for dzLinCtrlMat and dzLinObsMat. */
static bool _dzLinCOMatPrep(dzLin *c, zMat m, int size, zVec *v)
{
  if( zMatRowSize(m) != zMatRowSize(c->a) ||
      zMatColSize(m) != zMatColSize(c->a) ){
    ZRUNERROR( ZM_ERR_MAT_SIZEMISMATCH );
    return false;
  }
  return ( *v = zVecAlloc( size ) ) ? true : false;
}

/* create controllable matrix. */
zMat dzLinCtrlMat(dzLin *c, zMat m)
{
  zVec v;
  int i = 0;

  if( !_dzLinCOMatPrep( c, m, zVecSize(c->b), &v ) )
    return NULL;
  zVecCopyNC( c->b, v );
  while( 1 ){
    zMatPutColNC( m, i, v );
    if( ++i >= zMatColSize(m) ) break;
    zMulMatVecDRC( c->a, v );
  }
  zVecFree( v );
  return m;
}

/* create observable matrix. */
zMat dzLinObsMat(dzLin *c, zMat m)
{
  zVec v;
  int i = 0;

  if( !_dzLinCOMatPrep( c, m, zVecSize(c->c), &v ) )
    return NULL;
  zVecCopyNC( c->c, v );
  while( 1 ){
    zMatPutRowNC( m, i, v );
    if( ++i >= zMatRowSize(m) ) break;
    zMulMatTVecDRC( c->a, v );
  }
  zVecFree( v );
  return m;
}

/* internal operation for dzLinIsCtrl and dzLinIsObs. */
static double _dzLinCODet(dzLin *c, zMat (*func)(dzLin*,zMat))
{
  zMat m;
  double amax, val;

  if( !( m = zMatAlloc( zMatRowSize(c->a), zMatColSize(c->a) ) ) ){
    ZALLOCERROR();
    return false;
  }
  func( c, m );
  amax = zDataAbsMax( zMatBuf(m), zMatRowSize(m) * zMatColSize(m), NULL );
  val = zMatDet(m) / pow( amax, dzLinDim(c)-1 );
  zMatFree( m );
  return val;
}

/* internal operation for dzLinIsCtrl and dzLinIsObs. */
static bool _dzLinIsCO(dzLin *c, zMat (*func)(dzLin*,zMat))
{
  return !zIsTiny( _dzLinCODet( c, func ) ) ? true : false;
}

/* value to check if the linear system is controllable. */
double dzLinCtrlDet(dzLin *c)
{
  return _dzLinCODet( c, dzLinCtrlMat );
}

/* check if the linear system is controllable. */
bool dzLinIsCtrl(dzLin *c)
{
  return _dzLinIsCO( c, dzLinCtrlMat );
}

/* value to check if the linear system is observable. */
double dzLinObsDet(dzLin *c)
{
  return _dzLinCODet( c, dzLinObsMat );
}

/* check if the linear system is observable. */
bool dzLinIsObs(dzLin *c)
{
  return _dzLinIsCO( c, dzLinObsMat );
}

/* transformation matrix of a linear system to controllable canonical form.
 * NOTE: t is in fact T^-1.
 */
zMat dzLinCtrlCanon(dzLin *c, zMat t)
{
  zMat uc, uc_inv;
  double *ap, *tp;
  int i;

  uc = zMatAllocSqr( dzLinDim(c) );
  uc_inv = zMatAllocSqr( dzLinDim(c) );
  if( !uc || !uc_inv ){
    t = NULL;
    goto TERMINATE;
  }

  dzLinCtrlMat( c, uc );
  if( !zMatInv( uc, uc_inv ) ){
    ZRUNERROR( DZ_ERR_LIN_UNCTRL );
    t = NULL;
    goto TERMINATE;
  }
  ap = zMatBuf( c->a );
  tp = zMatBuf( t );
  zRawMatGetRow( zMatBuf(uc_inv), zMatRowSizeNC(t), zMatColSizeNC(t),
    zMatRowSizeNC(t)-1, tp );
  for( i=1; i<zMatRowSizeNC(t); i++, tp+=zMatColSizeNC(t) )
    zRawMulMatTVec( ap, tp, zMatRowSizeNC(t), zMatColSizeNC(t), tp+zMatColSizeNC(t) );

 TERMINATE:
  zMatFree( uc );
  zMatFree( uc_inv );
  return t;
}

/* pole assignment for linear system control. */
zVec dzLinPoleAssign(dzLin *c, zVec pole, zVec f)
{
  zMat a, tmp, t;
  zPex eig = NULL;
  double *ap;
  int i, dim;

  a = zMatAllocSqr( dzLinDim(c) );
  tmp = zMatAllocSqr( dzLinDim(c) );
  t = zMatAllocSqr( dzLinDim(c) );
  if( !a || !tmp || !t ){
    f = NULL;
    goto TERMINATE;
  }
  if( !dzLinCtrlCanon( c, t ) ){
    ZRUNERROR( DZ_ERR_LIN_UNASSIGNABLE_POLE );
    f = NULL;
    goto TERMINATE;
  }

  zMulMatInvMat( c->a, t, tmp );
  zMulMatMat( t, tmp, a );
  if( !( eig = zPexExp( pole ) ) ){
    f = NULL;
    goto TERMINATE;
  }
  ap = zMatRowBuf( a, zMatRowSizeNC(a)-1 );
  dim = zPexDim(eig);
  for( i=0; i<dim; i++, ap++ )
    zVecSetElem( f, i, zPexCoeff(eig,i) + *ap );
  zMulMatTVecDRC( t, f );

 TERMINATE:
  zMatFreeAtOnce( 3, a, tmp, t );
  zPexFree( eig );
  return f;
}

/* creation of observerof linear system. */
zVec dzLinCreateObs(dzLin *c, zVec pole, zVec k)
{
  dzLin dual_sys;

  if( !dzLinAlloc( &dual_sys, dzLinDim(c) ) ) return NULL;
  zMatTNC( c->a, dual_sys.a );
  zVecCopyNC( c->c, dual_sys.b );
  k = dzLinPoleAssign( &dual_sys, pole, k );
  dzLinDestroy( &dual_sys );
  return k;
}

/* residual matrix of Riccati equation.
 * (working memories are manually provided.)
 */
double dzLinRiccatiErrorDRC(zMat p, dzLin *c, zMat q, double r, zMat res, zMat tmp, zVec pb)
{
  zMulMatVecNC( p, c->b, pb );
  zMulMatMatNC( p, c->a, res );
  zMulMatTMatNC( c->a, p, tmp );
  zMatAddNCDRC( res, tmp );
  zMatAddNCDRC( res, q );
  zMatCatDyadNC( res, -r, pb, pb );
  return zMatNorm( res );
}

/* residual matrix of Riccati equation. */
double dzLinRiccatiError(zMat p, dzLin *c, zMat q, double r, zMat e)
{
  zMat tmp, res;
  zVec pb;
  double err;

  tmp = zMatAllocSqr( zMatRowSizeNC(p) );
  res = e ? e : zMatAllocSqr( zMatRowSizeNC(p) );
  pb = zVecAlloc( zVecSizeNC(c->b) );
  err = dzLinRiccatiErrorDRC( p, c, q, r, res, tmp, pb );
  if( !e ) zMatFree( res );
  zMatFree( tmp );
  zVecFree( pb );
  return err;
}

/* solve Riccati's equation by numerical Euler integration. */
zMat dzLinRiccatiSolveEuler(zMat p, dzLin *c, zMat q, double r, double tol, int iter)
{
#define DZ_RICCATI_DT 0.01
  int i;
  zMat res, tmp;
  zVec pb;
  double err, err_old = HUGE_VAL;

  res = zMatAllocSqr( zMatRowSizeNC(p) );
  tmp = zMatAllocSqr( zMatRowSizeNC(p) );
  pb  = zVecAlloc( zVecSizeNC(c->b) );
  if( !res || !tmp || !pb ) goto TERMINATE;
  ZITERINIT( iter );
  zMatCopyNC( q, p ); /* initial value */
  for( i=0; ; i++ ){
    err = dzLinRiccatiErrorDRC( p, c, q, r, res, tmp, pb );
    if( fabs( err - err_old ) < tol ) break;
    err_old = err;
    zMatCatNCDRC( p, DZ_RICCATI_DT, res );
    if( i >= iter ){
      ZITERWARN( iter );
      break;
    }
  }
 TERMINATE:
  zMatFree( res );
  zMatFree( tmp );
  zVecFree( pb );
  return p;
}

/* solve Riccati's equation by Kleinman's method (1967). */
zMat dzLinRiccatiSolveKleinman(zMat p, zVec f, dzLin *c, zMat q, double r, double tol, int iter)
{
  int i;
  zMat ae, qe;
  zVec pb, _f;
  double err, err_old = HUGE_VAL;
  double mag;

  ae = zMatAllocSqr( zMatRowSizeNC(p) );
  qe = zMatAllocSqr( zMatRowSizeNC(p) );
  pb = zVecAlloc( zVecSizeNC(c->b) );
  _f = f ? f : zVecAlloc( zVecSizeNC(c->b) );
  if( !ae || !qe || !pb || !_f ) goto TERMINATE;
  ZITERINIT( iter );
  /* initial value */
  mag = zMatNorm( c->a );
  zVecLinSpace( pb, -mag, -2*mag );
  dzLinPoleAssign( c, pb, _f );
  for( i=0; ; i++ ){
    zMatCopyNC( c->a, ae );
    zMatSubDyadNC( ae, c->b, _f );
    zMatRevNC( q, qe );
    zMatCatDyadNC( qe, -r, _f, _f );
    zLyapnovSolve( ae, qe, p );
    zMulMatTVec( p, c->b, _f );
    zVecDivDRC( _f, r );
    err = dzLinRiccatiErrorDRC( p, c, q, r, ae, qe, pb );
    if( fabs( err - err_old ) < tol ) break;
    err_old = err;
    if( i >= iter ){
      ZITERWARN( iter );
      break;
    }
  }
 TERMINATE:
  zMatFree( ae );
  zMatFree( qe );
  zVecFree( pb );
  if( !f ) zVecFree( _f );
  return p;
}

/* linear quadratic optimal regulator. */
zVec dzLinLQR(dzLin *c, zVec q, double r, zVec f)
{
  zMat _q, _r, _p;

  _p = zMatAllocSqr( dzLinDim(c) );
  _q = zMatAllocSqr( dzLinDim(c) );
  _r = zMatAllocSqr( dzLinDim(c) );
  if( !_p || !_q || !_r || !zMatDiag( _q, q ) ){
    f = NULL;
    goto TERMINATE;
  }
  zVecDyad( c->b, c->b, _r );
  zMatDivDRC( _r, r );
  if( !dzLinRiccatiSolveKleinman( _p, f, c, _q, r, zTOL, 0 ) ){
    if( !dzLinRiccatiSolveEuler( _p, c, _q, r, zTOL, 0 ) ){
      f = NULL;
      goto TERMINATE;
    }
    zMulMatTVec( _p, c->b, f );
    zVecDivDRC( f, r );
  }

 TERMINATE:
  zMatFree( _p );
  zMatFree( _q );
  zMatFree( _r );
  return f;
}

/* conversion from polynomial transfer function to linear system
 * in controllable / observable canonical form.
 */
static dzLin *_dzTF2LinCtrlCanon_tf(dzTF *tf, dzLin *lin)
{
  int i, n;
  double a;

  n = dzTFDenDim(tf) - 1;
  if( !dzLinAlloc( lin, n+1 ) ){
    ZRUNERROR( DZ_ERR_LIN_UNCONVERTIBLE_TF );
    return NULL;
  }
  for( i=1; i<=n; i++ )
    zMatSetElem( lin->a, i-1, i, 1.0 );
  a = dzTFDenElem( tf, n+1 );
  for( i=0; i<=n; i++ )
    zMatSetElem( lin->a, n, i, -dzTFDenElem( tf, i ) / a );
  zVecSetElem( lin->b, n, 1.0 );
  for( i=0; i<=n; i++ )
    zVecSetElem( lin->c, i, dzTFNumElem( tf, i ) / a );
  return lin;
}

static dzLin *_dzTF2LinObsCanon_tf(dzTF *tf, dzLin *lin)
{
  int i, n;
  double a;

  n = dzTFDenDim(tf) - 1;
  if( !dzLinAlloc( lin, n+1 ) ){
    ZRUNERROR( DZ_ERR_LIN_UNCONVERTIBLE_TF );
    return NULL;
  }
  for( i=1; i<=n; i++ )
    zMatSetElem( lin->a, i, i-1, 1.0 );
  a = dzTFDenElem( tf, n+1 );
  for( i=0; i<=n; i++ )
    zMatSetElem( lin->a, i, n, -dzTFDenElem( tf, i ) / a );
  for( i=0; i<=n; i++ )
    zVecSetElem( lin->b, i, dzTFNumElem( tf, i ) / a );
  zVecSetElem( lin->c, n, 1.0 );
  return lin;
}

static dzLin *_dzTF2LinCanon_non_tf(dzTF *tf, dzLin *lin, dzLin *(*tf2lin_tf)(dzTF*,dzLin*))
{
  zPex q, org_num, new_num;

  if( !zPexDiv( dzTFNum(tf), dzTFDen(tf), &q, &new_num ) ){
    ZALLOCERROR();
    lin = NULL;
    goto TERMINATE;
  }
  if( zPexDim(q) != 0 ){
    ZRUNERROR( DZ_ERR_TF_INVALID_DEN );
    zPexFPrint( stderr, dzTFDen(tf) );
    goto TERMINATE;
  }
  org_num = dzTFNum(tf);
  dzTFSetNum( tf, new_num );
  tf2lin_tf( tf, lin );
  dzTFSetNum( tf, org_num );
  lin->d = zPexCoeff( q, 0 );
 TERMINATE:
  zPexFree( q );
  zPexFree( new_num );
  return lin;
}

static dzLin *_dzTF2LinCanon(dzTF *tf, dzLin *lin, dzLin *(*tf2lin_tf)(dzTF*,dzLin*))
{
  if( dzTFDenDim(tf) < dzTFNumDim(tf) ){ /* non-proper case */
    ZRUNERROR( DZ_ERR_TF_NONPROPER );
    return NULL;
  }
  if( dzTFDenDim(tf) == dzTFNumDim(tf) ) /* not-strictly-proper case */
    return _dzTF2LinCanon_non_tf( tf, lin, tf2lin_tf );
  return tf2lin_tf( tf, lin );
}

dzLin *dzTF2LinCtrlCanon(dzTF *tf, dzLin *lin){
  return _dzTF2LinCanon( tf, lin, _dzTF2LinCtrlCanon_tf );
}

dzLin *dzTF2LinObsCanon(dzTF *tf, dzLin *lin){
  return _dzTF2LinCanon( tf, lin, _dzTF2LinObsCanon_tf );
}

/* ZTK */

static void *_dzLinAFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzLin*)obj)->a = zMatFromZTK( ztk ) ) ? obj : NULL;
}
static void *_dzLinBFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzLin*)obj)->b = zVecFromZTK( ztk ) ) ? obj : NULL;
}
static void *_dzLinCFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzLin*)obj)->c = zVecFromZTK( ztk ) ) ? obj : NULL;
}
static void *_dzLinDFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  ((dzLin*)obj)->d = ZTKDouble(ztk);
  return obj;
}

static bool _dzLinAFPrintZTK(FILE *fp, int i, void *prp){
  zMatFPrint( fp, ((dzLin*)prp)->a );
  return true;
}
static bool _dzLinBFPrintZTK(FILE *fp, int i, void *prp){
  zVecFPrint( fp, ((dzLin*)prp)->b );
  return true;
}
static bool _dzLinCFPrintZTK(FILE *fp, int i, void *prp){
  zVecFPrint( fp, ((dzLin*)prp)->c );
  return true;
}
static bool _dzLinDFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", ((dzLin*)prp)->d );
  return true;
}

static const ZTKPrp __ztk_prp_dzlin[] = {
  { ZTK_KEY_DZCO_LIN_A, 1, _dzLinAFromZTK, _dzLinAFPrintZTK },
  { ZTK_KEY_DZCO_LIN_B, 1, _dzLinBFromZTK, _dzLinBFPrintZTK },
  { ZTK_KEY_DZCO_LIN_C, 1, _dzLinCFromZTK, _dzLinCFPrintZTK },
  { ZTK_KEY_DZCO_LIN_D, 1, _dzLinDFromZTK, _dzLinDFPrintZTK },
};

dzLin *dzLinFromZTK(dzLin *lin, ZTK *ztk)
{
  dzLinInit( lin );
  if( !ZTKEvalKey( lin, NULL, ztk, __ztk_prp_dzlin ) ) return NULL;
  if( !lin->a || !lin->b || !lin->c ){
    _dzLinDestroy( lin );
    return NULL;
  }
  if( !_dzLinCheckSize( lin ) ){
    ZRUNERROR( DZ_ERR_LIN_SIZMIS );
    return NULL;
  }
  if( !( lin->x = zVecAlloc( zVecSize(lin->c) ) ) ||
      !_dzLinAllocODE( lin ) ){
    _dzLinDestroy( lin );
    return NULL;
  }
  return lin;
}

void dzLinFPrintZTK(FILE *fp, dzLin *lin)
{
  ZTKPrpKeyFPrint( fp, lin, __ztk_prp_dzlin );
}
