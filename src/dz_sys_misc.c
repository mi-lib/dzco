/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_misc - miscellany systems
 */

#include <dzco/dz_sys.h>

static void *_dzSysMIInFromZTK(void *val, int i, void *arg, ZTK *ztk){
  *((int*)val) = ZTKInt(ztk);
  return val;
}

static bool _dzSysMIInFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%d\n", dzSysInputNum((dzSys*)prp) );
  return true;
}

static ZTKPrp __ztk_prp_dzsys_mi[] = {
  { "in", 1, _dzSysMIInFromZTK, _dzSysMIInFPrintZTK },
};

static void _dzSysMIFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_mi );
}

/* ********************************************************** */
/* adder
 * ********************************************************** */

static zVec _dzSysAdderUpdate(dzSys *sys, double dt)
{
  int i;

  dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  for( i=1; i<dzSysInputNum(sys); i++ )
    dzSysOutputVal(sys,0) += dzSysInputVal(sys,i);
  return dzSysOutput(sys);
}

static dzSys *_dzSysAdderFromZTK(dzSys *sys, ZTK *ztk)
{
  int n;
  if( !ZTKEvalKey( &n, NULL, ztk, __ztk_prp_dzsys_mi ) ) return NULL;
  return dzSysAdderCreate( sys, n );
}

dzSysCom dz_sys_adder_com = {
  .typestr = "adder",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = dzSysDefaultRefresh,
  ._update = _dzSysAdderUpdate,
  ._fromZTK = _dzSysAdderFromZTK,
  ._fprintZTK = _dzSysMIFPrintZTK,
};

/* create an adder. */
dzSys *dzSysAdderCreate(dzSys *sys, int n)
{
  dzSysInit( sys );
  sys->com = &dz_sys_adder_com;
  dzSysAllocInput( sys, n );
  return dzSysInputNum(sys) == n && dzSysAllocOutput( sys, 1 ) ? sys : NULL;
}

/* ********************************************************** */
/* subtractor
 * ********************************************************** */

static zVec _dzSysSubtrUpdate(dzSys *sys, double dt)
{
  int i;

  dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  for( i=1; i<dzSysInputNum(sys); i++ )
    dzSysOutputVal(sys,0) -= dzSysInputVal(sys,i);
  return dzSysOutput(sys);
}

static dzSys *_dzSysSubtrFromZTK(dzSys *sys, ZTK *ztk)
{
  int n;
  if( !ZTKEvalKey( &n, NULL, ztk, __ztk_prp_dzsys_mi ) ) return NULL;
  return dzSysSubtrCreate( sys, n );
}

dzSysCom dz_sys_subtr_com = {
  .typestr = "subtr",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = dzSysDefaultRefresh,
  ._update = _dzSysSubtrUpdate,
  ._fromZTK = _dzSysSubtrFromZTK,
  ._fprintZTK = _dzSysMIFPrintZTK,
};

/* create a subtractor. */
dzSys *dzSysSubtrCreate(dzSys *sys, int n)
{
  dzSysInit( sys );
  sys->com = &dz_sys_subtr_com;
  dzSysAllocInput( sys, n );
  return dzSysInputNum(sys) == n && dzSysAllocOutput( sys, 1 ) ? sys : NULL;
}

/* ********************************************************** */
/* saturater
 * ********************************************************** */

#define __dz_sys_limit_min(s) ( ((double *)(s)->prp)[0] )
#define __dz_sys_limit_max(s) ( ((double *)(s)->prp)[1] )

static zVec _dzSysLimitUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) =
    zLimit( dzSysInputVal(sys,0), __dz_sys_limit_min(sys), __dz_sys_limit_max(sys) );
  return dzSysOutput(sys);
}

static void *_dzSysLimitMinFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysLimitMaxFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static bool _dzSysLimitMinFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_limit_min((dzSys*)prp) );
  return true;
}
static bool _dzSysLimitMaxFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_limit_max((dzSys*)prp) );
  return true;
}

static ZTKPrp __ztk_prp_dzsys_limit[] = {
  { "min", 1, _dzSysLimitMinFromZTK, _dzSysLimitMinFPrintZTK },
  { "max", 1, _dzSysLimitMaxFromZTK, _dzSysLimitMaxFPrintZTK },
};

static dzSys *_dzSysLimitFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[2];
  if( !ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_limit ) ) return NULL;
  return dzSysLimitCreate( sys, val[0], val[1] );
}

static void _dzSysLimitFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_limit );
}

dzSysCom dz_sys_limit_com = {
  .typestr = "limiter",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = dzSysDefaultRefresh,
  ._update = _dzSysLimitUpdate,
  ._fromZTK = _dzSysLimitFromZTK,
  ._fprintZTK = _dzSysLimitFPrintZTK,
};

/* create a saturater. */
dzSys *dzSysLimitCreate(dzSys *sys, double min, double max)
{
  dzSysInit( sys );
  sys->com = &dz_sys_limit_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 2 ) ) ) return NULL;
  __dz_sys_limit_min(sys) = zMin( max, min );
  __dz_sys_limit_max(sys) = zMax( max, min );
  return sys;
}
