/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_filt_bw - Butterworth filter
 */

#include <dzco/dz_sys.h>

/*! \brief first-order Butterworth filter */
typedef struct{
  double out;
} _dzBW1;

static void _dzBW1Refresh(_dzBW1 *p)
{
  p->out = 0;
}

static double _dzBW1Update(_dzBW1 *p, double wt, double input)
{
  return ( p->out = ( p->out + wt * input ) / ( 1 + wt ) );
}

/*! \brief second-order Butterworth filter */
typedef struct{
  double zeta;
  double out;
  double prevout;
  double prevdt;
} _dzBW2;

static void _dzBW2Refresh(_dzBW2 *p)
{
  p->out = p->prevout = 0;
  p->prevdt = 1.0; /* dummy */
}

static void _dzBW2Create(_dzBW2 *p, int i, int dim)
{
  p->zeta = sin( zPI * (double)( 2*i + 1.0 ) / dim );
  _dzBW2Refresh( p );
}

static double _dzBW2Update(_dzBW2 *p, double wt, double input, double dt)
{
  double tr, zwt2, ret;

  tr = dt / p->prevdt;
  zwt2 = 1 + 2 * p->zeta * wt;
  ret = ( (zwt2+tr)*p->out - tr*p->prevout + wt*wt*input ) / ( zwt2 + wt*wt );
  p->prevout = p->out;
  p->prevdt = dt;
  return ( p->out = ret );
}

/* N order Butterworth filter is a combination of
 * first order & second order Butterworth filters.
 */
typedef struct{
  uint n1, n2;
  _dzBW1 *f1;
  _dzBW2 *f2;
  double wc;
  double cf; /* only for memory */
  uint dim;   /* only for memory */
} _dzBW;

static void _dzBWDestroy(_dzBW *bw)
{
  if( bw->n1 > 0 ) zFree( bw->f1 );
  if( bw->n2 > 0 ) zFree( bw->f2 );
  bw->n1 = bw->n2 = 0;
}

static bool _dzBWCreate(_dzBW *bw, double cf, int dim)
{
  uint i;

  if( dim == 0 ){
    ZRUNERROR( DZ_ERR_SYS_BW_ZEROORDER );
    return false;
  }
  bw->n1 = ( bw->dim = dim ) % 2;
  bw->n2 = ( dim - bw->n1 ) / 2;
  bw->f1 = bw->n1 > 0 ? zAlloc( _dzBW1, bw->n1 ) : NULL;
  bw->f2 = bw->n2 > 0 ? zAlloc( _dzBW2, bw->n2 ) : NULL;
  if( ( bw->n1 > 0 && !bw->f1 ) || ( bw->n2 > 0 && !bw->f2 ) ){
    ZALLOCERROR();
    _dzBWDestroy( bw );
    return false;
  }
  bw->wc = 2 * zPI * ( bw->cf = cf );
  for( i=0; i<bw->n2; i++ )
    _dzBW2Create( &bw->f2[i], i, dim );
  return true;
}

static double _dzBWUpdate(_dzBW *bw, double input, double dt)
{
  double wt, output;
  uint i;

  wt = bw->wc * dt;
  if( bw->n1 > 0 ){
    input = _dzBW1Update( bw->f1, wt, input );
  }
  if( bw->n2 > 0 ){
    _dzBW2Update( &bw->f2[0], wt, input, dt );
    for( i=1; i<bw->n2; i++ ){
      _dzBW2Update( &bw->f2[i], wt, bw->f2[i-1].out, dt );
    }
    output = bw->f2[bw->n2-1].out;
  } else
    output = bw->f1->out;
  return output;
}

/* destroy a Butterworth filter. */
void dzSysBWDestroy(dzSys *sys)
{
  zArrayFree( dzSysInput(sys) );
  zVecFree( dzSysOutput(sys) );
  if( sys->prp ){
    _dzBWDestroy( (_dzBW *)sys->prp );
    zFree( sys->prp );
  }
  zNameFree( sys );
  dzSysInit( sys );
}

/* refresh the internal state of a Butterworth filter. */
void dzSysBWRefresh(dzSys *sys)
{
  _dzBW *bw;
  uint i;

  bw = (_dzBW *)sys->prp;
  if( bw->f1 )
    _dzBW1Refresh( bw->f1 );
  for( i=0; i<bw->n2; i++ )
    _dzBW2Refresh( &bw->f2[i] );
}

/* update a Butterworth filter. */
zVec dzSysBWUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = _dzBWUpdate( (_dzBW *)sys->prp, dzSysInputVal(sys,0), dt );
  return dzSysOutput(sys);
}

typedef struct{
  double cf;
  uint dim;
} _dzBWParam;

static void *_dzSysBWCFFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((_dzBWParam*)val)->cf = ZTKDouble(ztk);
  return val;
}
static void *_dzSysBWDimFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((_dzBWParam*)val)->dim = ZTKDouble(ztk);
  return val;
}

static bool _dzSysBWCFFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", ((_dzBW*)((dzSys*)prp)->prp)->cf );
  return true;
}
static bool _dzSysBWDimFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%d\n", ((_dzBW*)((dzSys*)prp)->prp)->dim );
  return true;
}

static ZTKPrp __ztk_prp_dzsys_bw[] = {
  { ZTK_KEY_DZCO_SYS_CUTOFFFREQ, 1, _dzSysBWCFFromZTK, _dzSysBWCFFPrintZTK },
  { ZTK_KEY_DZCO_SYS_DIM,        1, _dzSysBWDimFromZTK, _dzSysBWDimFPrintZTK },
};

static dzSys *_dzSysBWFromZTK(dzSys *sys, ZTK *ztk)
{
  _dzBWParam prm = { 1.0, 1 };
  if( !ZTKEvalKey( &prm, NULL, ztk, __ztk_prp_dzsys_bw ) ) return NULL;
  return dzSysBWCreate( sys, prm.cf, prm.dim );
}

static void _dzSysBWFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_bw );
}

dzSysCom dz_sys_bw_com = {
  .typestr = "butterworth",
  ._destroy = dzSysBWDestroy,
  ._refresh = dzSysBWRefresh,
  ._update = dzSysBWUpdate,
  ._fromZTK = _dzSysBWFromZTK,
  ._fprintZTK = _dzSysBWFPrintZTK,
};

/* create a Butterworth filter. */
dzSys *dzSysBWCreate(dzSys *sys, double cf, uint dim)
{
  dzSysInit( sys );
  sys->com = &dz_sys_bw_com;
  dzSysAllocInput( sys, 1 );
  return dzSysInputNum(sys) == 1 &&
         dzSysAllocOutput( sys, 1 ) &&
         ( sys->prp = zAlloc( _dzBW, 1 ) ) &&
         _dzBWCreate( (_dzBW *)sys->prp, cf, dim ) ? sys : NULL;
}
