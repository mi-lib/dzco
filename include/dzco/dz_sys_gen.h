/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_gen - function generators
 */

#ifndef __DZ_SYS_GEN_H__
#define __DZ_SYS_GEN_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/*! \brief prototype auto-generator */
#define dzSysPrototypeGen(Type,mtype) \
__EXPORT dzSys *dzSysCreate##Type(dzSys *sys, double amp, double delay, double period);\
extern dzSysCom dz_sys_##mtype##_com

/*! \brief definition auto-generator */
#define dzSysDefineGen(Type,mtype) \
dzSys *dzSysFScan##Type(FILE *fp, dzSys *sys){\
  double val[] = { 0.0, 0.0, 0.0 };\
  zFieldFScan( fp, _dzSysFScanGen, val );\
  return dzSysCreate##Type( sys, val[0], val[1], val[2] );\
}\
static dzSys *_dzSysFromZTK##Type(dzSys *sys, ZTK *ztk){\
  double val[] = { 0.0, 0.0, 0.0 };\
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_gen ) ) return NULL;\
  return dzSysCreate##Type( sys, val[0], val[1], val[2] );\
}\
dzSysCom dz_sys_##mtype##_com = {\
  typestr: #mtype,\
  destroy: dzSysDestroyDefault,\
  refresh: dzSysRefreshGen,\
  update: dzSysUpdate##Type,\
  fscan: dzSysFScan##Type,\
  fromZTK: _dzSysFromZTK##Type,\
  regZTK: _dzSysRegZTKGen,\
  fprint: _dzSysFPrintGen,\
};\
dzSys *dzSysCreate##Type(dzSys *sys, double amp, double delay, double period)\
{\
  dzSysInit( sys );\
  sys->com = &dz_sys_##mtype##_com;\
  dzSysAllocInput( sys, 1 );\
  if( dzSysInputNum(sys) != 1 ||\
      !dzSysAllocOutput( sys, 1 ) ||\
      !( sys->prp = zAlloc( double, 4 ) ) ) return NULL;\
  __dz_sys_gen_amp(sys) = amp;\
  __dz_sys_gen_delay(sys) = delay;\
  __dz_sys_gen_period(sys) = period;\
  dzSysRefreshGen( sys );\
  return sys;\
}

dzSysPrototypeGen( Step, step ); /* step */
dzSysPrototypeGen( Ramp, ramp ); /* ramp */
dzSysPrototypeGen( Sine, sine ); /* sinusoid */
dzSysPrototypeGen( Whitenoise, whitenoise ); /* whitenoise */

__END_DECLS

#endif /* __DZ_SYS_GEN_H__ */
