/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_lag - lag system
 */

#include <dzco/dz_sys.h>

/* ********************************************************** */
/* first-order-lag system
 * ********************************************************** */

#define __dz_sys_fol_tc(s)   ( ((double*)(s)->_prm)[0] )
#define __dz_sys_fol_gain(s) ( ((double*)(s)->_prm)[1] )

static bool _dzSysFReadFOL(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshFOL(dzSys *sys)
{
  dzSysOutputVal(sys,0) = 0;
}

zVec dzSysUpdateFOL(dzSys *sys, double dt)
{
  double tr;

  tr = dt / __dz_sys_fol_tc(sys);
  dzSysOutputVal(sys,0) = ( dzSysOutputVal(sys,0)
    + __dz_sys_fol_gain(sys) * dzSysInputVal(sys,0) * tr ) / ( 1 + tr );
  return dzSysOutput(sys);
}

bool _dzSysFReadFOL(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "tc" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "gain" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFReadFOL(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0 };

  zFieldFRead( fp, _dzSysFReadFOL, val );
  return dzSysCreateFOL( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFWriteFOL(FILE *fp, dzSys *sys)
{
  fprintf( fp, "tc: %g\n", __dz_sys_fol_tc(sys) );
  fprintf( fp, "gain: %g\n", __dz_sys_fol_gain(sys) );
}

dzSysMethod dz_sys_fol_met = {
  type: "fol",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshFOL,
  update: dzSysUpdateFOL,
  fread: dzSysFReadFOL,
  fwrite: dzSysFWriteFOL,
};

/* dzSysCreateFOL
 * - create first-order-lag system.
 */
bool dzSysCreateFOL(dzSys *sys, double tc, double gain)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 2 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_fol_met;
  __dz_sys_fol_tc(sys) = tc;
  __dz_sys_fol_gain(sys) = gain;
  dzSysRefresh( sys );
  return true;
}

void dzSysFOLSetTC(dzSys *sys, double tc)
{
  __dz_sys_fol_tc(sys) = tc;
}

void dzSysFOLSetGain(dzSys *sys, double gain)
{
  __dz_sys_fol_gain(sys) = gain;
}

/* ********************************************************** */
/* second-order-lag system
 * ********************************************************** */

#define __dz_sys_sol_t1(s)      ( ((double*)(s)->_prm)[0] )
#define __dz_sys_sol_t2(s)      ( ((double*)(s)->_prm)[1] )
#define __dz_sys_sol_damp(s)    ( ((double*)(s)->_prm)[2] )
#define __dz_sys_sol_gain(s)    ( ((double*)(s)->_prm)[3] )
#define __dz_sys_sol_prevout(s) ( ((double*)(s)->_prm)[4] )
#define __dz_sys_sol_previn(s)  ( ((double*)(s)->_prm)[5] )
#define __dz_sys_sol_tr(s)      ( ((double*)(s)->_prm)[6] )

static bool _dzSysFReadSOL(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshSOL(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_sol_prevout(sys) = __dz_sys_sol_previn(sys) = 0;
}

zVec dzSysUpdateSOL(dzSys *sys, double dt)
{
  double tr, trp, ret, dr;

  tr = __dz_sys_sol_t1(sys) / dt;
  trp = tr * __dz_sys_sol_tr(sys);
  dr = tr * ( tr + 2*__dz_sys_sol_damp(sys) );
  ret = ( dr + trp ) * dzSysOutputVal(sys,0) - trp * __dz_sys_sol_prevout(sys)
      + __dz_sys_sol_gain(sys) * ( dzSysInputVal(sys,0) +
          __dz_sys_sol_t2(sys)/dt*( dzSysInputVal(sys,0) - __dz_sys_sol_previn(sys) ) );
  __dz_sys_sol_prevout(sys) = dzSysOutputVal(sys,0);
  __dz_sys_sol_previn(sys)  = dzSysInputVal(sys,0);
  __dz_sys_sol_tr(sys) = tr;
  dzSysOutputVal(sys,0) = ret / ( dr + 1 );
  return dzSysOutput(sys);
}

bool _dzSysFReadSOL(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "t1" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "t2" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
  if( strcmp( buf, "damp" ) == 0 ){
    ((double *)val)[2] = zFDouble( fp );
  } else
  if( strcmp( buf, "gain" ) == 0 ){
    ((double *)val)[3] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFReadSOL(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0, 1.0, 0.0 };

  zFieldFRead( fp, _dzSysFReadSOL, val );
  return dzSysCreateSOL( sys, val[0], val[1], val[2], val[3] ) ? sys : NULL;
}

void dzSysFWriteSOL(FILE *fp, dzSys *sys)
{
  fprintf( fp, "t1: %g\n", __dz_sys_sol_t1(sys) );
  fprintf( fp, "t2: %g\n", __dz_sys_sol_t2(sys) );
  fprintf( fp, "damp: %g\n", __dz_sys_sol_damp(sys) );
  fprintf( fp, "gain: %g\n", __dz_sys_sol_gain(sys) );
}

dzSysMethod dz_sys_sol_met = {
  type: "sol",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshSOL,
  update: dzSysUpdateSOL,
  fread: dzSysFReadSOL,
  fwrite: dzSysFWriteSOL,
};

/* dzSysCreateSOL
 * - create second-order-lag system in standard form.
 */
bool dzSysCreateSOL(dzSys *sys, double t1, double t2, double damp, double gain)
{
  if( t1 <= zTOL ){
    ZRUNERROR( "too short time constant" );
    return false;
  }
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 7 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_sol_met;
  __dz_sys_sol_t1(sys) = t1;
  __dz_sys_sol_t2(sys) = t2;
  __dz_sys_sol_damp(sys) = damp;
  __dz_sys_sol_gain(sys) = gain;
  dzSysRefresh( sys );
  return true;
}

/* dzSysCreateSOLGen
 * - create second-order-lag system in general form.
 */
bool dzSysCreateSOLGen(dzSys *sys, double a, double b, double c, double d, double e)
{
  return dzSysCreateSOL( sys, sqrt(a/c), d/e, 0.5*b/sqrt(a*c), e/c );
}

/* ********************************************************** */
/* phase compensator system
 * ********************************************************** */

#define __dz_sys_pc_prev(s) ( ((double*)(s)->_prm)[0] )
#define __dz_sys_pc_t1(s)   ( ((double*)(s)->_prm)[1] )
#define __dz_sys_pc_t2(s)   ( ((double*)(s)->_prm)[2] )
#define __dz_sys_pc_gain(s) ( ((double*)(s)->_prm)[3] )

static bool _dzSysFReadPC(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshPC(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_pc_prev(sys) = 0;
}

zVec dzSysUpdatePC(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = ( __dz_sys_pc_t1(sys) * dzSysOutputVal(sys,0)
    + __dz_sys_pc_gain(sys) * ( dt * dzSysInputVal(sys,0)
    + __dz_sys_pc_t2(sys) * ( dzSysInputVal(sys,0) - __dz_sys_pc_prev(sys) ) ) ) / ( dt + __dz_sys_pc_t1(sys) );
  __dz_sys_pc_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

bool _dzSysFReadPC(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "t1" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "t2" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
  if( strcmp( buf, "gain" ) == 0 ){
    ((double *)val)[2] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFReadPC(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0, 0.0 };

  zFieldFRead( fp, _dzSysFReadPC, val );
  return dzSysCreatePC( sys, val[0], val[1], val[2] ) ? sys : NULL;
}

void dzSysFWritePC(FILE *fp, dzSys *sys)
{
  fprintf( fp, "t1: %g\n", __dz_sys_pc_t1(sys) );
  fprintf( fp, "t2: %g\n", __dz_sys_pc_t2(sys) );
  fprintf( fp, "gain: %g\n", __dz_sys_pc_gain(sys) );
}

dzSysMethod dz_sys_pc_met = {
  type: "pc",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshPC,
  update: dzSysUpdatePC,
  fread: dzSysFReadPC,
  fwrite: dzSysFWritePC,
};

/* dzSysCreatePC
 * - create phase compensator.
 */
bool dzSysCreatePC(dzSys *sys, double t1, double t2, double gain)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 4 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_pc_met;
  __dz_sys_pc_t1(sys) = t1;
  __dz_sys_pc_t2(sys) = t2;
  __dz_sys_pc_gain(sys) = gain;
  dzSysRefreshPC( sys );
  return true;
}

/* ********************************************************** */
/* adaptive system
 * ********************************************************** */

#define __dz_sys_adapt_tc(s)     ( ((double*)(s)->_prm)[0] )
#define __dz_sys_adapt_base(s)   ( ((double*)(s)->_prm)[1] )
#define __dz_sys_adapt_offset(s) ( ((double*)(s)->_prm)[2] )

static bool _dzSysFReadAdapt(FILE *fp, void *val, char *buf, bool *success);

void dzSysAdaptSetBase(dzSys *sys, double base)
{
  __dz_sys_adapt_base(sys) = base;
}

void dzSysRefreshAdapt(dzSys *sys)
{
  __dz_sys_adapt_offset(sys) = 0;
  dzSysOutputVal(sys,0) = __dz_sys_adapt_base(sys);
}

zVec dzSysUpdateAdapt(dzSys *sys, double dt)
{
  if( !zIsNan( dzSysInputVal(sys,0) ) ){
    __dz_sys_adapt_offset(sys) = dzSysInputVal(sys,0) - __dz_sys_adapt_base(sys);
    dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  } else{
    __dz_sys_adapt_offset(sys) -= __dz_sys_adapt_offset(sys) / __dz_sys_adapt_tc(sys) * dt;
    dzSysOutputVal(sys,0) = __dz_sys_adapt_offset(sys) + __dz_sys_adapt_base(sys);
  }
  return dzSysOutput(sys);
}

bool _dzSysFReadAdapt(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "tc" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "base" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFReadAdapt(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0 };

  zFieldFRead( fp, _dzSysFReadAdapt, val );
  return dzSysCreateAdapt( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFWriteAdapt(FILE *fp, dzSys *sys)
{
  fprintf( fp, "tc: %g\n", __dz_sys_adapt_tc(sys) );
  fprintf( fp, "base: %g\n", __dz_sys_adapt_base(sys) );
}

dzSysMethod dz_sys_adapt_met = {
  type: "adapt",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshAdapt,
  update: dzSysUpdateAdapt,
  fread: dzSysFReadAdapt,
  fwrite: dzSysFWriteAdapt,
};

/* dzSysCreateAdapt
 * - create a adaptive system.
 */
bool dzSysCreateAdapt(dzSys *sys, double tc, double base)
{
  if( tc <= zTOL ){
    ZRUNERROR( "too short time constant" );
    return false;
  }
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 3 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_adapt_met;
  __dz_sys_adapt_tc(sys) = tc;
  __dz_sys_adapt_base(sys) = base;
  dzSysRefreshAdapt( sys );
  return true;
}
