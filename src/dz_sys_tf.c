/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_tf - transfer function by polynomial rational expression
 */

#include <dzco/dz_sys.h>
#include <dzco/dz_lin.h>

typedef struct{
  int n;
  double *z; /* state variable */
  double *a; /* transient coefficient */
  double *c; /* output coefficient */
  double d;  /* output gain */
  dzTF *tf; /* original polynomial rational (only for memory) */
} dzSysTFPrm;

static void _dzSysTFPrmFree(dzSysTFPrm *prm)
{
  zFree( prm->z );
  zFree( prm->a );
  zFree( prm->c );
  dzTFDestroy( prm->tf );
  zFree( prm );
}

static dzSysTFPrm *_dzSysTFPrmAlloc(int n)
{
  dzSysTFPrm *prm;

  if( !( prm = zAlloc( dzSysTFPrm, 1 ) ) ) return NULL;
  prm->z = zAlloc( double, n );
  prm->a = zAlloc( double, n );
  prm->c = zAlloc( double, n );
  if( !prm->z || !prm->a || !prm->c ){
    _dzSysTFPrmFree( prm );
    return NULL;
  }
  prm->n = n;
  return prm;
}

static void _dzSysTFDestroy(dzSys *sys)
{
  zArrayFree( dzSysInput(sys) );
  zVecFree( dzSysOutput(sys) );
  _dzSysTFPrmFree( (dzSysTFPrm *)sys->prp );
  zNameFree( sys );
  dzSysInit( sys );
}

static void _dzSysTFRefresh(dzSys *sys)
{
  memset( ((dzSysTFPrm*)sys->prp)->z, 0, sizeof(double)*((dzSysTFPrm*)sys->prp)->n );
}

static zVec _dzSysTFUpdate(dzSys *sys, double dt)
{
  int i;
  dzSysTFPrm *prm;
  double v;

  prm = (dzSysTFPrm *)sys->prp;
  dzSysOutputVal(sys,0) =
    zRawVecInnerProd( prm->c, prm->z, prm->n ) + prm->d*dzSysInputVal(sys,0);
  v = zRawVecInnerProd( prm->a, prm->z, prm->n ) + dzSysInputVal(sys,0);
  for( i=1; i<prm->n; i++ )
    prm->z[i-1] += prm->z[i] * dt;
  prm->z[prm->n-1] += v * dt;
  return dzSysOutput(sys);
}

static void dzSysTFFPrintZTK(FILE *fp, dzSys *sys)
{
  dzTFFPrintZTK( fp, ((dzSysTFPrm*)sys->prp)->tf );
}

static dzSys *_dzSysTFFromZTK(dzSys *sys, ZTK *ztk)
{
  dzTF *tf;

  if( !( tf = zAlloc( dzTF, 1 ) ) ) return NULL;
  if( !dzTFFromZTK( tf, ztk ) ) return NULL;
  return dzSysTFCreate( sys, tf );
}

dzSysCom dz_sys_tf_com = {
  .typestr = "tf",
  ._destroy = _dzSysTFDestroy,
  ._refresh = _dzSysTFRefresh,
  ._update = _dzSysTFUpdate,
  ._fromZTK = _dzSysTFFromZTK,
  ._fprintZTK = dzSysTFFPrintZTK,
};

/* create a transfer function from a polynomial rational expression
 * as an infinite impulse response system. */
dzSys *dzSysTFCreate(dzSys *sys, dzTF *tf)
{
  dzLin lin;
  dzSysTFPrm *prm;

  if( !dzTF2LinCtrlCanon( tf, &lin ) ){
    ZRUNERROR( DZ_ERR_SYS_TF_UNABLE_CONV );
    return NULL;
  }
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( prm = _dzSysTFPrmAlloc( dzLinDim(&lin) ) ) )
    goto TERMINATE;
  zRawVecCopy( zMatRowBufNC(lin.a,prm->n-1), prm->a, prm->n );
  zRawVecCopy( zVecBufNC(lin.c), prm->c, prm->n );
  prm->d = lin.d;
  prm->tf = tf;

  sys->prp = prm;
  sys->com = &dz_sys_tf_com;
  dzSysRefresh( sys );
 TERMINATE:
  dzLinDestroy( &lin );
  return sys;
}
