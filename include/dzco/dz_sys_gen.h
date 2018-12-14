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
__EXPORT bool dzSysCreate##Type(dzSys *sys, double amp, double delay, double period);\
extern dzSysMethod dz_sys_##mtype##_met

/*! \brief definition auto-generator */
#define dzSysDefineGen(Type,mtype) \
dzSys *dzSysFRead##Type(FILE *fp, dzSys *sys)\
{\
  double val[] = { 0.0, 0.0, 0.0 };\
  zFieldFRead( fp, _dzSysFReadGen, val );\
  if( !dzSysCreate##Type( sys, val[0], val[1], val[2] ) ) sys = NULL;\
  return sys;\
}\
dzSysMethod dz_sys_##mtype##_met = {\
  type: #mtype,\
  destroy: dzSysDestroyDefault,\
  refresh: dzSysRefreshGen,\
  update: dzSysUpdate##Type,\
  fread: dzSysFRead##Type,\
  fwrite: dzSysFWriteGen,\
};\
bool dzSysCreate##Type(dzSys *sys, double amp, double delay, double period)\
{\
  dzSysInit( sys );\
  dzSysAllocInput( sys, 1 );\
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||\
      !( sys->_prm = zAlloc( double, 4 ) ) ){\
    ZALLOCERROR();\
    return false;\
  }\
  sys->_met = &dz_sys_##mtype##_met;\
  __dz_sys_gen_amp(sys) = amp;\
  __dz_sys_gen_delay(sys) = delay;\
  __dz_sys_gen_period(sys) = period;\
  dzSysRefreshGen( sys );\
  return true;\
}

dzSysPrototypeGen( Step, step ); /* step */
dzSysPrototypeGen( Ramp, ramp ); /* ramp */
dzSysPrototypeGen( Sine, sine ); /* sinusoid */
dzSysPrototypeGen( Whitenoise, whitenoise ); /* whitenoise */

__END_DECLS

#endif /* __DZ_SYS_GEN_H__ */
