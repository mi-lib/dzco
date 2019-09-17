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
  return dzSysCreateFOL( sys, val[0], val[1] );
}

static void *_dzSysFromZTKFOLTc(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKFOLGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintFOLTc(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fol_tc((dzSys*)prp) );
}
static void _dzSysFPrintFOLGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fol_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_fol[] = {
  { "tc", 1, _dzSysFromZTKFOLTc, _dzSysFPrintFOLTc },
  { "gain", 1, _dzSysFromZTKFOLGain, _dzSysFPrintFOLGain },
};

static bool _dzSysRegZTKFOL(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_fol ) ? true : false;
}

static dzSys *_dzSysFromZTKFOL(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_fol ) ) return NULL;
  return dzSysCreateFOL( sys, val[0], val[1] );
}

static void _dzSysFPrintFOL(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_fol );
}

dzSysCom dz_sys_fol_com = {
  typestr: "FOL",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshFOL,
  update: dzSysUpdateFOL,
  fscan: dzSysFScanFOL,
  regZTK: _dzSysRegZTKFOL,
  fromZTK: _dzSysFromZTKFOL,
  fprint: _dzSysFPrintFOL,
};

/* create a first-order-lag system. */
dzSys *dzSysCreateFOL(dzSys *sys, double tc, double gain)
{
  dzSysInit( sys );
  sys->com = &dz_sys_fol_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 2 ) ) ) return NULL;
  __dz_sys_fol_tc(sys) = tc;
  __dz_sys_fol_gain(sys) = gain;
  dzSysRefresh( sys );
  return sys;
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
  return dzSysCreateSOL( sys, val[0], val[1], val[2], val[3] );
}

static void *_dzSysFromZTKSOLT1(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKSOLT2(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKSOLDamp(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKSOLGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[3] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintSOLT1(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_t1((dzSys*)prp) );
}
static void _dzSysFPrintSOLT2(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_t2((dzSys*)prp) );
}
static void _dzSysFPrintSOLDamp(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_damp((dzSys*)prp) );
}
static void _dzSysFPrintSOLGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_sol[] = {
  { "t1", 1, _dzSysFromZTKSOLT1, _dzSysFPrintSOLT1 },
  { "t2", 1, _dzSysFromZTKSOLT2, _dzSysFPrintSOLT2 },
  { "damp", 1, _dzSysFromZTKSOLDamp, _dzSysFPrintSOLDamp },
  { "gain", 1, _dzSysFromZTKSOLGain, _dzSysFPrintSOLGain },
};

static bool _dzSysRegZTKSOL(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_sol ) ? true : false;
}

static dzSys *_dzSysFromZTKSOL(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0, 1.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_sol ) ) return NULL;
  return dzSysCreateSOL( sys, val[0], val[1], val[2], val[3] );
}

static void _dzSysFPrintSOL(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_sol );
}

dzSysCom dz_sys_sol_com = {
  typestr: "SOL",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshSOL,
  update: dzSysUpdateSOL,
  fscan: dzSysFScanSOL,
  regZTK: _dzSysRegZTKSOL,
  fromZTK: _dzSysFromZTKSOL,
  fprint: _dzSysFPrintSOL,
};

/* create a second-order-lag system in standard form. */
dzSys *dzSysCreateSOL(dzSys *sys, double t1, double t2, double damp, double gain)
{
  if( t1 <= zTOL ){
    ZRUNERROR( DZ_ERR_SYS_LAG_TOOSHORTTC );
    return NULL;
  }
  dzSysInit( sys );
  sys->com = &dz_sys_sol_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 7 ) ) ) return NULL;
  __dz_sys_sol_t1(sys) = t1;
  __dz_sys_sol_t2(sys) = t2;
  __dz_sys_sol_damp(sys) = damp;
  __dz_sys_sol_gain(sys) = gain;
  dzSysRefresh( sys );
  return sys;
}

/* create a second-order-lag system in general form. */
dzSys *dzSysCreateSOLGen(dzSys *sys, double a, double b, double c, double d, double e)
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
  return dzSysCreatePC( sys, val[0], val[1], val[2] );
}

static void *_dzSysFromZTKPCT1(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKPCT2(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKPCGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintPCT1(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pc_t1((dzSys*)prp) );
}
static void _dzSysFPrintPCT2(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pc_t2((dzSys*)prp) );
}
static void _dzSysFPrintPCGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pc_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_pc[] = {
  { "t1", 1, _dzSysFromZTKPCT1, _dzSysFPrintPCT1 },
  { "t2", 1, _dzSysFromZTKPCT2, _dzSysFPrintPCT2 },
  { "gain", 1, _dzSysFromZTKPCGain, _dzSysFPrintPCGain },
};

static bool _dzSysRegZTKPC(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_pc ) ? true : false;
}

static dzSys *_dzSysFromZTKPC(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_pc ) ) return NULL;
  return dzSysCreatePC( sys, val[0], val[1], val[2] );
}

static void _dzSysFPrintPC(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_pc );
}

dzSysCom dz_sys_pc_com = {
  typestr: "phasecomp",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshPC,
  update: dzSysUpdatePC,
  fscan: dzSysFScanPC,
  regZTK: _dzSysRegZTKPC,
  fromZTK: _dzSysFromZTKPC,
  fprint: _dzSysFPrintPC,
};

/* create a phase compensator. */
dzSys *dzSysCreatePC(dzSys *sys, double t1, double t2, double gain)
{
  dzSysInit( sys );
  sys->com = &dz_sys_pc_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 4 ) ) ) return NULL;
  __dz_sys_pc_t1(sys) = t1;
  __dz_sys_pc_t2(sys) = t2;
  __dz_sys_pc_gain(sys) = gain;
  dzSysRefreshPC( sys );
  return sys;
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
  return dzSysCreateAdapt( sys, val[0], val[1] );
}

static void *_dzSysFromZTKAdaptTc(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKAdaptBase(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintAdaptTc(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_adapt_tc((dzSys*)prp) );
}
static void _dzSysFPrintAdaptBase(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_adapt_base((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_adapt[] = {
  { "tc", 1, _dzSysFromZTKAdaptTc, _dzSysFPrintAdaptTc },
  { "base", 1, _dzSysFromZTKAdaptBase, _dzSysFPrintAdaptBase },
};

static bool _dzSysRegZTKAdapt(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_adapt ) ? true : false;
}

static dzSys *_dzSysFromZTKAdapt(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_adapt ) ) return NULL;
  return dzSysCreateAdapt( sys, val[0], val[1] );
}

static void _dzSysFPrintAdapt(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_adapt );
}

dzSysCom dz_sys_adapt_com = {
  typestr: "adapt",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshAdapt,
  update: dzSysUpdateAdapt,
  fscan: dzSysFScanAdapt,
  regZTK: _dzSysRegZTKAdapt,
  fromZTK: _dzSysFromZTKAdapt,
  fprint: _dzSysFPrintAdapt,
};

/* create an adaptive system. */
dzSys *dzSysCreateAdapt(dzSys *sys, double tc, double base)
{
  if( tc <= zTOL ){
    ZRUNERROR( DZ_ERR_SYS_LAG_TOOSHORTTC );
    return NULL;
  }
  dzSysInit( sys );
  sys->com = &dz_sys_adapt_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 3 ) ) ) return NULL;
  __dz_sys_adapt_tc(sys) = tc;
  __dz_sys_adapt_base(sys) = base;
  dzSysRefreshAdapt( sys );
  return sys;
}
