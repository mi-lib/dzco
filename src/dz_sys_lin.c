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
  dzLinDestroy( sys->prp );
  zFree( sys->prp );
}

void dzSysRefreshLin(dzSys *sys)
{
  zVecZero( ((dzLin *)sys->prp)->x );
}

zVec dzSysUpdateLin(dzSys *sys, double dt)
{
  dzLinStateUpdate( sys->prp, dzSysInputVal(sys,0), dt );
  dzSysOutputVal(sys,0) = dzLinOutput( sys->prp, dzSysInputVal(sys,0) );
  return dzSysOutput(sys);
}

dzSys *dzSysFScanLin(FILE *fp, dzSys *sys)
{
  dzLin *lin;

  if( !( lin = zAlloc( dzLin, 1 ) ) ) return NULL;
  dzLinFScan( fp, lin );
  return dzSysCreateLin( sys, lin );
}

void dzSysFPrintLin(FILE *fp, dzSys *sys)
{
  dzLinFPrint( fp, sys->prp );
}

static bool _dzSysRegZTKLin(ZTK *ztk)
{
  return dzLinRegZTK( ztk, ZTK_TAG_DZSYS );
}

static dzSys *_dzSysFromZTKLin(dzSys *sys, ZTK *ztk)
{
  dzLin *lin;

  if( !( lin = zAlloc( dzLin, 1 ) ) ) return NULL;
  if( !dzLinFromZTK( lin, ztk ) ) return NULL;
  return dzSysCreateLin( sys, lin );
}

dzSysCom dz_sys_lin_com = {
  typestr: "lin",
  destroy: dzSysDestroyLin,
  refresh: dzSysRefreshLin,
  update: dzSysUpdateLin,
  fscan: dzSysFScanLin,
  regZTK: _dzSysRegZTKLin,
  fromZTK: _dzSysFromZTKLin,
  fprint: dzSysFPrintLin,
};

/* create a linear system. */
dzSys *dzSysCreateLin(dzSys *sys, dzLin *lin)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return NULL;
  }
  sys->prp = lin;
  sys->com = &dz_sys_lin_com;
  dzSysRefresh( sys );
  return sys;
}
