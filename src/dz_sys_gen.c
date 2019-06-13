/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_gen - function generators
 */

#include <dzco/dz_sys.h>

#define __dz_sys_gen_t(s)      ((double*)(s)->_prm)[0]
#define __dz_sys_gen_amp(s)    ((double*)(s)->_prm)[1]
#define __dz_sys_gen_delay(s)  ((double*)(s)->_prm)[2]
#define __dz_sys_gen_period(s) ((double*)(s)->_prm)[3]

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

void dzSysFPrintGen(FILE *fp, dzSys *sys)
{
  fprintf( fp, "amp: %g\n", __dz_sys_gen_amp(sys) );
  fprintf( fp, "delay: %g\n", __dz_sys_gen_delay(sys) );
  fprintf( fp, "period: %g\n", __dz_sys_gen_period(sys) );
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
