/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_fg - function generators
 */

#include <dzco/dz_sys.h>

#define __dz_sys_fg_t(s)      ((double*)(s)->prp)[0]
#define __dz_sys_fg_amp(s)    ((double*)(s)->prp)[1]
#define __dz_sys_fg_delay(s)  ((double*)(s)->prp)[2]
#define __dz_sys_fg_period(s) ((double*)(s)->prp)[3]

static void _dzSysFGRefresh(dzSys *sys)
{
  __dz_sys_fg_t(sys) = 0;
}

static double _dzSysFGUpdateTime(dzSys *sys, double dt)
{
  if( ( __dz_sys_fg_t(sys) += dt ) > __dz_sys_fg_period(sys) )
    __dz_sys_fg_t(sys) -= __dz_sys_fg_period(sys);
  return __dz_sys_fg_t(sys);
}

static void *_dzSysFGAmpFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFGDelayFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFGPeriodFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}
static void _dzSysFGAmpFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fg_amp((dzSys*)prp) );
}
static void _dzSysFGDelayFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fg_delay((dzSys*)prp) );
}
static void _dzSysFGPeriodFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_fg_period((dzSys*)prp) );
}
static ZTKPrp __ztk_prp_dzsys_fg[] = {
  { "amp", 1, _dzSysFGAmpFromZTK, _dzSysFGAmpFPrintZTK },
  { "delay", 1, _dzSysFGDelayFromZTK, _dzSysFGDelayFPrintZTK },
  { "period", 1, _dzSysFGPeriodFromZTK, _dzSysFGPeriodFPrintZTK },
};

static void _dzSysFGFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_fg );
}

/* ********************************************************** */
/* step function
 * ********************************************************** */

static zVec _dzSysStepUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) =
    _dzSysFGUpdateTime(sys,dt) > __dz_sys_fg_delay(sys) ?
    __dz_sys_fg_amp(sys) : 0;
  return dzSysOutput(sys);
}

dzSysFGDefine( Step, step );

/* ********************************************************** */
/* ramp function
 * ********************************************************** */

static zVec _dzSysRampUpdate(dzSys *sys, double dt)
{
  double t;

  t = _dzSysFGUpdateTime(sys,dt) - __dz_sys_fg_delay(sys);
  dzSysOutputVal(sys,0) = t > 0 ? __dz_sys_fg_amp(sys)*t : 0;
  return dzSysOutput(sys);
}

dzSysFGDefine( Ramp, ramp );

/* ********************************************************** */
/* sinusoidal function
 * ********************************************************** */

static zVec _dzSysSineUpdate(dzSys *sys, double dt)
{
  double t;

  t = _dzSysFGUpdateTime(sys,dt) - __dz_sys_fg_delay(sys);
  dzSysOutputVal(sys,0) = t > 0 ? __dz_sys_fg_amp(sys)*sin(2*zPI*t/__dz_sys_fg_period(sys)) : 0;
  return dzSysOutput(sys);
}

dzSysFGDefine( Sine, sine );

/* ********************************************************** */
/* white noise function
 * ********************************************************** */

static zVec _dzSysWhitenoiseUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0)
    = _dzSysFGUpdateTime(sys,dt) > __dz_sys_fg_delay(sys) ?
    __dz_sys_fg_amp(sys) * zRandF(-1.0,1.0) : 0;
  return dzSysOutput(sys);
}

dzSysFGDefine( Whitenoise, whitenoise );
