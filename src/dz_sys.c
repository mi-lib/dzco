/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys - system class
 */

#include <dzco/dz_sys.h>

#include <stdarg.h>

/* default destroying method */
void dzSysDefaultDestroy(dzSys *sys)
{
  zNameFree( sys );
  zArrayFree( dzSysInput(sys) );
  zVecFree( dzSysOutput(sys) );
  zFree( sys->prp );
  dzSysInit( sys );
}

/* default refreshing method */
void dzSysDefaultRefresh(dzSys *sys){}

/* connect two systems. */
bool dzSysConnect(dzSys *s1, uint p1, dzSys *s2, uint p2)
{
  if( p1 >= dzSysOutputNum(s1) ){
    ZRUNWARN( DZ_WARN_SYS_INVALID_OUTPUTPORT, zName(s1), p1 );
    return false;
  }
  if( p2 >= dzSysInputNum(s2) ){
    ZRUNWARN( DZ_WARN_SYS_INVALID_INPUTPORT, zName(s2), p2 );
    return false;
  }
  dzSysInputElem(s2,p2)->sp = s1;
  dzSysInputElem(s2,p2)->port = p1;
  dzSysInputPtr(s2,p2) = &dzSysOutputVal(s1,p1);
  return true;
}

/* connect multiple systems. */
void dzSysChain(int n, ...)
{
  va_list arg;
  int i;
  dzSys *s1, *s2;

  va_start( arg, n );
  s1 = va_arg( arg, dzSys * );
  for( i=1; i<n; i++ ){
    s2 = va_arg( arg, dzSys * );
    dzSysConnect( s1, 0, s2, 0 );
    s1 = s2;
  }
  va_end( arg );
}

static dzSys *_dzSysQueryAssign(dzSys *sys, const char *str)
{
  DZ_SYS_COM_ARRAY;
  int i;

  for( i=0; _dz_sys_com[i]; i++ )
    if( strcmp( _dz_sys_com[i]->typestr, str ) == 0 ){
      sys->com = _dz_sys_com[i];
      return sys;
    }
  ZRUNERROR( DZ_WARN_SYS_TYPE_UNFOUND, str );
  return NULL;
}

static void *_dzSysNameFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return zNameSet( (dzSys*)obj, ZTKVal(ztk) ) ? obj : NULL;
}
static void *_dzSysTypeFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return _dzSysQueryAssign( (dzSys*)obj, ZTKVal(ztk) ) ? obj : NULL;
}

static void _dzSysNameFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", zName((dzSys*)obj) );
}
static void _dzSysTypeFPrintZTK(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", ((dzSys*)obj)->com ? ((dzSys*)obj)->com->typestr : "unknown" );
}

static ZTKPrp __ztk_prp_dzsys[] = {
  { "name", 1, _dzSysNameFromZTK, _dzSysNameFPrintZTK },
  { "type", 1, _dzSysTypeFromZTK, _dzSysTypeFPrintZTK },
};

void *dzSysFromZTK(dzSys *sys, ZTK *ztk)
{
  char *name;
  if( !ZTKEvalKey( sys, NULL, ztk, __ztk_prp_dzsys ) ) return NULL;
  name = zNamePtr(sys);
  if( !sys->com || !sys->com->_fromZTK( sys, ztk ) ) return NULL;
  zNameSet( sys, name );
  return sys;
}

void dzSysFPrintZTK(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys );
  if( sys->com )
    sys->com->_fprintZTK( fp, sys );
}

/* ********************************************************** */
/* \class dzSysArray
 * ********************************************************** */

/* allocate an array of systems. */
dzSysArray *dzSysArrayAlloc(dzSysArray *arr, uint size)
{
  uint i;

  zArrayAlloc( arr, dzSys, size );
  if( zArraySize(arr) != size ) return NULL;
  for( i=0; i<size; i++ )
    dzSysInit( zArrayElemNC(arr,i) );
  return arr;
}

/* destroy an array of systems. */
void dzSysArrayDestroy(dzSysArray *arr)
{
  uint i;

  for( i=0; i<zArraySize(arr); i++ )
    dzSysDestroy( zArrayElemNC(arr,i) );
  zArrayFree( arr );
}

/* find a system from array by name. */
dzSys *dzSysArrayNameFind(dzSysArray *arr, const char *name)
{
  dzSys *sys;

  zArrayFindName( arr, name, sys );
  if( !sys ){
    ZRUNWARN( DZ_WARN_SYS_NAME_UNFOUND, name );
    return NULL;
  }
  return sys;
}

/* update all systems of an array. */
void dzSysArrayUpdate(dzSysArray *arr, double dt)
{
  uint i;

  for( i=0; i<zArraySize(arr); i++ )
    dzSysUpdate( zArrayElemNC(arr,i), dt );
}

/* scan connectivity information of systems from a file. */
typedef enum{ DZ_SYS_CONNECT_OUT, DZ_SYS_CONNECT_IN } _dzSysConnectState;

static void *_dzSysArraySysFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return dzSysFromZTK( zArrayElemNC((dzSysArray*)obj,i), ztk ) ? obj : NULL;
}

static void *_dzSysArrayConnectFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  _dzSysConnectState state = DZ_SYS_CONNECT_OUT;
  dzSys *sys_out, *sys_in;
  uint port_out, port_in;

  if( !ZTKKeyRewind( ztk ) || !ZTKValRewind( ztk ) ) return NULL;
  sys_out = sys_in = NULL;
  port_out = port_in = 0;
  do{
    switch( state ){
    case DZ_SYS_CONNECT_OUT:
      if( !( sys_out = dzSysArrayNameFind( (dzSysArray *)obj, ZTKVal(ztk) ) ) ) return NULL;
      ZTKValNext( ztk );
      port_out = ZTKInt(ztk);
      state = DZ_SYS_CONNECT_IN;
      break;
    case DZ_SYS_CONNECT_IN:
      if( !( sys_in = dzSysArrayNameFind( (dzSysArray *)obj, ZTKVal(ztk) ) ) ) return NULL;
      ZTKValNext( ztk );
      port_in = ZTKInt(ztk);
      if( !dzSysConnect( sys_out, port_out, sys_in, port_in ) ) return NULL;
      state = DZ_SYS_CONNECT_OUT;
      break;
    default:
      ZRUNWARN( DZ_WARN_SYS_ALREADYCONNECTED, ZTKVal(ztk) );
      return NULL;
    }
  } while( ZTKValPtr(ztk) );
  return obj;
}

static void _dzSysArrayConnectFPrintZTK(FILE *fp, int i, void *obj){
  uint j, k;
  dzSys *sys;
  dzSysPort *sp;

  for( j=0; j<zArraySize((dzSysArray*)obj); j++ ){
    sys = zArrayElemNC((dzSysArray*)obj,j);
    for( k=0; k<dzSysInputNum(sys); k++ )
      if( ( sp = dzSysInputElem(sys,k) )->sp )
        fprintf( fp, "%s %d %s %d\n", zName(sp->sp), sp->port, zName(sys), k );
  }
  fprintf( fp, "\n" );
}

static ZTKPrp __ztk_prp_tag_dzsys[] = {
  { ZTK_TAG_DZSYS, -1, _dzSysArraySysFromZTK, NULL },
  { ZTK_TAG_DZSYS_CONNECT, 1, _dzSysArrayConnectFromZTK, _dzSysArrayConnectFPrintZTK },
};

/* read the current position of a ZTK file and create an array of systems. */
dzSysArray *dzSysArrayFromZTK(dzSysArray *sarray, ZTK *ztk)
{
  uint num_sys;

  if( ( num_sys = ZTKCountTag( ztk, ZTK_TAG_DZSYS ) ) == 0 ){
    ZRUNWARN( DZ_WARN_SYSARRAY_EMPTY );
    return NULL;
  }
  if( !dzSysArrayAlloc( sarray, num_sys ) ) return NULL;
  ZTKEvalTag( sarray, NULL, ztk, __ztk_prp_tag_dzsys );
  return sarray;
}

/* print an array of systems to the current position of a ZTK file. */
void dzSysArrayFPrintZTK(FILE *fp, dzSysArray *arr)
{
  uint i;

  for( i=0; i<zArraySize(arr); i++ ){
    fprintf( fp, "[%s]\n", ZTK_TAG_DZSYS );
    dzSysFPrintZTK( fp, zArrayElemNC(arr,i) );
    fprintf( fp, "\n" );
  }
  ZTKPrpTagFPrint( fp, arr, __ztk_prp_tag_dzsys );
}

/* read a ZTK file and create an array of systems. */
dzSysArray *dzSysArrayReadZTK(dzSysArray *sarray, char filename[])
{
  ZTK ztk;

  ZTKInit( &ztk );
  zArrayInit( sarray );
  if( ZTKParse( &ztk, filename ) )
    sarray = dzSysArrayFromZTK( sarray, &ztk );
  ZTKDestroy( &ztk );
  return sarray;
}

/* write an array of systems to a file in ZTK format. */
bool dzSysArrayWriteZTK(dzSysArray *sarray, char filename[])
{
  FILE *fp;

  if( !( fp = zOpenZTKFile( filename, "w" ) ) ) return false;
  dzSysArrayFPrintZTK( fp, sarray );
  fclose(fp);
  return true;
}
