/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_filt_maf - moving-average filter
 */

#include <dzco/dz_sys.h>

#define __dz_sys_maf_ff(s)  ( ((double*)(s)->prp)[0] )
#define __dz_sys_maf_iov(s) ( ((double*)(s)->prp)[1] )

/* cut-off frequency to forgetting-factor */
static double _dzSysMAFCF2FF(double cf, double dt){
  return 1.0 / ( 1 + 2*zPI*cf*dt );
}

/* forgetting-factor to cut-off frequency */
static double _dzSysMAFFF2CF(double ff, double dt){
  return ( 1.0/ff - 1.0 ) / ( 2*zPI*dt );
}

static void _dzSysMAFRefresh(dzSys *sys)
{
  __dz_sys_maf_iov(sys) = 1.0;
  dzSysOutputVal(sys,0) = 0;
}

static zVec _dzSysMAFUpdate(dzSys *sys, double dt)
{
  __dz_sys_maf_iov(sys) = __dz_sys_maf_ff(sys) * __dz_sys_maf_iov(sys) + 1.0;
  dzSysOutputVal(sys,0) +=
    ( dzSysInputVal(sys,0) - dzSysOutputVal(sys,0) ) / __dz_sys_maf_iov(sys);
  return dzSysOutput(sys);
}

static void *_dzSysMAFFFFromZTK(void *val, int i, void *arg, ZTK *ztk){
  *(double*)val = ZTKDouble(ztk);
  return val;
}

static void _dzSysMAFFFFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_maf_ff((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_maf[] = {
  { "ff", 1, _dzSysMAFFFFromZTK, _dzSysMAFFFFPrintZTK },
};

static bool _dzSysMAFRegZTK(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_maf ) ? true : false;
}

static dzSys *_dzSysMAFFromZTK(dzSys *sys, ZTK *ztk)
{
  double ff = 0;
  if( !ZTKEvalKey( &ff, NULL, ztk, __ztk_prp_dzsys_maf ) ) return NULL;
  return dzSysMAFCreate( sys, ff );
}

static void _dzSysMAFFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_maf );
}

dzSysCom dz_sys_maf_com = {
  typestr: "maf",
  _destroy: dzSysDefaultDestroy,
  _refresh: _dzSysMAFRefresh,
  _update: _dzSysMAFUpdate,
  _regZTK: _dzSysMAFRegZTK,
  _fromZTK: _dzSysMAFFromZTK,
  _fprintZTK: _dzSysMAFFPrintZTK,
};

void dzSysMAFSetCF(dzSys *sys, double cf, double dt)
{ /* set forgetting-factor based on the cut-off frequency */
  __dz_sys_maf_ff(sys) = _dzSysMAFCF2FF( cf, dt );
}

double dzSysMAFCF(dzSys *sys, double dt)
{ /* cut-off frequency */
  return _dzSysMAFFF2CF( __dz_sys_maf_ff(sys), dt );
}

/* create a moving-average filter. */
dzSys *dzSysMAFCreate(dzSys *sys, double ff)
{
  dzSysInit( sys );
  sys->com = &dz_sys_maf_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 2 ) ) ) return NULL;
  __dz_sys_maf_ff(sys) = ff;
  dzSysRefresh( sys );
  return sys;
}
