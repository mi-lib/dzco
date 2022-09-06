/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_fg - function generators
 */

#ifndef __DZ_SYS_FG_H__
#define __DZ_SYS_FG_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/*! \brief auto prototype generator */
#define dzSysFGPrototype(Type,mtype) \
__EXPORT dzSys *dzSys##Type##Create(dzSys *sys, double amp, double delay, double period);\
extern dzSysCom dz_sys_##mtype##_com

/*! \brief auto definition generator */
#define dzSysFGDefine(Type,mtype) \
static dzSys *_dzSys##Type##FromZTK(dzSys *sys, ZTK *ztk){\
  double val[] = { 0.0, 0.0, 0.0 };\
  if( !ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_fg ) ) return NULL;\
  return dzSys##Type##Create( sys, val[0], val[1], val[2] );\
}\
dzSysCom dz_sys_##mtype##_com = {\
  .typestr = #mtype,\
  ._destroy = dzSysDefaultDestroy,\
  ._refresh = _dzSysFGRefresh,\
  ._update = _dzSys##Type##Update,\
  ._fromZTK = _dzSys##Type##FromZTK,\
  ._fprintZTK = _dzSysFGFPrintZTK,\
};\
dzSys *dzSys##Type##Create(dzSys *sys, double amp, double delay, double period)\
{\
  dzSysInit( sys );\
  sys->com = &dz_sys_##mtype##_com;\
  dzSysAllocInput( sys, 1 );\
  if( dzSysInputNum(sys) != 1 ||\
      !dzSysAllocOutput( sys, 1 ) ||\
      !( sys->prp = zAlloc( double, 4 ) ) ) return NULL;\
  __dz_sys_fg_amp(sys) = amp;\
  __dz_sys_fg_delay(sys) = delay;\
  __dz_sys_fg_period(sys) = period;\
  dzSysRefresh( sys );\
  return sys;\
}

dzSysFGPrototype( Step, step ); /* step */
dzSysFGPrototype( Ramp, ramp ); /* ramp */
dzSysFGPrototype( Sine, sine ); /* sinusoid */
dzSysFGPrototype( Whitenoise, whitenoise ); /* whitenoise */

__END_DECLS

#endif /* __DZ_SYS_FG_H__ */
