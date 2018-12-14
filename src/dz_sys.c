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
  zNameDestroy( sys );
  zArrayFree( dzSysInput(sys) );
  zVecFree( dzSysOutput(sys) );
  zFree( sys->_prm );
  dzSysInit( sys );
}

/* default refreshing method */
void dzSysRefreshDefault(dzSys *sys){}

/* dzSysConnect
 * - connect two systems.
 */
bool dzSysConnect(dzSys *s1, int p1, dzSys *s2, int p2)
{
  if( p1 >= dzSysOutputNum(s1) ){
    ZRUNWARN( "invalid output port of a system %s:%d", zName(s1), p1 );
    return false;
  }
  if( p2 >= dzSysInputNum(s2) ){
    ZRUNWARN( "invalid input port of a system %s:%d", zName(s2), p2 );
    return false;
  }
  dzSysInputElem(s2,p2)->sp = s1;
  dzSysInputElem(s2,p2)->port = p1;
  dzSysInputPtr(s2,p2) = &dzSysOutputVal(s1,p1);
  return true;
}

/* dzSysChain
 * - connect systems.
 */
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

static dzSysMethod *_dzSysMethodByStr(char str[]);

dzSysMethod *_dzSysMethodByStr(char str[])
{
  static dzSysMethod *met_array[] = {
    &dz_sys_adder_met, &dz_sys_subtr_met, &dz_sys_limit_met,
    &dz_sys_p_met, &dz_sys_i_met, &dz_sys_d_met, &dz_sys_pid_met, &dz_sys_qpd_met,
    &dz_sys_fol_met, &dz_sys_sol_met, &dz_sys_pc_met, &dz_sys_adapt_met,
    &dz_sys_lin_met,
    &dz_sys_pex_met,
    &dz_sys_maf_met, &dz_sys_bw_met,
    &dz_sys_step_met, &dz_sys_ramp_met, &dz_sys_sine_met, &dz_sys_whitenoise_met,
    NULL,
  };
  register int i;

  for( i=0; met_array[i]; i++ ){
    if( strcmp( met_array[i]->type, str ) == 0 ) return met_array[i];
  }
  ZRUNERROR( "cannot find a system type %s", str );
  return NULL;
}

typedef struct{
  dzSysMethod *met;
  char name[BUFSIZ];
} _dzSysParam;

/* dzSysFRead
 * - read a system from file.
 */
bool _dzSysFRead(FILE *fp, void *instance, char *buf, bool *success)
{
  if( strcmp( buf, "type" ) == 0 ){
    if( !( ((_dzSysParam *)instance)->met = _dzSysMethodByStr( zFToken(fp,buf,BUFSIZ) ) ) )
      *success = false;
  } else
  if( strcmp( buf, "name" ) == 0 ){
    if( !zFToken( fp, ((_dzSysParam *)instance)->name, BUFSIZ ) )
      *success = false;
  } else
    return false;
  return true;
}

dzSys *dzSysFRead(FILE *fp, dzSys *sys)
{
  _dzSysParam prm;
  int cur;

  prm.met = NULL;
  prm.name[0] = '\0';
  cur = ftell( fp );
  zFieldFRead( fp, _dzSysFRead, &prm );
  if( !prm.met ){
    ZRUNERROR( "type not specified" );
    return NULL;
  }
  fseek( fp, cur, SEEK_SET );
  if( prm.met->fread( fp, sys ) ){
    if( !zNameSet( sys, prm.name ) ){
      ZALLOCERROR();
      return NULL;
    }
    return sys;
  }
  return NULL;
}

/* dzSysFWrite
 * - write a system to file.
 */
void dzSysFWrite(FILE *fp, dzSys *sys)
{
  fprintf( fp, "name: %s\n", zName( sys ) );
  fprintf( fp, "type: %s\n", sys->_met->type );
  sys->_met->fwrite( fp, sys );
}

/* ********************************************************** */
/* \class dzSysArray
 * ********************************************************** */

static bool _dzSysFAlloc(FILE *fp, dzSysArray *arr);

static bool __dzSysArrayConnectFRead(FILE *fp, void *instance, char *buf, bool *success);

static bool _dzSysArrayConnectFRead(FILE *fp, dzSysArray *arr);
static void _dzSysArrayConnectFWrite(FILE *fp, dzSysArray *arr);

/* dzSysArrayDestroy
 * - destroy an array of systems.
 */
void dzSysArrayDestroy(dzSysArray *arr)
{
  register int i;

  for( i=0; i<zArrayNum(arr); i++ )
    dzSysDestroy( zArrayElem(arr,i) );
  zArrayFree( arr );
}

/* (static)
 * _dzSysFAlloc
 * - count subsystems to be allocated in a system chain.
 */
bool _dzSysFAlloc(FILE *fp, dzSysArray *arr)
{
  int n;

  n = zFCountTag( fp, DZ_SYS_TAG );
  zArrayAlloc( arr, dzSys, n );
  if( !zArrayBuf(arr) ){
    ZALLOCERROR();
    return false;
  }
  return true;
}

/* dzSysArrayNameFind
 * - find a system from array by name.
 */
dzSys *dzSysArrayNameFind(dzSysArray *arr, const char *name)
{
  dzSys *sys;

  zArrayFindName( arr, name, sys );
  if( !sys ){
    ZRUNWARN( "invalid system name %s", name );
    return NULL;
  }
  return sys;
}

/* dzSysArrayUpdate
 * - update all systems of an array.
 */
void dzSysArrayUpdate(dzSysArray *arr, double dt)
{
  register int i;

  for( i=0; i<zArrayNum(arr); i++ )
    dzSysUpdate( zArrayElem(arr,i), dt );
}

/* (static)
 * _dzSysArrayConnectFRead
 * - read connectivity information of systems from file.
 */
typedef enum{ DZ_SYS_CONNECT_OUT, DZ_SYS_CONNECT_IN } _dzSysConnectState;

typedef struct{
  dzSysArray *arr;
  _dzSysConnectState state;
  dzSys *sys_out, *sys_in;
  int port_out, port_in;
} _dzSysConnectParam;

bool __dzSysArrayConnectFRead(FILE *fp, void *instance, char *buf, bool *success)
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
    ZRUNWARN( "connection already determined, invalid token %s", buf );
    return false;
  }
  return true;
}

bool _dzSysArrayConnectFRead(FILE *fp, dzSysArray *arr)
{
  _dzSysConnectParam prm;

  prm.arr = arr;
  prm.state = DZ_SYS_CONNECT_OUT;
  return zFieldFRead( fp, __dzSysArrayConnectFRead, &prm );
}

/* (static)
 * _dzSysArrayConnectFWrite
 * - write connectivity information of systems to file.
 */
void _dzSysArrayConnectFWrite(FILE *fp, dzSysArray *arr)
{
  register int i, j;
  dzSys *sys;
  dzSysPort *sp;

  fprintf( fp, "[%s]\n", DZ_SYS_CONNECT_TAG );
  for( i=0; i<zArrayNum(arr); i++ ){
    sys = zArrayElem(arr,i);
    for( j=0; j<dzSysInputNum(sys); j++ )
      if( ( sp = dzSysInputElem(sys,j) )->sp )
        fprintf( fp, "%s %d %s %d\n", zName(sp->sp), sp->port, zName(sys), j );
  }
}

/* dzSysArrayFRead
 * - read an array of systems from file.
 */
typedef struct{
  dzSysArray *arr;
  int count;
} _dzSysArrayParam;

bool _dzSysArrayFRead(FILE *fp, void *instance, char *buf, bool *success)
{
  _dzSysArrayParam *prm;

  prm = instance;
  if( strcmp( buf, DZ_SYS_TAG ) == 0 ){
    if( !dzSysFRead( fp, zArrayElem(prm->arr,prm->count++) ) ){
      *success = false;
      return false;
    }
  } else
  if( strcmp( buf, DZ_SYS_CONNECT_TAG ) == 0 ){
    if( !_dzSysArrayConnectFRead( fp, prm->arr ) )
      *success = false;
  } else
    return false;
  return true;
}

bool dzSysArrayFRead(FILE *fp, dzSysArray *arr)
{
  _dzSysArrayParam prm;

  zArrayInit( arr );
  if( !_dzSysFAlloc( fp, arr ) ) return false;
  rewind( fp );
  prm.count = 0;
  prm.arr = arr;
  return zTagFRead( fp, _dzSysArrayFRead, &prm );
}

/* dzSysArrayFWrite
 * - write an array of systems to file.
 */
void dzSysArrayFWrite(FILE *fp, dzSysArray *arr)
{
  register int i;

  for( i=0; i<zArrayNum(arr); i++ ){
    fprintf( fp, "[%s]\n", DZ_SYS_TAG );
    dzSysFWrite( fp, zArrayElem(arr,i) );
    fprintf( fp, "\n" );
  }
  _dzSysArrayConnectFWrite( fp, arr );
}
