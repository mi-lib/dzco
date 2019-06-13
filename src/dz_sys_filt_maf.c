/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_filt_maf - moving-average filter
 */

#include <dzco/dz_sys.h>

#define __dz_sys_maf_ff(s)  ( ((double*)(s)->_prm)[0] )
#define __dz_sys_maf_iov(s) ( ((double*)(s)->_prm)[1] )

static double _dzSysMAFCF2FF(double cf, double dt);
static double _dzSysMAFFF2CF(double ff, double dt);

static bool _dzSysFScanMAF(FILE *fp, void *val, char *buf, bool *success);

/* cut-off frequency to forgetting-factor */
double _dzSysMAFCF2FF(double cf, double dt){
  return 1.0 / ( 1 + 2*zPI*cf*dt );
}

/* forgetting-factor to cut-off frequency */
double _dzSysMAFFF2CF(double ff, double dt){
  return ( 1.0/ff - 1.0 ) / ( 2*zPI*dt );
}

void dzSysRefreshMAF(dzSys *sys)
{
  __dz_sys_maf_iov(sys) = 1.0;
  dzSysOutputVal(sys,0) = 0;
}

zVec dzSysUpdateMAF(dzSys *sys, double dt)
{
  __dz_sys_maf_iov(sys) = __dz_sys_maf_ff(sys) * __dz_sys_maf_iov(sys) + 1.0;
  dzSysOutputVal(sys,0) +=
    ( dzSysInputVal(sys,0) - dzSysOutputVal(sys,0) ) / __dz_sys_maf_iov(sys);
  return dzSysOutput(sys);
}

bool _dzSysFScanMAF(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "ff" ) == 0 ){
    *(double *)val = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanMAF(FILE *fp, dzSys *sys)
{
  double ff = 0;

  zFieldFScan( fp, _dzSysFScanMAF, &ff );
  return dzSysCreateMAF( sys, ff ) ? sys : NULL;
}

void dzSysFPrintMAF(FILE *fp, dzSys *sys)
{
  fprintf( fp, "ff: %g\n", __dz_sys_maf_ff(sys) );
}

dzSysMethod dz_sys_maf_met = {
  type: "maf",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshMAF,
  update: dzSysUpdateMAF,
  fscan: dzSysFScanMAF,
  fprint: dzSysFPrintMAF,
};

void dzSysMAFSetCF(dzSys *sys, double cf, double dt)
{ /* set forgetting-factor based on the cut-off frequency */
  __dz_sys_maf_ff(sys) = _dzSysMAFCF2FF( cf, dt );
}

double dzSysMAFCF(dzSys *sys, double dt)
{ /* cut-off frequency */
  return _dzSysMAFFF2CF( __dz_sys_maf_ff(sys), dt );
}

/* dzSysCreateMAF
 * - create a moving-average filter.
 */
bool dzSysCreateMAF(dzSys *sys, double ff)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 2 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_maf_met;
  __dz_sys_maf_ff(sys) = ff;
  dzSysRefresh( sys );
  return true;
}
