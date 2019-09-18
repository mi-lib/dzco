/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_lin - linear system
 */

#include <dzco/dz_sys.h>

/* ********************************************************** */
/* general linear system
 * ********************************************************** */

static void _dzSysLinDestroy(dzSys *sys)
{
  dzLinDestroy( sys->prp );
  zFree( sys->prp );
}

static void _dzSysLinRefresh(dzSys *sys)
{
  zVecZero( ((dzLin *)sys->prp)->x );
}

static zVec _dzSysLinUpdate(dzSys *sys, double dt)
{
  dzLinStateUpdate( sys->prp, dzSysInputVal(sys,0), dt );
  dzSysOutputVal(sys,0) = dzLinOutput( sys->prp, dzSysInputVal(sys,0) );
  return dzSysOutput(sys);
}

static void _dzSysLinFPrintZTK(FILE *fp, dzSys *sys)
{
  dzLinFPrintZTK( fp, sys->prp );
}

static bool _dzSysLinRegZTK(ZTK *ztk)
{
  return dzLinRegZTK( ztk, ZTK_TAG_DZSYS );
}

static dzSys *_dzSysLinFromZTK(dzSys *sys, ZTK *ztk)
{
  dzLin *lin;

  if( !( lin = zAlloc( dzLin, 1 ) ) ) return NULL;
  if( !dzLinFromZTK( lin, ztk ) ) return NULL;
  return dzSysLinCreate( sys, lin );
}

dzSysCom dz_sys_lin_com = {
  typestr: "lin",
  _destroy: _dzSysLinDestroy,
  _refresh: _dzSysLinRefresh,
  _update: _dzSysLinUpdate,
  _regZTK: _dzSysLinRegZTK,
  _fromZTK: _dzSysLinFromZTK,
  _fprintZTK: _dzSysLinFPrintZTK,
};

/* create a linear system. */
dzSys *dzSysLinCreate(dzSys *sys, dzLin *lin)
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
