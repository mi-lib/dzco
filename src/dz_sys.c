/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys - system class
 */

#include <dzco/dz_sys.h>

#include <stdarg.h>

/* default destroying method */
void dzSysDestroyDefault(dzSys *sys)
{
  zNameFree( sys );
  zArrayFree( dzSysInput(sys) );
  zVecFree( dzSysOutput(sys) );
  zFree( sys->prp );
  dzSysInit( sys );
}

/* default refreshing method */
void dzSysRefreshDefault(dzSys *sys){}

/* connect two systems. */
bool dzSysConnect(dzSys *s1, int p1, dzSys *s2, int p2)
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
  register int i;
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

static dzSysCom *_dzSysComByStr(char str[])
{
  DZ_SYS_COM_ARRAY;
  register int i;

  for( i=0; _dz_sys_com[i]; i++ )
    if( strcmp( _dz_sys_com[i]->typestr, str ) == 0 ) return _dz_sys_com[i];
  ZRUNWARN( DZ_WARN_SYS_TYPE_UNFOUND, str );
  return NULL;
}

typedef struct{
  dzSysCom *com;
  char name[BUFSIZ];
} _dzSysParam;

/* scan a system from a file. */
bool _dzSysFScan(FILE *fp, void *instance, char *buf, bool *success)
{
  if( strcmp( buf, "type" ) == 0 ){
    if( !( ((_dzSysParam *)instance)->com = _dzSysComByStr( zFToken(fp,buf,BUFSIZ) ) ) )
      *success = false;
  } else
  if( strcmp( buf, "name" ) == 0 ){
    if( !zFToken( fp, ((_dzSysParam *)instance)->name, BUFSIZ ) )
      *success = false;
  } else
    return false;
  return true;
}

dzSys *dzSysFScan(FILE *fp, dzSys *sys)
{
  _dzSysParam prm;
  int cur;

  prm.com = NULL;
  prm.name[0] = '\0';
  cur = ftell( fp );
  zFieldFScan( fp, _dzSysFScan, &prm );
  if( !prm.com ){
    ZRUNERROR( DZ_ERR_SYS_TYPE_UNSPECIFIED );
    return NULL;
  }
  fseek( fp, cur, SEEK_SET );
  if( prm.com->fscan( fp, sys ) ){
    if( !zNameSet( sys, prm.name ) ){
      ZALLOCERROR();
      return NULL;
    }
    return sys;
  }
  return NULL;
}

static dzSys *_dzSysQueryAssign(dzSys *sys, char *str)
{
  DZ_SYS_COM_ARRAY;
  register int i;

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

static void _dzSysNameFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", zName((dzSys*)obj) );
}
static void _dzSysTypeFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", ((dzSys*)obj)->com ? ((dzSys*)obj)->com->typestr : "unknown" );
}

static ZTKPrp __ztk_prp_dzsys[] = {
  { "name", 1, _dzSysNameFromZTK, _dzSysNameFPrint },
  { "type", 1, _dzSysTypeFromZTK, _dzSysTypeFPrint },
};

void *dzSysFromZTK(dzSys *sys, ZTK *ztk)
{
  char *name;
  if( !ZTKEncodeKey( sys, NULL, ztk, __ztk_prp_dzsys ) ) return NULL;
  name = zNamePtr(sys);
  if( !sys->com || !sys->com->fromZTK( sys, ztk ) ) return NULL;
  zNameSet( sys, name );
  return sys;
}

void dzSysFPrint(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys );
  if( sys->com )
    sys->com->fprint( fp, sys );
}

/* ********************************************************** */
/* \class dzSysArray
 * ********************************************************** */

static bool _dzSysFAlloc(FILE *fp, dzSysArray *arr);

static bool __dzSysArrayConnectFScan(FILE *fp, void *instance, char *buf, bool *success);

static bool _dzSysArrayConnectFScan(FILE *fp, dzSysArray *arr);

/* allocate an array of systems. */
dzSysArray *dzSysArrayAlloc(dzSysArray *arr, int size)
{
  register int i;

  zArrayAlloc( arr, dzSys, size );
  if( zArraySize(arr) != size ) return NULL;
  for( i=0; i<size; i++ )
    dzSysInit( zArrayElemNC(arr,i) );
  return arr;
}

/* destroy an array of systems. */
void dzSysArrayDestroy(dzSysArray *arr)
{
  register int i;

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
  register int i;

  for( i=0; i<zArraySize(arr); i++ )
    dzSysUpdate( zArrayElemNC(arr,i), dt );
}

/* scan connectivity information of systems from a file. */
typedef enum{ DZ_SYS_CONNECT_OUT, DZ_SYS_CONNECT_IN } _dzSysConnectState;

typedef struct{
  dzSysArray *arr;
  _dzSysConnectState state;
  dzSys *sys_out, *sys_in;
  int port_out, port_in;
} _dzSysConnectParam;

bool __dzSysArrayConnectFScan(FILE *fp, void *instance, char *buf, bool *success)
{
  _dzSysConnectParam *prm;

  prm = instance;
  switch( prm->state ){
  case DZ_SYS_CONNECT_OUT:
    if( !( prm->sys_out = dzSysArrayNameFind( prm->arr, buf ) ) ){
      *success = false;
      return false;
    }
    prm->port_out = zFInt( fp );
    prm->state = DZ_SYS_CONNECT_IN;
    break;
  case DZ_SYS_CONNECT_IN:
    if( !( prm->sys_in = dzSysArrayNameFind( prm->arr, buf ) ) ){
      *success = false;
      return false;
    }
    prm->port_in = zFInt( fp );
    if( !dzSysConnect( prm->sys_out, prm->port_out, prm->sys_in, prm->port_in ) ){
      *success = false;
      return false;
    }
    prm->state = DZ_SYS_CONNECT_OUT;
    break;
  default:
    ZRUNWARN( DZ_WARN_SYS_ALREADYCONNECTED, buf );
    return false;
  }
  return true;
}

bool _dzSysArrayConnectFScan(FILE *fp, dzSysArray *arr)
{
  _dzSysConnectParam prm;

  prm.arr = arr;
  prm.state = DZ_SYS_CONNECT_OUT;
  return zFieldFScan( fp, __dzSysArrayConnectFScan, &prm );
}

/* scan an array of systems from a file. */
typedef struct{
  dzSysArray *arr;
  int count;
} _dzSysArrayParam;

bool _dzSysArrayFScan(FILE *fp, void *instance, char *buf, bool *success)
{
  _dzSysArrayParam *prm;

  prm = instance;
  if( strcmp( buf, ZTK_TAG_DZSYS ) == 0 ){
    if( !dzSysFScan( fp, zArrayElemNC(prm->arr,prm->count++) ) ){
      *success = false;
      return false;
    }
  } else
  if( strcmp( buf, ZTK_TAG_DZSYS_CONNECT ) == 0 ){
    if( !_dzSysArrayConnectFScan( fp, prm->arr ) )
      *success = false;
  } else
    return false;
  return true;
}

/* (static)
 * count subsystems to be allocated in a system chain. */
bool _dzSysFAlloc(FILE *fp, dzSysArray *arr)
{
  return dzSysArrayAlloc( arr, zFCountTag( fp, ZTK_TAG_DZSYS ) ) ? true : false;
}

bool dzSysArrayFScan(FILE *fp, dzSysArray *arr)
{
  _dzSysArrayParam prm;

  zArrayInit( arr );
  if( !_dzSysFAlloc( fp, arr ) ) return false;
  rewind( fp );
  prm.count = 0;
  prm.arr = arr;
  return zTagFScan( fp, _dzSysArrayFScan, &prm );
}

static void *_dzSysArraySysFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return dzSysFromZTK( zArrayElemNC((dzSysArray*)obj,i), ztk ) ? obj : NULL;
}

static void *_dzSysArrayConnectFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  _dzSysConnectState state = DZ_SYS_CONNECT_OUT;
  dzSys *sys_out, *sys_in;
  int port_out, port_in;

  if( !ZTKKeyRewind( ztk ) || !ZTKValRewind( ztk ) ) return NULL;
  sys_out = sys_in = NULL;
  port_out = port_in = 0;
  do{
    switch( state ){
    case DZ_SYS_CONNECT_OUT:
      if( !( sys_out = dzSysArrayNameFind( obj, ZTKVal(ztk) ) ) ) return NULL;
      ZTKValNext( ztk );
      port_out = ZTKInt(ztk);
      state = DZ_SYS_CONNECT_IN;
      break;
    case DZ_SYS_CONNECT_IN:
      if( !( sys_in = dzSysArrayNameFind( obj, ZTKVal(ztk) ) ) ) return NULL;
      ZTKValNext( ztk );
      port_in = ZTKInt(ztk);
      if( !dzSysConnect( sys_out, port_out, sys_in, port_in ) ) return NULL;
      state = DZ_SYS_CONNECT_OUT;
      break;
    default:
      ZRUNWARN( "connection already determined, invalid token %s", ZTKVal(ztk) );
      return NULL;
    }
  } while( ZTKValPtr(ztk) );
  return obj;
}

static void _dzSysArrayConnectFPrint(FILE *fp, int i, void *obj){
  register int j, k;
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
  { ZTK_TAG_DZSYS_CONNECT, 1, _dzSysArrayConnectFromZTK, _dzSysArrayConnectFPrint },
};

bool dzSysRegZTK(ZTK *ztk)
{
  DZ_SYS_COM_ARRAY;
  register int i;

  if( !ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys ) ) return false;
  for( i=0; _dz_sys_com[i]; i++ )
    if( !_dz_sys_com[i]->regZTK( ztk ) ) return false;
  return ZTKDefRegTag( ztk, ZTK_TAG_DZSYS_CONNECT ) ? true : false;
}

dzSysArray *dzSysArrayFromZTK(dzSysArray *sarray, ZTK *ztk)
{
  int num_sys;

  if( ( num_sys = ZTKCountTag( ztk, ZTK_TAG_DZSYS ) ) == 0 ){
    ZRUNWARN( DZ_WARN_SYSARRAY_EMPTY );
    return NULL;
  }
  if( !dzSysArrayAlloc( sarray, num_sys ) ) return NULL;
  ZTKEncodeTag( sarray, NULL, ztk, __ztk_prp_tag_dzsys );
  return sarray;
}

dzSysArray *dzSysArrayScanZTK(dzSysArray *sarray, char filename[])
{
  ZTK ztk;

  ZTKInit( &ztk );
  if( !dzSysRegZTK( &ztk ) ) return NULL;
  zArrayInit( sarray );
  if( ZTKParse( &ztk, filename ) )
    sarray = dzSysArrayFromZTK( sarray, &ztk );
  ZTKDestroy( &ztk );
  return sarray;
}

/* print an array of systems to a file. */
void dzSysArrayFPrint(FILE *fp, dzSysArray *arr)
{
  register int i;

  for( i=0; i<zArraySize(arr); i++ ){
    fprintf( fp, "[%s]\n", ZTK_TAG_DZSYS );
    dzSysFPrint( fp, zArrayElemNC(arr,i) );
    fprintf( fp, "\n" );
  }
  ZTKPrpTagFPrint( fp, arr, __ztk_prp_tag_dzsys );
}
