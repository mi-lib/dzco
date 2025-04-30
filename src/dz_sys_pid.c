/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_pid - PID controller
 */

#include <dzco/dz_sys.h>

/* ********************************************************** */
/* proportional amplifier
 * ********************************************************** */

#define __dz_sys_p_gain(s) ( *(double *)(s)->prp )

static zVec _dzSysPUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = __dz_sys_p_gain(sys) * dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

static void *_dzSysPGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  *((double*)val) = ZTKDouble(ztk);
  return val;
}

static bool _dzSysPGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_p_gain((dzSys*)prp) );
  return true;
}

static const ZTKPrp __ztk_prp_dzsys_p[] = {
  { ZTK_KEY_DZCO_SYS_GAIN, 1, _dzSysPGainFromZTK, _dzSysPGainFPrintZTK },
};

static dzSys *_dzSysPFromZTK(dzSys *sys, ZTK *ztk)
{
  double gain = 0;
  if( !_ZTKEvalKey( &gain, NULL, ztk, __ztk_prp_dzsys_p ) ) return NULL;
  return dzSysPCreate( sys, gain );
}

static void _dzSysPFPrintZTK(FILE *fp, dzSys *sys)
{
  _ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_p );
}

dzSysCom dz_sys_p_com = {
  .typestr = "amplifier",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = dzSysDefaultRefresh,
  ._update = _dzSysPUpdate,
  ._fromZTK = _dzSysPFromZTK,
  ._fprintZTK = _dzSysPFPrintZTK,
};

/* create a proportional amplifier. */
dzSys *dzSysPCreate(dzSys *sys, double gain)
{
  dzSysInit( sys );
  sys->com = &dz_sys_p_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 1 ) ) ) return NULL;
  dzSysPSetGain( sys, gain );
  return sys;
}

void dzSysPSetGain(dzSys *sys, double gain)
{
  __dz_sys_p_gain(sys) = gain;
}

/* ********************************************************** */
/* integrator
 * ********************************************************** */

#define __dz_sys_i_prev(s) ( ((double *)(s)->prp)[0] )
#define __dz_sys_i_gain(s) ( ((double *)(s)->prp)[1] )
#define __dz_sys_i_fgt(s)  ( ((double *)(s)->prp)[2] )

static void _dzSysIRefresh(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_i_prev(sys) = 0;
}

static zVec _dzSysIUpdate(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = ( 1 - __dz_sys_i_fgt(sys) ) * dzSysOutputVal(sys,0)
    + __dz_sys_i_gain(sys) * __dz_sys_i_prev(sys) * dt;
  __dz_sys_i_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

static void *_dzSysIGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysIFgtFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static bool _dzSysIGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_i_gain((dzSys*)prp) );
  return true;
}
static bool _dzSysIFgtFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_i_fgt((dzSys*)prp) );
  return true;
}

static const ZTKPrp __ztk_prp_dzsys_i[] = {
  { ZTK_KEY_DZCO_SYS_GAIN,             1, _dzSysIGainFromZTK, _dzSysIGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_FORGETTINGFACTOR, 1, _dzSysIFgtFromZTK, _dzSysIFgtFPrintZTK },
};

static dzSys *_dzSysIFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0 };
  if( !_ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_i ) ) return NULL;
  return dzSysICreate( sys, val[0], val[1] );
}

static void _dzSysIFPrintZTK(FILE *fp, dzSys *sys)
{
  _ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_i );
}

dzSysCom dz_sys_i_com = {
  .typestr = "integrator",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysIRefresh,
  ._update = _dzSysIUpdate,
  ._fromZTK = _dzSysIFromZTK,
  ._fprintZTK = _dzSysIFPrintZTK,
};

/* create an integrator. */
dzSys *dzSysICreate(dzSys *sys, double gain, double fgt)
{
  dzSysInit( sys );
  sys->com = &dz_sys_i_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 3 ) ) ) return NULL;
  __dz_sys_i_gain(sys) = gain;
  __dz_sys_i_fgt(sys) = fgt;
  dzSysRefresh( sys );
  return sys;
}

void dzSysISetGain(dzSys *sys, double gain)
{
  __dz_sys_i_gain(sys) = gain;
}

void dzSysISetFgt(dzSys *sys, double fgt)
{
  if( fgt < 0 )
    ZRUNWARN( DZ_ERR_SYS_PID_NEGATIVEFGT, fgt );
  if( fgt > 1 )
    ZRUNWARN( DZ_ERR_SYS_PID_TOOLARGEFGT, fgt );
  __dz_sys_i_fgt(sys) = fgt;
}

/* ********************************************************** */
/* differentiator
 * ********************************************************** */

/* x[k] = T/(dt+T) x[k-1] + K/(dt+T) (u[k]-u[k-1]) */

#define __dz_sys_d_prev(s) ( ((double*)(s)->prp)[0] )
#define __dz_sys_d_gain(s) ( ((double*)(s)->prp)[1] )
#define __dz_sys_d_tc(s)   ( ((double*)(s)->prp)[2] )

static void _dzSysDRefresh(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_d_prev(sys) = 0;
}

static zVec _dzSysDUpdate(dzSys *sys, double dt)
{
  double r;

  r = 1.0 / ( dt + __dz_sys_d_tc(sys) );
  dzSysOutputVal(sys,0) = r * (
    __dz_sys_d_tc(sys) * dzSysOutputVal(sys,0)
    + __dz_sys_d_gain(sys) * ( dzSysInputVal(sys,0) -__dz_sys_d_prev(sys) ) );
  __dz_sys_d_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

static void *_dzSysDGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysDTcFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static bool _dzSysDGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_d_gain((dzSys*)prp) );
  return true;
}
static bool _dzSysDTcFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_d_tc((dzSys*)prp) );
  return true;
}

static const ZTKPrp __ztk_prp_dzsys_d[] = {
  { ZTK_KEY_DZCO_SYS_GAIN,         1, _dzSysDGainFromZTK, _dzSysDGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_TIMECONSTANT, 1, _dzSysDTcFromZTK, _dzSysDTcFPrintZTK },
};

static dzSys *_dzSysDFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0 };
  if( !_ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_d ) ) return NULL;
  return dzSysDCreate( sys, val[0], val[1] );
}

static void _dzSysDFPrintZTK(FILE *fp, dzSys *sys)
{
  _ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_d );
}

dzSysCom dz_sys_d_com = {
  .typestr = "differentiator",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysDRefresh,
  ._update = _dzSysDUpdate,
  ._fromZTK = _dzSysDFromZTK,
  ._fprintZTK = _dzSysDFPrintZTK,
};

/* create a differentiator. */
dzSys *dzSysDCreate(dzSys *sys, double gain, double tc)
{
  dzSysInit( sys );
  sys->com = &dz_sys_d_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 3 ) ) ) return NULL;
  dzSysDSetGain( sys, gain );
  dzSysDSetTC( sys, tc );
  dzSysRefresh( sys );
  return sys;
}

void dzSysDSetGain(dzSys *sys, double gain)
{
  __dz_sys_d_gain(sys) = gain;
}

void dzSysDSetTC(dzSys *sys, double t)
{
  __dz_sys_d_tc(sys) = t;
}

/* ********************************************************** */
/* PID(Proportional, Integral and Differential) controller
 * ********************************************************** */

#define __dz_sys_pid_pgain(s)   ( ((double*)(s)->prp)[0] )
#define __dz_sys_pid_intg(s)    ( ((double*)(s)->prp)[1] )
#define __dz_sys_pid_prev(s)    ( ((double*)(s)->prp)[2] )
#define __dz_sys_pid_fgt(s)     ( ((double*)(s)->prp)[3] )
#define __dz_sys_pid_igain(s)   ( ((double*)(s)->prp)[4] )
#define __dz_sys_pid_dgain(s)   ( ((double*)(s)->prp)[5] )
#define __dz_sys_pid_tc(s)      ( ((double*)(s)->prp)[6] )

static void _dzSysPIDRefresh(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_pid_intg(sys) = __dz_sys_pid_prev(sys) = 0;
}

static zVec _dzSysPIDUpdate(dzSys *sys, double dt)
{
  __dz_sys_pid_intg(sys) = ( 1 - __dz_sys_pid_fgt(sys) ) * __dz_sys_pid_intg(sys)
    + __dz_sys_pid_igain(sys) * __dz_sys_pid_prev(sys) * dt;
  dzSysOutputVal(sys,0) =
    __dz_sys_pid_pgain(sys) * dzSysInputVal(sys,0)
    + __dz_sys_pid_intg(sys)
    + ( __dz_sys_pid_tc(sys) * dzSysOutputVal(sys,0)
      + __dz_sys_pid_dgain(sys) * ( dzSysInputVal(sys,0) - __dz_sys_pid_prev(sys) ) )
    / ( dt + __dz_sys_pid_tc(sys) );
  __dz_sys_pid_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

static void *_dzSysPIDPGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysPIDIGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysPIDDGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysPIDTcFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[3] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysPIDFgtFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[4] = ZTKDouble(ztk);
  return val;
}

static bool _dzSysPIDPGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_pgain((dzSys*)prp) );
  return true;
}
static bool _dzSysPIDIGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_igain((dzSys*)prp) );
  return true;
}
static bool _dzSysPIDDGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_dgain((dzSys*)prp) );
  return true;
}
static bool _dzSysPIDTcFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_tc((dzSys*)prp) );
  return true;
}
static bool _dzSysPIDFgtFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_fgt((dzSys*)prp) );
  return true;
}

static const ZTKPrp __ztk_prp_dzsys_pid[] = {
  { ZTK_KEY_DZCO_SYS_PGAIN,            1, _dzSysPIDPGainFromZTK, _dzSysPIDPGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_IGAIN,            1, _dzSysPIDIGainFromZTK, _dzSysPIDIGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_DGAIN,            1, _dzSysPIDDGainFromZTK, _dzSysPIDDGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_TIMECONSTANT,     1, _dzSysPIDTcFromZTK, _dzSysPIDTcFPrintZTK },
  { ZTK_KEY_DZCO_SYS_FORGETTINGFACTOR, 1, _dzSysPIDFgtFromZTK, _dzSysPIDFgtFPrintZTK },
};

static dzSys *_dzSysPIDFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
  if( !_ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_pid ) ) return NULL;
  return dzSysPIDCreate( sys, val[0], val[1], val[2], val[3], val[4] );
}

static void _dzSysPIDFPrintZTK(FILE *fp, dzSys *sys)
{
  _ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_pid );
}

dzSysCom dz_sys_pid_com = {
  .typestr = "PID",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysPIDRefresh,
  ._update = _dzSysPIDUpdate,
  ._fromZTK = _dzSysPIDFromZTK,
  ._fprintZTK = _dzSysPIDFPrintZTK,
};

/* create a PID controller. */
dzSys *dzSysPIDCreate(dzSys *sys, double kp, double ki, double kd, double tc, double fgt)
{
  dzSysInit( sys );
  sys->com = &dz_sys_pid_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 7 ) ) ) return NULL;
  __dz_sys_pid_pgain(sys) = kp;
  __dz_sys_pid_igain(sys) = ki;
  __dz_sys_pid_dgain(sys) = kd;
  __dz_sys_pid_tc(sys) = tc;
  dzSysPIDSetFgt( sys, fgt );
  dzSysRefresh( sys );
  return sys;
}

void dzSysPIDSetPGain(dzSys *sys, double kp)
{
  __dz_sys_pid_pgain(sys) = kp;
}

void dzSysPIDSetIGain(dzSys *sys, double ki)
{
  __dz_sys_pid_igain(sys) = ki;
}

void dzSysPIDSetDGain(dzSys *sys, double kd)
{
  __dz_sys_pid_dgain(sys) = kd;
}

void dzSysPIDSetTC(dzSys *sys, double tc)
{
  __dz_sys_pid_tc(sys) = tc;
}

void dzSysPIDSetFgt(dzSys *sys, double fgt)
{
  if( fgt < 0 )
    ZRUNWARN( DZ_ERR_SYS_PID_NEGATIVEFGT, fgt );
  if( fgt > 1 )
    ZRUNWARN( DZ_ERR_SYS_PID_TOOLARGEFGT, fgt );
  __dz_sys_pid_fgt(sys) = fgt;
}

/* ********************************************************** */
/* QPD (Quadratic Proportional and Differential) controller
 * ********************************************************** */

#define __dz_sys_qpd_kq1(s)    ((double *)(s)->prp)[0]
#define __dz_sys_qpd_kq2(s)    ((double *)(s)->prp)[1]
#define __dz_sys_qpd_goal(s)   ((double *)(s)->prp)[2]
#define __dz_sys_qpd_init(s)   ((double *)(s)->prp)[3]
#define __dz_sys_qpd_pgain(s)  ((double *)(s)->prp)[4]
#define __dz_sys_qpd_dgain(s)  ((double *)(s)->prp)[5]
#define __dz_sys_qpd_eps(s)    ((double *)(s)->prp)[6]
#define __dz_sys_qpd_prev(s)   ((double *)(s)->prp)[7]

void dzSysQPDSetGoal(dzSys *sys, double goal)
{
  __dz_sys_qpd_goal(sys) = goal;
  __dz_sys_qpd_init(sys) = goal - dzSysInputVal(sys,0);
}

static void _dzSysQPDRefresh(dzSys *sys)
{
  __dz_sys_qpd_prev(sys) = dzSysInputVal(sys,0);
}

static zVec _dzSysQPDUpdate(dzSys *sys, double dt)
{
  double r;

  r = __dz_sys_qpd_goal(sys) - dzSysInputVal(sys,0);
  if( zIsTiny( __dz_sys_qpd_init(sys) ) ){
    dzSysOutputVal(sys,0) = 0;
  } else{
    dzSysOutputVal(sys,0) =
      __dz_sys_qpd_kq1(sys) * r * ( __dz_sys_qpd_kq2(sys) - 1.5*r/__dz_sys_qpd_init(sys) )
      - __dz_sys_qpd_dgain(sys) * ( dzSysInputVal(sys,0) - __dz_sys_qpd_prev(sys) ) / dt;
    __dz_sys_qpd_prev(sys) = dzSysInputVal(sys,0);
  }
  return dzSysOutput(sys);
}

static void *_dzSysQPDPGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysQPDDGainFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysQPDEpsFromZTK(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}

static bool _dzSysQPDPGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_qpd_pgain((dzSys*)prp) );
  return true;
}
static bool _dzSysQPDDGainFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_qpd_dgain((dzSys*)prp) );
  return true;
}
static bool _dzSysQPDEpsFPrintZTK(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_qpd_eps((dzSys*)prp) );
  return true;
}

static const ZTKPrp __ztk_prp_dzsys_qpd[] = {
  { ZTK_KEY_DZCO_SYS_PGAIN,   1, _dzSysQPDPGainFromZTK, _dzSysQPDPGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_DGAIN,   1, _dzSysQPDDGainFromZTK, _dzSysQPDDGainFPrintZTK },
  { ZTK_KEY_DZCO_SYS_QPD_EPS, 1, _dzSysQPDEpsFromZTK, _dzSysQPDEpsFPrintZTK },
};

static dzSys *_dzSysQPDFromZTK(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0, 1.0e-3 };
  if( !_ZTKEvalKey( val, NULL, ztk, __ztk_prp_dzsys_qpd ) ) return NULL;
  return dzSysQPDCreate( sys, val[0], val[1], val[2] );
}

static void _dzSysQPDFPrintZTK(FILE *fp, dzSys *sys)
{
  _ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_qpd );
}

dzSysCom dz_sys_qpd_com = {
  .typestr = "QPD",
  ._destroy = dzSysDefaultDestroy,
  ._refresh = _dzSysQPDRefresh,
  ._update = _dzSysQPDUpdate,
  ._fromZTK = _dzSysQPDFromZTK,
  ._fprintZTK = _dzSysQPDFPrintZTK,
};

/* create a QPD controller. */
dzSys *dzSysQPDCreate(dzSys *sys, double kp, double kd, double eps)
{
  dzSysInit( sys );
  sys->com = &dz_sys_qpd_com;
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) != 1 ||
      !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 8 ) ) ) return NULL;
  __dz_sys_qpd_kq1(sys) = 2 * ( 1 - eps ) * kp;
  __dz_sys_qpd_kq2(sys) = 0.5 * ( 3 - 2 * eps ) / ( 1 - eps );
  __dz_sys_qpd_goal(sys) = 0.0;
  __dz_sys_qpd_pgain(sys) = kp;
  __dz_sys_qpd_dgain(sys) = kd;
  __dz_sys_qpd_eps(sys) = eps;
  dzSysRefresh( sys );
  return sys;
}
