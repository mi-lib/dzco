#include <dzco/dz_sys.h>

#define DZ_WARN_SYSARRAY_EMPTY "empty array of systems specified."

dzSys *dzSysAssign(dzSys *sys, char *str)
{
  static dzSysCom *com_array[] = {
    &dz_sys_adder_com, &dz_sys_subtr_com, &dz_sys_limit_com,
    &dz_sys_p_com, &dz_sys_i_com, &dz_sys_d_com, &dz_sys_pid_com, &dz_sys_qpd_com,
    &dz_sys_fol_com, &dz_sys_sol_com, &dz_sys_pc_com, &dz_sys_adapt_com,
    &dz_sys_lin_com,
    &dz_sys_pex_com,
    &dz_sys_maf_com, &dz_sys_bw_com,
    &dz_sys_step_com, &dz_sys_ramp_com, &dz_sys_sine_com, &dz_sys_whitenoise_com,
    NULL,
  };
  register int i;

  for( i=0; com_array[i]; i++ ){
    if( strcmp( com_array[i]->typestr, str ) == 0 ){
#if 0
      if( !( sys->prp = ( sys->com = com_array[i] )->alloc() ) ){
        ZALLOCERROR();
        return NULL;
      }
      sys->com->_init( sys->prp );
#else
      sys->com = com_array[i];
#endif
      return sys;
    }
  }
  ZRUNERROR( "%s: unknown type of a system.", str );
  return NULL;
}

static void *_dzSysNameFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return zNameSet( (dzSys*)obj, ZTKVal(ztk) ) ? obj : NULL;
}
static void *_dzSysTypeFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return dzSysAssign( (dzSys*)obj, ZTKVal(ztk) ) ? obj : NULL;
}

static void _dzSysNameFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", zName((dzSys*)obj) );
}
static void _dzSysTypeFPrint(FILE *fp, int i, void *obj){
  fprintf( fp, "%s\n", ((dzSys*)obj)->com->typestr );
}

static ZTKPrp __ztk_prp_dzsys[] = {
  { "name", 1, _dzSysNameFromZTK, _dzSysNameFPrint },
  { "type", 1, _dzSysTypeFromZTK, _dzSysTypeFPrint },
};

static void *dzSysFromZTK(dzSys *sys, ZTK *ztk){
  if( !ZTKEncodeKey( sys, NULL, ztk, __ztk_prp_dzsys ) ) return NULL;
#if 0
  return sys->com->fromZTK( sys, ztk );
#endif
  return sys;
}

static void _dzSysFPrint(FILE *fp, dzSys *sys){
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys );
#if 0
  ((dzSys*)obj)->com->fprint( fp, ((dzSys*)obj)->prp );
#endif
}






static void *_dzSysArraySysFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return dzSysFromZTK( zArrayElemNC((dzSysArray*)obj,i), ztk ) ? obj : NULL;
}

static void *_dzSysArrayConnectFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  if( !ZTKKeyRewind( ztk ) ) return NULL;
  if( !ZTKValRewind( ztk ) ) return NULL;
  do{
    printf( "%s\n", ZTKVal(ztk) );
  } while( ZTKValNext( ztk ) );
  return obj;
}

static void _dzSysArrayConnectFPrint(FILE *fp, int i, void *obj){
  /* to be implemented. */
}

static ZTKPrp __ztk_prp_tag_dzsys[] = {
  { ZTK_TAG_DZSYS, -1, _dzSysArraySysFromZTK, NULL },
  { ZTK_TAG_DZSYS_CONNECT, 1, _dzSysArrayConnectFromZTK, _dzSysArrayConnectFPrint },
};

bool dzSysRegZTK(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys ) &&
         ZTKDefRegTag( ztk, ZTK_TAG_DZSYS_CONNECT ) ? true : false;
}

dzSysArray *dzSysArrayFromZTK(dzSysArray *sarray, ZTK *ztk)
{
  int num_sys;

  if( ( num_sys = ZTKCountTag( ztk, ZTK_TAG_DZSYS ) ) > 0 ){
    zArrayAlloc( sarray, dzSys, num_sys );
    if( zArraySize(sarray) != num_sys ) return NULL;
  } else{
    ZRUNWARN( DZ_WARN_SYSARRAY_EMPTY );
    return NULL;
  }
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
void _dzSysArrayFPrint(FILE *fp, dzSysArray *arr)
{
  register int i;

  for( i=0; i<zArraySize(arr); i++ ){
    fprintf( fp, "[%s]\n", ZTK_TAG_DZSYS );
    _dzSysFPrint( fp, zArrayElemNC(arr,i) );
    fprintf( fp, "\n" );
  }
  ZTKPrpTagFPrint( fp, arr, __ztk_prp_tag_dzsys );
}



int main(int argc, char *argv[])
{
  dzSysArray arr;

  dzSysArrayScanZTK( &arr, argc > 1 ? argv[1] : "scan_test.ztk" );
  _dzSysArrayFPrint( stdout, &arr );
  dzSysArrayDestroy( &arr );
  return 0;
}
