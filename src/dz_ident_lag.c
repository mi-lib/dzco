/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_ident_lag - identify lag system
 */

#include <dzco/dz_ident_lag.h>

/* dzIdentTrig
 * - find trigger time stamp for system identification.
 */
int dzIdentTrig(double r[], int n)
{
  double r_old;
  register int i;

  r_old = r[0];
  for( i=1; i<n; i++ ){
    if( !zIsTiny( r[i] - r_old ) ) return i;
    r_old = r[i];
  }
  ZRUNERROR( "trigger not found" );
  return -1;
}

/* ********************************************************** */
/* first-order-lag system identification
 * ********************************************************** */

struct _dzIdentFOL_t{
  double *t; /* time stamps of sampled data */
  double *r; /* sampled reference data */
  double *y; /* sampled output data */
  int n;     /* number of samples */
  int trig;  /* trigger stamp */
};

static double _dzIdentFOLEval(zVec prm, void *priv);

/* dzIdentFOLInit
 * - initial estimation of first-order-lag system parameters.
 */
void dzIdentFOL1(double t[], double r[], double y[], int n, int trig, double *tc, double *gain)
{
  register int i;
  double k, r0, y0;

  if( trig > 0 ){
    r0 = r[trig-1];
    y0 = y[trig-1];
  } else
    r0 = y0 = 0;
  /* gain */
  k = 0.63 * ( *gain = ( y[n-1] - y0 ) / ( r[n-1] - r0 ) );
  /* time constant */
  for( i=trig; i<n; i++ )
    if( ( y[i] - y0 ) > k * ( r[n-1] - r0 ) ){
      *tc = t[i] - t[trig];
      return;
    }
  *tc = t[n-1] - t[trig]; /* out of range */
}

/* (static)
 * _dzIdentFOLEval
 * - evaluate residual of FOL system.
 */
double _dzIdentFOLEval(zVec prm, void *priv)
{
  struct _dzIdentFOL_t *ws = priv;
  register int i;
  double tc, k, r, eval;

  tc = zVecElem( prm, 0 ); /* time constant */
  k  = zVecElem( prm, 1 ); /* gain */
  for( eval=0, i=ws->trig; i<ws->n; i++ ){
    r = k * ws->r[i] * ( 1 - exp(-(ws->t[i]-ws->t[ws->trig])/tc) ) - ws->y[i];
    eval += r*r;
  }
  return eval;
}

/* dzIdentFOL
 * - identify first-order-lag system
 *   (time constant and gain).
 */
bool dzIdentFOL(double t[], double r[], double y[], int n, int trig, double *tc, double *gain)
{
  struct _dzIdentFOL_t ws;
  zVec prm;
  zOptNM opt;

  ws.t = t;
  ws.r = r;
  ws.y = y;
  ws.n = n;
  ws.trig = trig;
  dzIdentFOL1( t, r, y, n, trig, tc, gain );
  if( !( prm = zVecCreateList( 2, *tc, *gain ) ) ){
    ZALLOCERROR();
    return false;
  }

  zOptNMCreate( &opt, 2, _dzIdentFOLEval );
  zOptNMSolve( &opt, prm, &ws, zTOL, 0, NULL );
  *tc   = zVecElem( prm, 0 );
  *gain = zVecElem( prm, 1 );
  zVecFree( prm );
  zOptNMDestroy( &opt );
  return true;
}

/* ********************************************************** */
/* second-order-lag system identification
 * ********************************************************** */

struct _dzIdentSOL_t{
  double *t; /* time stamps of sampled data */
  double *r; /* sampled reference data */
  double *y; /* sampled output data */
  int n;     /* number of samples */
  int trig;  /* trigger stamp */
  int peak;  /* peak stamp */
};

static int _dzIdentSOLPeak(double y[], int n, int trig);
static double _dzIdentSOL_ref(double t, double z, double k);
static double _dzIdentSOLEval(zVec prm, void *priv);

/* (static)
 * _dzIdentSOLPeak
 * - find peak time stamp for system identification.
 */
int _dzIdentSOLPeak(double y[], int n, int trig)
{
  double y_max, val;
  register int i, peak;

  y_max = fabs( y[( peak = trig )] - y[trig-1] );
  for( i=trig+1; i<n; i++ )
    if( ( val = fabs( y[i] - y[trig-1] ) ) >= y_max ){
      peak = i;
      y_max = val;
    }
  return peak;
}

/* dzIdentSOL1
 * - initial estimation of second-order-lag system parameters.
 */
void dzIdentSOL1(double t[], double r[], double y[], int n, int trig, double *tc, double *z, double *gain)
{
  int peak;
  double p, r0, y0;

  if( trig > 0 ){
    r0 = r[trig-1];
    y0 = y[trig-1];
  } else
    r0 = y0 = 0;
  peak = _dzIdentSOLPeak( y, n, trig );
  /* gain */
  *gain = ( y[n-1] - y0 ) / ( r[n-1] - r0 );
  /* damping coefficient */
  p = ( y[peak] - y[n-1] ) / *gain;
  if( zIsTiny( p ) ){
    *z = 1.0; /* critical damping */
    *tc = 1.0; /* dummy time constant */
  } else{
    p = log( fabs(p) );
    *z = -p / sqrt( zSqr(p) + zSqr(zPI) );
    /* time constant */
    *tc = ( t[peak] - t[trig] ) * sqrt( 1 - zSqr(*z) )/zPI;
  }
}

/* (static)
 * _dzIdentSOL_ref
 * - ideal referential value of SOL function.
 */
double _dzIdentSOL_ref(double t, double z, double k)
{
  double d, l1, l2;

  if( z < 1.0 ){
    d = sqrt( 1 - z*z );
    return k * ( 1 - exp(-z*t)/d * cos( d*t - atan(z/d) ) );
  } else if( z > 1.0 ){
    d = sqrt( z*z - 1 );
    l1 = -z+d;
    l2 = -z-d;
    return k * ( 1 + 0.5/d * ( l2*exp(l1*t) - l1*exp(l2*t) ) );
  } else
    return k * ( 1 - exp(-t) - t*exp(-t) );
}

/* (static)
 * _dzIdentSOLEval
 * - evaluate residual of SOL system.
 */
double _dzIdentSOLEval(zVec prm, void *priv)
{
  struct _dzIdentSOL_t *ws = priv;
  register int i;
  double tc, z, gain, r, eval;

  tc = zVecElem( prm, 0 ); /* time constant */
  z  = zVecElem( prm, 1 ); /* damping coefficient */
  gain  = zVecElem( prm, 2 ); /* gain */
  for( eval=0, i=ws->trig; i<ws->n; i++ ){
    r = _dzIdentSOL_ref( (ws->t[i]-ws->t[ws->trig])/tc, z, gain ) * ws->r[i] - ws->y[i];
    eval += r*r;
  }
  return eval;
}

/* dzIdentSOL
 * - identify second-order-lag system
 *   (time constant, damping coefficient and gain).
 */
bool dzIdentSOL(double t[], double r[], double y[], int n, int trig, double *tc, double *z, double *gain)
{
  struct _dzIdentSOL_t ws;
  zVec prm;
  zOptNM opt;

  ws.t = t;
  ws.r = r;
  ws.y = y;
  ws.n = n;
  dzIdentSOL1( t, r, y, n, trig, tc, z, gain );
  if( !( prm = zVecCreateList( 3, *tc, *z, *gain ) ) ){
    ZALLOCERROR();
    return false;
  }
  ws.trig = trig;

  zOptNMCreate( &opt, 3, _dzIdentSOLEval );
  zOptNMSolve( &opt, prm, &ws, zTOL, 0, NULL );
  *tc   = zVecElem( prm, 0 );
  *z    = zVecElem( prm, 1 );
  *gain = zVecElem( prm, 2 );
  zVecFree( prm );
  zOptNMDestroy( &opt );
  return true;
}
