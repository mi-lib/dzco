/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_gen - function generators
 */

#include <dzco/dz_sys.h>

#define __dz_sys_gen_t(s)      ((double*)(s)->prp)[0]
#define __dz_sys_gen_amp(s)    ((double*)(s)->prp)[1]
#define __dz_sys_gen_delay(s)  ((double*)(s)->prp)[2]
#define __dz_sys_gen_period(s) ((double*)(s)->prp)[3]

static bool _dzSysFScanGen(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshGen(dzSys *sys)
{
  __dz_sys_gen_t(sys) = 0;
}

double dzSysUpdateTimeGen(dzSys *sys, double dt)
{
  if( ( __dz_sys_gen_t(sys) += dt ) > __dz_sys_gen_period(sys) )
    __dz_sys_gen_t(sys) -= __dz_sys_gen_period(sys);
  return __dz_sys_gen_t(sys);
}

bool _dzSysFScanGen(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "amp" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "delay" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
  if( strcmp( buf, "period" ) == 0 ){
    ((double *)val)[2] = zFDouble( fp );
  } else
    return false;
  return true;
}

static void *_dzSysFromZTKGenAmp(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKGenDelay(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKGenPeriod(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}
static void _dzSysFPrintGenAmp(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_gen_amp((dzSys*)prp) );
}
static void _dzSysFPrintGenDelay(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_gen_delay((dzSys*)prp) );
}
static void _dzSysFPrintGenPeriod(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_gen_period((dzSys*)prp) );
}
static ZTKPrp __ztk_prp_dzsys_gen[] = {
  { "amp", 1, _dzSysFromZTKGenAmp, _dzSysFPrintGenAmp },
  { "delay", 1, _dzSysFromZTKGenDelay, _dzSysFPrintGenDelay },
  { "period", 1, _dzSysFromZTKGenPeriod, _dzSysFPrintGenPeriod },
};

static bool _dzSysRegZTKGen(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_gen ) ? true : false;
}

static void _dzSysFPrintGen(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_gen );
}

/* ********************************************************** */
/* step function
 * ********************************************************** */

zVec dzSysUpdateStep(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) =
    dzSysUpdateTimeGen(sys,dt) > __dz_sys_gen_delay(sys) ?
    __dz_sys_gen_amp(sys) : 0;
  return dzSysOutput(sys);
}

dzSysDefineGen( Step, step );

/* ********************************************************** */
/* ramp function
 * ********************************************************** */

zVec dzSysUpdateRamp(dzSys *sys, double dt)
{
  double t;

  t = dzSysUpdateTimeGen(sys,dt) - __dz_sys_gen_delay(sys);
  dzSysOutputVal(sys,0) = t > 0 ? __dz_sys_gen_amp(sys)*t : 0;
  return dzSysOutput(sys);
}

dzSysDefineGen( Ramp, ramp );

/* ********************************************************** */
/* sinusoidal function
 * ********************************************************** */

zVec dzSysUpdateSine(dzSys *sys, double dt)
{
  double t;

  t = dzSysUpdateTimeGen(sys,dt) - __dz_sys_gen_delay(sys);
  dzSysOutputVal(sys,0) = t > 0 ? __dz_sys_gen_amp(sys)*sin(2*zPI*t/__dz_sys_gen_period(sys)) : 0;
  return dzSysOutput(sys);
}

dzSysDefineGen( Sine, sine );

/* ********************************************************** */
/* white noise function
 * ********************************************************** */

zVec dzSysUpdateWhitenoise(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0)
    = dzSysUpdateTimeGen(sys,dt) > __dz_sys_gen_delay(sys) ?
    __dz_sys_gen_amp(sys) * zRandF(-1.0,1.0) : 0;
  return dzSysOutput(sys);
}

dzSysDefineGen( Whitenoise, whitenoise );
