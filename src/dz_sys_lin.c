/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_lin - linear system
 */

#include <dzco/dz_sys.h>

/* ********************************************************** */
/* general linear system
 * ********************************************************** */

void dzSysDestroyLin(dzSys *sys)
{
  dzLinDestroy( sys->_prm );
  zFree( sys->_prm );
}

void dzSysRefreshLin(dzSys *sys)
{
  zVecClear( ((dzLin *)sys->_prm)->x );
}

zVec dzSysUpdateLin(dzSys *sys, double dt)
{
  dzLinStateUpdate( sys->_prm, dzSysInputVal(sys,0), dt );
  dzSysOutputVal(sys,0) = dzLinOutput( sys->_prm, dzSysInputVal(sys,0) );
  return dzSysOutput(sys);
}

dzSys *dzSysFReadLin(FILE *fp, dzSys *sys)
{
  dzLin *lin;

  if( !( lin = zAlloc( dzLin, 1 ) ) ){
    ZALLOCERROR();
    return NULL;
  }
  dzLinFRead( fp, lin );
  if( !dzSysCreateLin( sys, lin ) ) sys = NULL;
  return sys;
}

void dzSysFWriteLin(FILE *fp, dzSys *sys)
{
  dzLinFWrite( fp, sys->_prm );
}

dzSysMethod dz_sys_lin_met = {
  type: "lin",
  destroy: dzSysDestroyLin,
  refresh: dzSysRefreshLin,
  update: dzSysUpdateLin,
  fread: dzSysFReadLin,
  fwrite: dzSysFWriteLin,
};

/* dzSysCreateLin
 * - create linear system.
 */
bool dzSysCreateLin(dzSys *sys, dzLin *lin)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_prm = lin;
  sys->_met = &dz_sys_lin_met;
  dzSysRefresh( sys );
  return true;
}
