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

static void _dzBW1Refresh(_dzBW1 *p);
static double _dzBW1Update(_dzBW1 *p, double wt, double input);

void _dzBW1Refresh(_dzBW1 *p)
{
  p->out = 0;
}

double _dzBW1Update(_dzBW1 *p, double wt, double input)
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

static void _dzBW2Refresh(_dzBW2 *p);
static void _dzBW2Create(_dzBW2 *p, int i, int dim);
static double _dzBW2Update(_dzBW2 *p, double wt, double input, double dt);

void _dzBW2Refresh(_dzBW2 *p)
{
  p->out = p->prevout = 0;
  p->prevdt = 1.0; /* dummy */
}

void _dzBW2Create(_dzBW2 *p, int i, int dim)
{
  p->zeta = sin( zPI * (double)( 2*i + 1.0 ) / dim );
  _dzBW2Refresh( p );
}

double _dzBW2Update(_dzBW2 *p, double wt, double input, double dt)
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
  int n1, n2;
  _dzBW1 *f1;
  _dzBW2 *f2;
  double wc;
  double cf; /* only for memory */
  int dim;   /* only for memory */
} _dzBW;

static bool _dzBWCreate(_dzBW *bw, double cf, int dim);
static void _dzBWDestroy(_dzBW *bw);
static double _dzBWUpdate(_dzBW *bw, double input, double dt);

void _dzBWDestroy(_dzBW *bw)
{
  if( bw->n1 > 0 ) zFree( bw->f1 );
  if( bw->n2 > 0 ) zFree( bw->f2 );
  bw->n1 = bw->n2 = 0;
}

bool _dzBWCreate(_dzBW *bw, double cf, int dim)
{
  register int i;

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

double _dzBWUpdate(_dzBW *bw, double input, double dt)
{
  double wt, output;
  register int i;

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

/* dzSysDestroyBW
 * - destroy a Butterworth filter.
 */
void dzSysDestroyBW(dzSys *sys)
{
  zArrayFree( dzSysInput(sys) );
  zVecFree( dzSysOutput(sys) );
  if( sys->_prm ){
    _dzBWDestroy( sys->_prm );
    zFree( sys->_prm );
  }
  zNameFree( sys );
  dzSysInit( sys );
}

/* dzSysRefreshBW
 * - refresh the internal state of a Butterworth filter.
 */
void dzSysRefreshBW(dzSys *sys)
{
  _dzBW *bw;
  register int i;

  bw = sys->_prm;
  if( bw->f1 )
    _dzBW1Refresh( bw->f1 );
  for( i=0; i<bw->n2; i++ )
    _dzBW2Refresh( &bw->f2[i] );
}

/* dzSysUpdateBW
 * - update a Butterworth filter.
 */
zVec dzSysUpdateBW(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = _dzBWUpdate( sys->_prm, dzSysInputVal(sys,0), dt );
  return dzSysOutput(sys);
}

typedef struct{
  double cf;
  int dim;
} _dzBWParam;

static bool _dzSysFScanBW(FILE *fp, void *prm, char *buf, bool *success);

bool _dzSysFScanBW(FILE *fp, void *prm, char *buf, bool *success)
{
  if( strcmp( buf, "cf" ) == 0 ){
    ((_dzBWParam *)prm)->cf = zFDouble( fp );
  } else
  if( strcmp( buf, "dim" ) == 0 ){
    ((_dzBWParam *)prm)->dim = zFInt( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanBW(FILE *fp, dzSys *sys)
{
  _dzBWParam prm = { 1.0, 1 };

  zFieldFScan( fp, _dzSysFScanBW, &prm );
  return dzSysCreateBW( sys, prm.cf, prm.dim ) ? sys : NULL;
}

void dzSysFPrintBW(FILE *fp, dzSys *sys)
{
  fprintf( fp, "cf: %g\n", ((_dzBW*)sys->_prm)->cf );
  fprintf( fp, "dim: %d\n", ((_dzBW*)sys->_prm)->dim );
}

dzSysMethod dz_sys_bw_met = {
  type: "butterworth",
  destroy: dzSysDestroyBW,
  refresh: dzSysRefreshBW,
  update: dzSysUpdateBW,
  fscan: dzSysFScanBW,
  fprint: dzSysFPrintBW,
};

/* dzSysCreateBW
 * - create a Butterworth filter.
 */
bool dzSysCreateBW(dzSys *sys, double cf, uint dim)
{
  _dzBW *bw;

  if( dim == 0 ){
    ZRUNERROR( "zero-order filter required" );
    return false;
  }
  if( !( bw = zAlloc( _dzBW, 1 ) ) ){
    ZALLOCERROR();
    return false;
  }
  if( !_dzBWCreate( bw, cf, dim ) ){
    zFree( bw );
    return false;
  }
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_prm = bw;
  sys->_met = &dz_sys_bw_met;
  return true;
}
