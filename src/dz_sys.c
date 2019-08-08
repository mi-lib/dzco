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
  zFree( sys->_prm );
  dzSysInit( sys );
}

/* default refreshing method */
void dzSysRefreshDefault(dzSys *sys){}

/* connect two systems. */
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

/* scan a system from a file. */
bool _dzSysFScan(FILE *fp, void *instance, char *buf, bool *success)
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

dzSys *dzSysFScan(FILE *fp, dzSys *sys)
{
  _dzSysParam prm;
  int cur;

  prm.met = NULL;
  prm.name[0] = '\0';
  cur = ftell( fp );
  zFieldFScan( fp, _dzSysFScan, &prm );
  if( !prm.met ){
    ZRUNERROR( "type not specified" );
    return NULL;
  }
  fseek( fp, cur, SEEK_SET );
  if( prm.met->fscan( fp, sys ) ){
    if( !zNameSet( sys, prm.name ) ){
      ZALLOCERROR();
      return NULL;
    }
    return sys;
  }
  return NULL;
}

/* print a system to a file. */
void dzSysFPrint(FILE *fp, dzSys *sys)
{
  fprintf( fp, "name: %s\n", zName( sys ) );
  fprintf( fp, "type: %s\n", sys->_met->type );
  sys->_met->fprint( fp, sys );
}

/* ********************************************************** */
/* \class dzSysArray
 * ********************************************************** */

static bool _dzSysFAlloc(FILE *fp, dzSysArray *arr);

static bool __dzSysArrayConnectFScan(FILE *fp, void *instance, char *buf, bool *success);

static bool _dzSysArrayConnectFScan(FILE *fp, dzSysArray *arr);
static void _dzSysArrayConnectFPrint(FILE *fp, dzSysArray *arr);

/* destroy an array of systems. */
void dzSysArrayDestroy(dzSysArray *arr)
{
  register int i;

  for( i=0; i<zArraySize(arr); i++ )
    dzSysDestroy( zArrayElemNC(arr,i) );
  zArrayFree( arr );
}

/* (static)
 * count subsystems to be allocated in a system chain. */
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

/* find a system from array by name. */
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

/* update all systems of an array. */
void dzSysArrayUpdate(dzSysArray *arr, double dt)
{
  register int i;

  for( i=0; i<zArraySize(arr); i++ )
    dzSysUpdate( zArrayElemNC(arr,i), dt );
}

/* (static)
 * scan connectivity information of systems from a file. */
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
    ZRUNWARN( "connection already determined, invalid token %s", buf );
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

/* (static)
 * print connectivity information of systems to a file. */
void _dzSysArrayConnectFPrint(FILE *fp, dzSysArray *arr)
{
  register int i, j;
  dzSys *sys;
  dzSysPort *sp;

  fprintf( fp, "[%s]\n", DZ_SYS_CONNECT_TAG );
  for( i=0; i<zArraySize(arr); i++ ){
    sys = zArrayElemNC(arr,i);
    for( j=0; j<dzSysInputNum(sys); j++ )
      if( ( sp = dzSysInputElem(sys,j) )->sp )
        fprintf( fp, "%s %d %s %d\n", zName(sp->sp), sp->port, zName(sys), j );
  }
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
  if( strcmp( buf, DZ_SYS_TAG ) == 0 ){
    if( !dzSysFScan( fp, zArrayElemNC(prm->arr,prm->count++) ) ){
      *success = false;
      return false;
    }
  } else
  if( strcmp( buf, DZ_SYS_CONNECT_TAG ) == 0 ){
    if( !_dzSysArrayConnectFScan( fp, prm->arr ) )
      *success = false;
  } else
    return false;
  return true;
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

/* print an array of systems to a file. */
void dzSysArrayFPrint(FILE *fp, dzSysArray *arr)
{
  register int i;

  for( i=0; i<zArraySize(arr); i++ ){
    fprintf( fp, "[%s]\n", DZ_SYS_TAG );
    dzSysFPrint( fp, zArrayElemNC(arr,i) );
    fprintf( fp, "\n" );
  }
  _dzSysArrayConnectFPrint( fp, arr );
}
