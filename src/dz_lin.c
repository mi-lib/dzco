/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_lin - linear system
 */

#include <dzco/dz_lin.h>

/* ********************************************************** */
/* CLASS: dzLin
 * general linear system
 * ********************************************************** */

static bool _dzLinAllocODE(dzLin *c);
static void _dzLinDestroy(dzLin *c);
static void _dzLinDestroyODE(dzLin *c);
static bool _dzLinCheckSize(dzLin *lin);

static zVec __dz_lin_state_dif(double t, zVec x, void *sys, zVec dx);
static bool _dzLinCOMatPrep(dzLin *c, zMat m, uint size, zMat *af, zVec *v);

static bool _dzLinFRead(FILE *fp, void *instance, char *buf, bool *success);

/* dzLinInit
 * - initialize a linear system.
 */
dzLin *dzLinInit(dzLin *lin)
{
  lin->a = NULL;
  lin->b = lin->c = lin->x = NULL;
  lin->d = 0;
  lin->_ax = lin->_bu = NULL;
  return lin;
}

/* dzLinAlloc
 * - allocate working space of a linear system.
 */
bool dzLinAlloc(dzLin *lin, int dim)
{
  lin->a = zMatAllocSqr( dim );
  lin->b = zVecAlloc( dim );
  lin->c = zVecAlloc( dim );
  lin->d = 0;
  lin->x = zVecAlloc( dim );
  if( !lin->a || !lin->b || !lin->c || !lin->x || !_dzLinAllocODE( lin ) ){
    ZRUNERROR( "cannot create a linear system" );
    _dzLinDestroy( lin );
    return false;
  }
  return true;
}

/* (static)
 * _dzLinAllocODE
 * - allocate internal working space of a linear system.
 */
bool _dzLinAllocODE(dzLin *lin)
{
  int dim;

  dim = dzLinDim( lin );
  lin->_ax = zVecAlloc( dim );
  lin->_bu = zVecAlloc( dim );
  zODEAssign( &lin->_ode, RKG, NULL, NULL ); /* Runge-Kutta-Gill's method */
  if( !lin->_ax || !lin->_bu ||
      !zODEInit( &lin->_ode, dim, 0, __dz_lin_state_dif ) ){
    ZRUNERROR( "cannot create a linear system" );
    _dzLinDestroyODE( lin );
    return false;
  }
  return true;
}

/* (static)
 * _dzLinDestroy
 * - destroy working space a linear system.
 */
void _dzLinDestroy(dzLin *lin)
{
  zMatFree( lin->a );
  zVecFree( lin->b );
  zVecFree( lin->c );
  zVecFree( lin->x );
}

/* (static)
 * _dzLinDestroyODE
 * - destroy internal working space of a linear system.
 */
void _dzLinDestroyODE(dzLin *lin)
{
  zVecFree( lin->_ax );
  zVecFree( lin->_bu );
  zODEDestroy( &lin->_ode );
}

/* dzLinDestroy
 * - destroy general linear system.
 */
void dzLinDestroy(dzLin *lin)
{
  _dzLinDestroy( lin );
  _dzLinDestroyODE( lin );
}

/* (static)
 * _dzLinCheckSize
 * - check size consistency of a linear system.
 */
bool _dzLinCheckSize(dzLin *lin)
{
  return zMatIsSqr( lin->a ) &&
         zMatColVecSizeIsEqual( lin->a, lin->b ) &&
         zMatRowVecSizeIsEqual( lin->a, lin->c ) ? true : false;
}

/* (static)
 * __dz_lin_state_dif
 * - compute state velocity.
 */
zVec __dz_lin_state_dif(double t, zVec x, void *sys, zVec dx)
{
  dzLin *lin;

  lin = sys;
  zMulMatVec( lin->a, x, lin->_ax );
  return zVecAdd( lin->_ax, lin->_bu, dx );
}

/* dzLinStateUpdate
 * - update the inner state of linear system.
 */
void dzLinStateUpdate(dzLin *c, double input, double dt)
{
  zVecMul( c->b, input, c->_bu );
  zODEUpdate( &c->_ode, 0, c->x, dt, c );
}

/* dzLinObsUpdate
 * - update the inner state of linear observer.
 */
void dzLinObsUpdate(dzLin *c, zVec k, double input, double error, double dt)
{
  zVecMul( c->b, input, c->_bu );
  zVecCatDRC( c->_bu, -error, k );
  zODEUpdate( &c->_ode, 0, c->x, dt, c );
}

/* dzLinOutput
 * - calculation of the output of linear system.
 */
double dzLinOutput(dzLin *c, double input)
{
  return zVecInnerProd( c->c, c->x ) + c->d * input;
}

/* dzLinStateFeedback
 * - state feedback for linear system.
 */
double dzLinStateFeedback(dzLin *c, zVec ref, zVec f)
{
  return ( ref ? zVecInnerProd(f,ref) : 0 ) - zVecInnerProd(f,c->x);
}

/* (static)
 * _dzLinCOMatPrep
 * - preparation for dzLinCtrlMat and dzLinObsMat.
 */
bool _dzLinCOMatPrep(dzLin *c, zMat m, uint size, zMat *af, zVec *v)
{
  if( zMatRowSize(m) != zMatRowSize(c->a) ||
      zMatColSize(m) != zMatColSize(c->a) ){
    ZRUNERROR( "size mismatch of matrices" );
    return false;
  }
  *af = zMatAlloc( zMatRowSize(c->a), zMatColSize(c->a) );
  *v  = zVecAlloc( size );
  if( !*af || !*v ){
    ZRUNERROR( "cannot create controllable matrix" );
    zMatFree( *af );
    zVecFree( *v );
    return false;
  }
  zMatIdent( *af );
  return true;
}

/* dzLinCtrlMat
 * - create controllable matrix.
 */
zMat dzLinCtrlMat(dzLin *c, zMat m)
{
  zMat af;
  zVec v;
  register uint i = 0;

  if( !_dzLinCOMatPrep( c, m, zVecSize(c->b), &af, &v ) )
    return NULL;
  while( 1 ){
    zMulMatVec( af, c->b, v );
    zMatSetCol( m, i, v );
    if( ++i >= zMatColSize(m) ) break;
    zMulMatMatDRC( c->a, af );
  }
  zMatFree( af );
  zVecFree( v );
  return m;
}

/* dzLinObsMat
 * - create observable matrix.
 */
zMat dzLinObsMat(dzLin *c, zMat m)
{
  zMat af;
  zVec v;
  register uint i = 0;

  if( !_dzLinCOMatPrep( c, m, zVecSize(c->c), &af, &v ) )
    return NULL;
  while( 1 ){
    zMulVecMat( c->c, af, v );
    zMatSetRow( m, i, v );
    if( ++i >= zMatRowSize(m) ) break;
    zMulMatMatDRC( c->a, af );
  }
  zMatFree( af );
  zVecFree( v );
  return m;
}

static bool _dzLinIsCO(dzLin *c, zMat (*func)(dzLin*,zMat));

/* (static)
 * _dzLinIsCO
 * - inner operation of dzLinIsCtrl and dzLinIsObs.
 */
bool _dzLinIsCO(dzLin *c, zMat (*func)(dzLin*,zMat))
{
  zMat m;
  bool result;

  if( !( m = zMatAlloc( zMatRowSize(c->a), zMatColSize(c->a) ) ) ){
    ZALLOCERROR();
    return false;
  }
  result = zMatDet( func(c,m) ) != 0 ? true : false;
  zMatFree( m );
  return result;
}

/* dzLinIsCtrl
 * - check if the linear system is controllable.
 */
bool dzLinIsCtrl(dzLin *c)
{
  return _dzLinIsCO( c, dzLinCtrlMat );
}

/* dzLinIsObs
 * - check if the linear system is observable.
 */
bool dzLinIsObs(dzLin *c)
{
  return _dzLinIsCO( c, dzLinObsMat );
}

/* dzLinCtrlCanon
 * - transformation matrix of a linear system to controllable
 *   canonical form.
 * NOTE: 't' is actually T^-1.
 */
zMat dzLinCtrlCanon(dzLin *c, zMat t)
{
  zMat uc, uc_inv;
  double *ap, *tp;
  register uint i;

  uc = zMatAllocSqr( dzLinDim(c) );
  uc_inv = zMatAllocSqr( dzLinDim(c) );
  if( !uc || !uc_inv ){
    t = NULL;
    goto TERMINATE;
  }

  dzLinCtrlMat( c, uc );
  if( !zMatInv( uc, uc_inv ) ){
    ZRUNERROR( "system is not controllable" );
    t = NULL;
    goto TERMINATE;
  }
  ap = zMatBuf( c->a );
  tp = zMatBuf( t );
  zRawMatGetRow( zMatBuf(uc_inv), zMatRowSizeNC(t), zMatColSizeNC(t),
    zMatRowSizeNC(t)-1, tp );
  for( i=1; i<zMatRowSizeNC(t); i++, tp+=zMatColSizeNC(t) )
    zRawMulVecMat( tp, ap, zMatRowSizeNC(t), zMatColSizeNC(t), tp+zMatColSizeNC(t) );

 TERMINATE:
  zMatFree( uc );
  zMatFree( uc_inv );
  return t;
}

/* dzLinPoleAssign
 * - pole assignment for linear system control.
 */
zVec dzLinPoleAssign(dzLin *c, zVec pole, zVec f)
{
  zMat a, t;
  zPex eig = NULL;
  double *ap;
  register int i, dim;

  a = zMatAllocSqr( dzLinDim(c) );
  t = zMatAllocSqr( dzLinDim(c) );
  if( !a || !t ){
    f = NULL;
    goto TERMINATE;
  }
  if( !dzLinCtrlCanon( c, t ) ){
    ZRUNERROR( "cannot assign desired pole" );
    f = NULL;
    goto TERMINATE;
  }

  zMulMatInvMat( c->a, t, a );
  zMulMatMatDRC( t, a );
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
  zMatFree( a );
  zMatFree( t );
  zPexFree( eig );
  return f;
}

/* dzLinCreateObs
 * - creation of observerof linear system.
 */
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

/* dzLinRiccatiErrorDRC
 * - residual matrix of Riccati equation.
 *   (working memories are manually provided.)
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

/* dzLinRiccatiErrorDRC
 * - residual matrix of Riccati equation.
 */
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

/* dzLinRiccatiSolveEuler
 * - solve Riccati's equation by numerical Euler integration.
 */
zMat dzLinRiccatiSolveEuler(zMat p, dzLin *c, zMat q, double r, double tol, int iter)
{
#define DZ_RICCATI_DT 0.01
  register int i;
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

/* dzLinRiccatiSolveKleinman
 * - solve Riccati's equation by Kleinman's method (1967).
 */
zMat dzLinRiccatiSolveKleinman(zMat p, zVec f, dzLin *c, zMat q, double r, double tol, int iter)
{
  register int i;
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
    zMulVecMat( c->b, p, _f );
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

/* dzLinLQR
 * - linear quadratic optimal regulator.
 */
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
    zMulVecMat( c->b, _p, f );
    zVecDivDRC( f, r );
  }

 TERMINATE:
  zMatFree( _p );
  zMatFree( _q );
  zMatFree( _r );
  return f;
}

/* dzPex2LinCtrlCanon, dzPex2LinObsCanon
 * - conversion from polynomial transfer function to linear system
 *   in controllable/observable canonical form.
 */
static dzLin *_dzPex2LinCtrlCanon_sp(dzPex *sp, dzLin *lin);
static dzLin *_dzPex2LinObsCanon_sp(dzPex *sp, dzLin *lin);
static dzLin *_dzPex2LinCanon_non_sp(dzPex *sp, dzLin *lin, dzLin *(*pex2lin_sp)(dzPex*,dzLin*));
static dzLin *_dzPex2LinCanon(dzPex *sp, dzLin *lin, dzLin *(*pex2lin_sp)(dzPex*,dzLin*));

dzLin *_dzPex2LinCtrlCanon_sp(dzPex *sp, dzLin *lin)
{
  register uint i, n;
  double a;

  n = dzPexDenDim(sp) - 1;
  if( !dzLinAlloc( lin, n+1 ) ){
    ZRUNERROR( "cannot convert the transfer function to linear system" );
    return NULL;
  }
  for( i=1; i<=n; i++ )
    zMatSetElem( lin->a, i-1, i, 1.0 );
  a = dzPexDenElem( sp, n+1 );
  for( i=0; i<=n; i++ )
    zMatSetElem( lin->a, n, i, -dzPexDenElem( sp, i ) / a );
  zVecSetElem( lin->b, n, 1.0 );
  for( i=0; i<=n; i++ )
    zVecSetElem( lin->c, i, dzPexNumElem( sp, i ) / a );
  return lin;
}

dzLin *_dzPex2LinObsCanon_sp(dzPex *sp, dzLin *lin)
{
  register int i, n;
  double a;

  n = dzPexDenDim(sp) - 1;
  if( !dzLinAlloc( lin, n+1 ) ){
    ZRUNERROR( "cannot convert the transfer function to linear system" );
    return NULL;
  }
  for( i=1; i<=n; i++ )
    zMatSetElem( lin->a, i, i-1, 1.0 );
  a = dzPexDenElem( sp, n+1 );
  for( i=0; i<=n; i++ )
    zMatSetElem( lin->a, i, n, -dzPexDenElem( sp, i ) / a );
  for( i=0; i<=n; i++ )
    zVecSetElem( lin->b, i, dzPexNumElem( sp, i ) / a );
  zVecSetElem( lin->c, n, 1.0 );
  return lin;
}

dzLin *_dzPex2LinCanon_non_sp(dzPex *sp, dzLin *lin, dzLin *(*pex2lin_sp)(dzPex*,dzLin*))
{
  zPex q, org_num, new_num;

  if( !zPexDiv( dzPexNum(sp), dzPexDen(sp), &q, &new_num ) ){
    ZALLOCERROR();
    lin = NULL;
    goto TERMINATE;
  }
  if( zPexDim(q) != 0 ){
    ZRUNERROR( "fatal error - wrong denominator" );
    zPexFWrite( stderr, dzPexDen(sp) );
    goto TERMINATE;
  }
  org_num = dzPexNum(sp);
  dzPexSetNum( sp, new_num );
  pex2lin_sp( sp, lin );
  dzPexSetNum( sp, org_num );
  lin->d = zPexCoeff( q, 0 );
 TERMINATE:
  zPexFree( q );
  zPexFree( new_num );
  return lin;
}

dzLin *_dzPex2LinCanon(dzPex *sp, dzLin *lin, dzLin *(*pex2lin_sp)(dzPex*,dzLin*))
{
  if( dzPexDenDim(sp) < dzPexNumDim(sp) ){ /* non-proper case */
    ZRUNERROR( "system is not proper" );
    return NULL;
  }
  if( dzPexDenDim(sp) == dzPexNumDim(sp) ) /* not-strictly-proper case */
    return _dzPex2LinCanon_non_sp( sp, lin, pex2lin_sp );
  return pex2lin_sp( sp, lin );
}

dzLin *dzPex2LinCtrlCanon(dzPex *sp, dzLin *lin){
  return _dzPex2LinCanon( sp, lin, _dzPex2LinCtrlCanon_sp );
}

dzLin *dzPex2LinObsCanon(dzPex *sp, dzLin *lin){
  return _dzPex2LinCanon( sp, lin, _dzPex2LinObsCanon_sp );
}

/* (static)
 * _dzLinFRead
 * - read a linear system from file.
 */
bool _dzLinFRead(FILE *fp, void *instance, char *buf, bool *success)
{
  if( strcmp( buf, "a" ) == 0 ){
    ((dzLin *)instance)->a = zMatFRead( fp );
  } else
  if( strcmp( buf, "b" ) == 0 ){
    ((dzLin *)instance)->b = zVecFRead( fp );
  } else
  if( strcmp( buf, "c" ) == 0 ){
    ((dzLin *)instance)->c = zVecFRead( fp );
  } else
  if( strcmp( buf, "d" ) == 0 ){
    ((dzLin *)instance)->d = zFDouble( fp );
  } else
    return false;
  return true;
}

/* dzLinFRead
 * - read a linear system from file.
 */
dzLin *dzLinFRead(FILE *fp, dzLin *lin)
{
  dzLinInit( lin );
  zFieldFRead( fp, _dzLinFRead, lin );
  if( !lin->a || !lin->b || !lin->c ){
    ZALLOCERROR();
    _dzLinDestroy( lin );
    return NULL;
  }
  if( !_dzLinCheckSize( lin ) ){
    ZRUNERROR( "inconsistent system dimension" );
    return NULL;
  }
  if( !( lin->x = zVecAlloc( zVecSize(lin->c) ) ) ||
      !_dzLinAllocODE( lin ) ){
    ZALLOCERROR();
    _dzLinDestroy( lin );
    return NULL;
  }
  return lin;
}

/* dzLinFWrite
 * - write a linear system to file.
 */
void dzLinFWrite(FILE *fp, dzLin *lin)
{
  fprintf( fp, "a: " ); zMatFWrite( fp, lin->a );
  fprintf( fp, "b: " ); zVecFWrite( fp, lin->b );
  fprintf( fp, "c: " ); zVecFWrite( fp, lin->c );
  fprintf( fp, "d: %g\n", lin->d );
}
