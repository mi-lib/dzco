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

static void _dzSysFOLRefresh(dzSys *sys)
{
  dzSysOutputVal(sys,0) = 0;
}

static zVec _dzSysFOLUpdate(dzSys *sys, double dt)
{
  double tr;

  tr = dt / __dz_sys_fol_tc(sys);
  dzSysOutputVal(sys,0) = ( dzSysOutputVal(sys,0)
    + __dz_sys_fol_gain(sys) * dzSysInputVal(sys,0) * tr ) / ( 1 + tr );
  return dzSysOutput(sys);
}

static void *_dzSysFOLTcFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFOLGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFOLTcFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fol_tc((dzSys*)prp) );
}
static void _dzSysFOLGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fol_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_fol[] = {
  { "tc", 1, _dzSysFOLTcFromZTK, _dzSysFOLTcFPrintZTK },
  { "gain", 1, _dzSysFOLGainFromZTK, _dzSysFOLGainFPrintZTK },
};

static dzSys *_dzSysFOLFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0 };
  if( !ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_fol ) ) return NULL;
  return dzSysFOLCreate( sys, val[0], val[1] );
}

static void _dzSysFOLFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_fol );
}

dzSysCom dz_sys_fol_com = {
  .typestr = "FOL",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysFOLRefresh,
  ._update = _dzSysFOLUpdate,
  ._fromZTK = _dzSysFOLFromZTK,
  ._fprintZTK = _dzSysFOLFPrintZTK,
};

/* create a first-order-lag system. */
dzSys *dzSysFOLCreate(dzSys *sys, double tc, double gain)
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

static void _dzSysSOLRefresh(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_sol_prevout(sys) = __dz_sys_sol_previn(sys) = 0;
}

static zVec _dzSysSOLUpdate(dzSys *sys, double dt)
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

static void *_dzSysSOLT1FromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysSOLT2FromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysSOLDampFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysSOLGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[3] = ZTKDouble(ztk);
  return val;
}

static void _dzSysSOLT1FPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_t1((dzSys*)prp) );
}
static void _dzSysSOLT2FPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_t2((dzSys*)prp) );
}
static void _dzSysSOLDampFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_damp((dzSys*)prp) );
}
static void _dzSysSOLGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_sol_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_sol[] = {
  { "t1", 1, _dzSysSOLT1FromZTK, _dzSysSOLT1FPrintZTK },
  { "t2", 1, _dzSysSOLT2FromZTK, _dzSysSOLT2FPrintZTK },
  { "damp", 1, _dzSysSOLDampFromZTK, _dzSysSOLDampFPrintZTK },
  { "gain", 1, _dzSysSOLGainFromZTK, _dzSysSOLGainFPrintZTK },
};

static dzSys *_dzSysSOLFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0, 1.0, 0.0 };
  if( !ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_sol ) ) return NULL;
  return dzSysSOLCreate( sys, val[0], val[1], val[2], val[3] );
}

static void _dzSysSOLFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_sol );
}

dzSysCom dz_sys_sol_com = {
  .typestr = "SOL",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysSOLRefresh,
  ._update = _dzSysSOLUpdate,
  ._fromZTK = _dzSysSOLFromZTK,
  ._fprintZTK = _dzSysSOLFPrintZTK,
};

/* create a second-order-lag system in standard form. */
dzSys *dzSysSOLCreate(dzSys *sys, double t1, double t2, double damp, double gain)
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

/* create a second-order-lag system in generic form. */
dzSys *dzSysSOLCreateGeneric(dzSys *sys, double a, double b, double c, double d, double e)
{
  return dzSysSOLCreate( sys, sqrt(a/c), d/e, 0.5*b/sqrt(a*c), e/c );
}

/* ********************************************************** */
/* phase compensator system
 * ********************************************************** */

#define __dz_sys_pc_prev(s) ( ((double*)(s)->prp)[0] )
#define __dz_sys_pc_t1(s)   ( ((double*)(s)->prp)[1] )
#define __dz_sys_pc_t2(s)   ( ((double*)(s)->prp)[2] )
#define __dz_sys_pc_gain(s) ( ((double*)(s)->prp)[3] )

static void _dzSysPCRefresh(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_pc_prev(sys) = 0;
}

static zVec _dzSysPCUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = ( __dz_sys_pc_t1(sys) * dzSysOutputVal(sys,0)
    + __dz_sys_pc_gain(sys) * ( dt * dzSysInputVal(sys,0)
    + __dz_sys_pc_t2(sys) * ( dzSysInputVal(sys,0) - __dz_sys_pc_prev(sys) ) ) ) / ( dt + __dz_sys_pc_t1(sys) );
  __dz_sys_pc_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

static void *_dzSysPCT1FromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysPCT2FromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysPCGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}

static void _dzSysPCT1FPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pc_t1((dzSys*)prp) );
}
static void _dzSysPCT2FPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pc_t2((dzSys*)prp) );
}
static void _dzSysPCGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pc_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_pc[] = {
  { "t1", 1, _dzSysPCT1FromZTK, _dzSysPCT1FPrintZTK },
  { "t2", 1, _dzSysPCT2FromZTK, _dzSysPCT2FPrintZTK },
  { "gain", 1, _dzSysPCGainFromZTK, _dzSysPCGainFPrintZTK },
};

static dzSys *_dzSysPCFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0, 0.0 };
  if( !ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_pc ) ) return NULL;
  return dzSysPCCreate( sys, val[0], val[1], val[2] );
}

static void _dzSysPCFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_pc );
}

dzSysCom dz_sys_pc_com = {
  .typestr = "phasecomp",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysPCRefresh,
  ._update = _dzSysPCUpdate,
  ._fromZTK = _dzSysPCFromZTK,
  ._fprintZTK = _dzSysPCFPrintZTK,
};

/* create a phase compensator. */
dzSys *dzSysPCCreate(dzSys *sys, double t1, double t2, double gain)
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
  dzSysRefresh( sys );
  return sys;
}

/* ********************************************************** */
/* adaptive system
 * ********************************************************** */

#define __dz_sys_adapt_tc(s)     ( ((double*)(s)->prp)[0] )
#define __dz_sys_adapt_base(s)   ( ((double*)(s)->prp)[1] )
#define __dz_sys_adapt_offset(s) ( ((double*)(s)->prp)[2] )

void dzSysAdaptSetBase(dzSys *sys, double base)
{
  __dz_sys_adapt_base(sys) = base;
}

static void _dzSysAdaptRefresh(dzSys *sys)
{
  __dz_sys_adapt_offset(sys) = 0;
  dzSysOutputVal(sys,0) = __dz_sys_adapt_base(sys);
}

static zVec _dzSysAdaptUpdate(dzSys *sys, double dt)
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

static void *_dzSysAdaptTcFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysAdaptBaseFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysAdaptTcFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_adapt_tc((dzSys*)prp) );
}
static void _dzSysAdaptBaseFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_adapt_base((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_adapt[] = {
  { "tc", 1, _dzSysAdaptTcFromZTK, _dzSysAdaptTcFPrintZTK },
  { "base", 1, _dzSysAdaptBaseFromZTK, _dzSysAdaptBaseFPrintZTK },
};

static dzSys *_dzSysAdaptFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 1.0, 0.0 };
  if( !ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_adapt ) ) return NULL;
  return dzSysAdaptCreate( sys, val[0], val[1] );
}

static void _dzSysAdaptFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_adapt );
}

dzSysCom dz_sys_adapt_com = {
  .typestr = "adapt",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysAdaptRefresh,
  ._update = _dzSysAdaptUpdate,
  ._fromZTK = _dzSysAdaptFromZTK,
  ._fprintZTK = _dzSysAdaptFPrintZTK,
};

/* create an adaptive system. */
dzSys *dzSysAdaptCreate(dzSys *sys, double tc, double base)
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
  dzSysRefresh( sys );
  return sys;
}
