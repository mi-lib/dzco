/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_misc - miscellany systems
 */

#include <dzco/dz_sys.h>

static bool _dzSysFScanMI(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "in" ) == 0 ){
    *(int *)val = zFInt( fp );
    return true;
  }
  return false;
}

static void *_dzSysFromZTKMIIn(void *val, int i, void *arg, ZTK *ztk){
  *((int*)val) = ZTKInt(ztk);
  return val;
}

static void _dzSysFPrintMIIn(FILE *fp, int i, void *prp){
  fprintf( fp, "%d\n", dzSysInputNum((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_mi[] = {
  { "in", 1, _dzSysFromZTKMIIn, _dzSysFPrintMIIn },
};

static bool _dzSysRegZTKMI(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_mi ) ? true : false;
}

static void _dzSysFPrintMI(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_mi );
}

/* ********************************************************** */
/* adder
 * ********************************************************** */

zVec dzSysUpdateAdder(dzSys *sys, double dt)
{
  register int i;

  dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  for( i=1; i<dzSysInputNum(sys); i++ )
    dzSysOutputVal(sys,0) += dzSysInputVal(sys,i);
  return dzSysOutput(sys);
}

dzSys *dzSysFScanAdder(FILE *fp, dzSys *sys)
{
  int n = 2;

  zFieldFScan( fp, _dzSysFScanMI, &n );
  return dzSysCreateAdder( sys, n );
}

static dzSys *_dzSysFromZTKAdder(dzSys *sys, ZTK *ztk)
{
  int n;
  if( !ZTKEncodeKey( &n, NULL, ztk, __ztk_prp_dzsys_mi ) ) return NULL;
  return dzSysCreateAdder( sys, n );
}

dzSysCom dz_sys_adder_com = {
  typestr: "adder",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateAdder,
  fscan: dzSysFScanAdder,
  regZTK: _dzSysRegZTKMI,
  fromZTK: _dzSysFromZTKAdder,
  fprint: _dzSysFPrintMI,
};

/* create an adder. */
dzSys *dzSysCreateAdder(dzSys *sys, int n)
{
  dzSysInit( sys );
  sys->com = &dz_sys_adder_com;
  dzSysAllocInput( sys, n );
  return dzSysInputNum(sys) == n &&
         dzSysAllocOutput( sys, 1 ) ? sys : NULL;
}

/* ********************************************************** */
/* subtractor
 * ********************************************************** */

zVec dzSysUpdateSubtr(dzSys *sys, double dt)
{
  register int i;

  dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  for( i=1; i<dzSysInputNum(sys); i++ )
    dzSysOutputVal(sys,0) -= dzSysInputVal(sys,i);
  return dzSysOutput(sys);
}

dzSys *dzSysFScanSubtr(FILE *fp, dzSys *sys)
{
  int n = 2;

  zFieldFScan( fp, _dzSysFScanMI, &n );
  return dzSysCreateSubtr( sys, n );
}

static dzSys *_dzSysFromZTKSubtr(dzSys *sys, ZTK *ztk)
{
  int n;
  if( !ZTKEncodeKey( &n, NULL, ztk, __ztk_prp_dzsys_mi ) ) return NULL;
  return dzSysCreateSubtr( sys, n );
}

dzSysCom dz_sys_subtr_com = {
  typestr: "subtr",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateSubtr,
  fscan: dzSysFScanSubtr,
  regZTK: _dzSysRegZTKMI,
  fromZTK: _dzSysFromZTKSubtr,
  fprint: _dzSysFPrintMI,
};

/* create a subtractor. */
dzSys *dzSysCreateSubtr(dzSys *sys, int n)
{
  dzSysInit( sys );
  sys->com = &dz_sys_subtr_com;
  dzSysAllocInput( sys, n );
  return dzSysInputNum(sys) == n &&
         dzSysAllocOutput( sys, 1 ) ? sys : NULL;
}

/* ********************************************************** */
/* saturater
 * ********************************************************** */

#define __dz_sys_limit_min(s) ( ((double *)(s)->prp)[0] )
#define __dz_sys_limit_max(s) ( ((double *)(s)->prp)[1] )

static bool _dzSysFScanLimit(FILE *fp, void *val, char *buf, bool *success);

zVec dzSysUpdateLimit(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) =
    zLimit( dzSysInputVal(sys,0), __dz_sys_limit_min(sys), __dz_sys_limit_max(sys) );
  return dzSysOutput(sys);
}

bool _dzSysFScanLimit(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "min" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "max" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanLimit(FILE *fp, dzSys *sys)
{
  double val[] = { -HUGE_VAL, HUGE_VAL };

  zFieldFScan( fp, _dzSysFScanLimit, val );
  return dzSysCreateLimit( sys, val[0], val[1] );
}

static void *_dzSysFromZTKLimitMin(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKLimitMax(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintLimitMin(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_limit_min((dzSys*)prp) );
}
static void _dzSysFPrintLimitMax(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_limit_max((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_limit[] = {
  { "min", 1, _dzSysFromZTKLimitMin, _dzSysFPrintLimitMin },
  { "max", 1, _dzSysFromZTKLimitMax, _dzSysFPrintLimitMax },
};

static bool _dzSysRegZTKLimit(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_limit ) ? true : false;
}

static dzSys *_dzSysFromZTKLimit(dzSys *sys, ZTK *ztk)
{
  double val[2];
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_limit ) ) return NULL;
  return dzSysCreateLimit( sys, val[0], val[1] );
}

static void _dzSysFPrintLimit(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_limit );
}

dzSysCom dz_sys_limit_com = {
  typestr: "limiter",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateLimit,
  fscan: dzSysFScanLimit,
  regZTK: _dzSysRegZTKLimit,
  fromZTK: _dzSysFromZTKLimit,
  fprint: _dzSysFPrintLimit,
};

/* create a saturater. */
dzSys *dzSysCreateLimit(dzSys *sys, double min, double max)
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
