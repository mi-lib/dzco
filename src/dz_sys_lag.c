/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_lag - lag system
 */

#include <dzco/dz_sys.h>

/* ********************************************************** */
/* first-order-lag system
 * ********************************************************** */

#define __dz_sys_fol_tc(s)   ( ((double*)(s)->prp)[0] )
#define __dz_sys_fol_gain(s) ( ((double*)(s)->prp)[1] )

static bool _dzSysFScanFOL(FILE *fp, void *val, char *buf, bool *success);

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

bool _dzSysFScanFOL(FILE *fp, void *val, char *buf, bool *success)
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

dzSys *dzSysFScanFOL(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanFOL, val );
  return dzSysCreateFOL( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFPrintFOL(FILE *fp, dzSys *sys)
{
  fprintf( fp, "tc: %g\n", __dz_sys_fol_tc(sys) );
  fprintf( fp, "gain: %g\n", __dz_sys_fol_gain(sys) );
}

dzSysCom dz_sys_fol_com = {
  typestr: "fol",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshFOL,
  update: dzSysUpdateFOL,
  fscan: dzSysFScanFOL,
  fprint: dzSysFPrintFOL,
};

/* create a first-order-lag system. */
bool dzSysCreateFOL(dzSys *sys, double tc, double gain)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 2 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->com = &dz_sys_fol_com;
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

#define __dz_sys_sol_t1(s)      ( ((double*)(s)->prp)[0] )
#define __dz_sys_sol_t2(s)      ( ((double*)(s)->prp)[1] )
#define __dz_sys_sol_damp(s)    ( ((double*)(s)->prp)[2] )
#define __dz_sys_sol_gain(s)    ( ((double*)(s)->prp)[3] )
#define __dz_sys_sol_prevout(s) ( ((double*)(s)->prp)[4] )
#define __dz_sys_sol_previn(s)  ( ((double*)(s)->prp)[5] )
#define __dz_sys_sol_tr(s)      ( ((double*)(s)->prp)[6] )

static bool _dzSysFScanSOL(FILE *fp, void *val, char *buf, bool *success);

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

bool _dzSysFScanSOL(FILE *fp, void *val, char *buf, bool *success)
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

dzSys *dzSysFScanSOL(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0, 1.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanSOL, val );
  return dzSysCreateSOL( sys, val[0], val[1], val[2], val[3] ) ? sys : NULL;
}

void dzSysFPrintSOL(FILE *fp, dzSys *sys)
{
  fprintf( fp, "t1: %g\n", __dz_sys_sol_t1(sys) );
  fprintf( fp, "t2: %g\n", __dz_sys_sol_t2(sys) );
  fprintf( fp, "damp: %g\n", __dz_sys_sol_damp(sys) );
  fprintf( fp, "gain: %g\n", __dz_sys_sol_gain(sys) );
}

dzSysCom dz_sys_sol_com = {
  typestr: "sol",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshSOL,
  update: dzSysUpdateSOL,
  fscan: dzSysFScanSOL,
  fprint: dzSysFPrintSOL,
};

/* create a second-order-lag system in standard form. */
bool dzSysCreateSOL(dzSys *sys, double t1, double t2, double damp, double gain)
{
  if( t1 <= zTOL ){
    ZRUNERROR( "too short time constant" );
    return false;
  }
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 7 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->com = &dz_sys_sol_com;
  __dz_sys_sol_t1(sys) = t1;
  __dz_sys_sol_t2(sys) = t2;
  __dz_sys_sol_damp(sys) = damp;
  __dz_sys_sol_gain(sys) = gain;
  dzSysRefresh( sys );
  return true;
}

/* create a second-order-lag system in general form. */
bool dzSysCreateSOLGen(dzSys *sys, double a, double b, double c, double d, double e)
{
  return dzSysCreateSOL( sys, sqrt(a/c), d/e, 0.5*b/sqrt(a*c), e/c );
}

/* ********************************************************** */
/* phase compensator system
 * ********************************************************** */

#define __dz_sys_pc_prev(s) ( ((double*)(s)->prp)[0] )
#define __dz_sys_pc_t1(s)   ( ((double*)(s)->prp)[1] )
#define __dz_sys_pc_t2(s)   ( ((double*)(s)->prp)[2] )
#define __dz_sys_pc_gain(s) ( ((double*)(s)->prp)[3] )

static bool _dzSysFScanPC(FILE *fp, void *val, char *buf, bool *success);

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

bool _dzSysFScanPC(FILE *fp, void *val, char *buf, bool *success)
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

dzSys *dzSysFScanPC(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanPC, val );
  return dzSysCreatePC( sys, val[0], val[1], val[2] ) ? sys : NULL;
}

void dzSysFPrintPC(FILE *fp, dzSys *sys)
{
  fprintf( fp, "t1: %g\n", __dz_sys_pc_t1(sys) );
  fprintf( fp, "t2: %g\n", __dz_sys_pc_t2(sys) );
  fprintf( fp, "gain: %g\n", __dz_sys_pc_gain(sys) );
}

dzSysCom dz_sys_pc_com = {
  typestr: "pc",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshPC,
  update: dzSysUpdatePC,
  fscan: dzSysFScanPC,
  fprint: dzSysFPrintPC,
};

/* create a phase compensator. */
bool dzSysCreatePC(dzSys *sys, double t1, double t2, double gain)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 4 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->com = &dz_sys_pc_com;
  __dz_sys_pc_t1(sys) = t1;
  __dz_sys_pc_t2(sys) = t2;
  __dz_sys_pc_gain(sys) = gain;
  dzSysRefreshPC( sys );
  return true;
}

/* ********************************************************** */
/* adaptive system
 * ********************************************************** */

#define __dz_sys_adapt_tc(s)     ( ((double*)(s)->prp)[0] )
#define __dz_sys_adapt_base(s)   ( ((double*)(s)->prp)[1] )
#define __dz_sys_adapt_offset(s) ( ((double*)(s)->prp)[2] )

static bool _dzSysFScanAdapt(FILE *fp, void *val, char *buf, bool *success);

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

bool _dzSysFScanAdapt(FILE *fp, void *val, char *buf, bool *success)
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

dzSys *dzSysFScanAdapt(FILE *fp, dzSys *sys)
{
  double val[] = { 1.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanAdapt, val );
  return dzSysCreateAdapt( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFPrintAdapt(FILE *fp, dzSys *sys)
{
  fprintf( fp, "tc: %g\n", __dz_sys_adapt_tc(sys) );
  fprintf( fp, "base: %g\n", __dz_sys_adapt_base(sys) );
}

dzSysCom dz_sys_adapt_com = {
  typestr: "adapt",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshAdapt,
  update: dzSysUpdateAdapt,
  fscan: dzSysFScanAdapt,
  fprint: dzSysFPrintAdapt,
};

/* create an adaptive system. */
bool dzSysCreateAdapt(dzSys *sys, double tc, double base)
{
  if( tc <= zTOL ){
    ZRUNERROR( "too short time constant" );
    return false;
  }
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 3 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->com = &dz_sys_adapt_com;
  __dz_sys_adapt_tc(sys) = tc;
  __dz_sys_adapt_base(sys) = base;
  dzSysRefreshAdapt( sys );
  return true;
}
