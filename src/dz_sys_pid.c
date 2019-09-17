/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_pid - PID controller
 */

#include <dzco/dz_sys.h>

/* ********************************************************** */
/* proportional amplifier
 * ********************************************************** */

static bool _dzSysFScanP(FILE *fp, void *gain, char *buf, bool *success);

#define __dz_sys_p_gain(s) ( *(double *)(s)->prp )

zVec dzSysUpdateP(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = __dz_sys_p_gain(sys) * dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

bool _dzSysFScanP(FILE *fp, void *gain, char *buf, bool *success)
{
  if( strcmp( buf, "gain" ) == 0 ){
    *(double *)gain = zFDouble( fp );
    return true;
  }
  return false;
}

dzSys *dzSysFScanP(FILE *fp, dzSys *sys)
{
  double gain = 0;

  zFieldFScan( fp, _dzSysFScanP, &gain );
  return dzSysCreateP( sys, gain );
}

static void *_dzSysFromZTKPGain(void *val, int i, void *arg, ZTK *ztk){
  *((double*)val) = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintPGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_p_gain((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_p[] = {
  { "gain", 1, _dzSysFromZTKPGain, _dzSysFPrintPGain },
};

static bool _dzSysRegZTKP(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_p ) ? true : false;
}

static dzSys *_dzSysFromZTKP(dzSys *sys, ZTK *ztk)
{
  double gain = 0;
  if( !ZTKEncodeKey( &gain, NULL, ztk, __ztk_prp_dzsys_p ) ) return NULL;
  return dzSysCreateP( sys, gain );
}

static void _dzSysFPrintP(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_p );
}

dzSysCom dz_sys_p_com = {
  typestr: "amplifier",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateP,
  fscan: dzSysFScanP,
  regZTK: _dzSysRegZTKP,
  fromZTK: _dzSysFromZTKP,
  fprint: _dzSysFPrintP,
};

/* create a proportional amplifier. */
dzSys *dzSysCreateP(dzSys *sys, double gain)
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

static bool _dzSysFScanI(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshI(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_i_prev(sys) = 0;
}

zVec dzSysUpdateI(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) = ( 1 - __dz_sys_i_fgt(sys) ) * dzSysOutputVal(sys,0)
    + __dz_sys_i_gain(sys) * __dz_sys_i_prev(sys) * dt;
  __dz_sys_i_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

bool _dzSysFScanI(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "gain" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "fgt" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanI(FILE *fp, dzSys *sys)
{
  double val[] = { 0.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanI, val );
  return dzSysCreateI( sys, val[0], val[1] );
}

static void *_dzSysFromZTKIGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKIFgt(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintIGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_i_gain((dzSys*)prp) );
}
static void _dzSysFPrintIFgt(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_i_fgt((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_i[] = {
  { "gain", 1, _dzSysFromZTKIGain, _dzSysFPrintIGain },
  { "fgt", 1, _dzSysFromZTKIFgt, _dzSysFPrintIFgt },
};

static bool _dzSysRegZTKI(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_i ) ? true : false;
}

static dzSys *_dzSysFromZTKI(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_i ) ) return NULL;
  return dzSysCreateI( sys, val[0], val[1] );
}

static void _dzSysFPrintI(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_i );
}

dzSysCom dz_sys_i_com = {
  typestr: "integrator",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshI,
  update: dzSysUpdateI,
  fscan: dzSysFScanI,
  regZTK: _dzSysRegZTKI,
  fromZTK: _dzSysFromZTKI,
  fprint: _dzSysFPrintI,
};

/* create an integrator. */
dzSys *dzSysCreateI(dzSys *sys, double gain, double fgt)
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

static bool _dzSysFScanD(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshD(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_d_prev(sys) = 0;
}

zVec dzSysUpdateD(dzSys *sys, double dt)
{
  double r;

  r = 1.0 / ( dt + __dz_sys_d_tc(sys) );
  dzSysOutputVal(sys,0) = r * (
    __dz_sys_d_tc(sys) * dzSysOutputVal(sys,0)
    + __dz_sys_d_gain(sys) * ( dzSysInputVal(sys,0) -__dz_sys_d_prev(sys) ) );
  __dz_sys_d_prev(sys) = dzSysInputVal(sys,0);
  return dzSysOutput(sys);
}

bool _dzSysFScanD(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "gain" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "tc" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanD(FILE *fp, dzSys *sys)
{
  double val[] = { 0.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanD, val );
  return dzSysCreateD( sys, val[0], val[1] );
}

static void *_dzSysFromZTKDGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKDTc(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintDGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_d_gain((dzSys*)prp) );
}
static void _dzSysFPrintDTc(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_d_tc((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_d[] = {
  { "gain", 1, _dzSysFromZTKDGain, _dzSysFPrintDGain },
  { "tc", 1, _dzSysFromZTKDTc, _dzSysFPrintDTc },
};

static bool _dzSysRegZTKD(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_d ) ? true : false;
}

static dzSys *_dzSysFromZTKD(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_d ) ) return NULL;
  return dzSysCreateD( sys, val[0], val[1] );
}

static void _dzSysFPrintD(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_d );
}

dzSysCom dz_sys_d_com = {
  typestr: "differentiator",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshD,
  update: dzSysUpdateD,
  fscan: dzSysFScanD,
  regZTK: _dzSysRegZTKD,
  fromZTK: _dzSysFromZTKD,
  fprint: _dzSysFPrintD,
};

/* create a differentiator. */
dzSys *dzSysCreateD(dzSys *sys, double gain, double tc)
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

static bool _dzSysFScanPID(FILE *fp, void *val, char *buf, bool *success);

void dzSysRefreshPID(dzSys *sys)
{
  dzSysOutputVal(sys,0) = __dz_sys_pid_intg(sys) = __dz_sys_pid_prev(sys) = 0;
}

zVec dzSysUpdatePID(dzSys *sys, double dt)
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

bool _dzSysFScanPID(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "pgain" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "igain" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
  if( strcmp( buf, "dgain" ) == 0 ){
    ((double *)val)[2] = zFDouble( fp );
  } else
  if( strcmp( buf, "tc" ) == 0 ){
    ((double *)val)[3] = zFDouble( fp );
  } else
  if( strcmp( buf, "fgt" ) == 0 ){
    ((double *)val)[4] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanPID(FILE *fp, dzSys *sys)
{
  double val[] = { 0.0, 0.0, 0.0, 0.0, 0.0 };

  zFieldFScan( fp, _dzSysFScanPID, val );
  return dzSysCreatePID( sys, val[0], val[1], val[2], val[3], val[4] );
}

static void *_dzSysFromZTKPIDPGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKPIDIGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKPIDDGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKPIDTc(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[3] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKPIDFgt(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[4] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintPIDPGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_pgain((dzSys*)prp) );
}
static void _dzSysFPrintPIDIGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_igain((dzSys*)prp) );
}
static void _dzSysFPrintPIDDGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_dgain((dzSys*)prp) );
}
static void _dzSysFPrintPIDTc(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_tc((dzSys*)prp) );
}
static void _dzSysFPrintPIDFgt(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_pid_fgt((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_pid[] = {
  { "pgain", 1, _dzSysFromZTKPIDPGain, _dzSysFPrintPIDPGain },
  { "igain", 1, _dzSysFromZTKPIDIGain, _dzSysFPrintPIDIGain },
  { "dgain", 1, _dzSysFromZTKPIDDGain, _dzSysFPrintPIDDGain },
  { "tc", 1, _dzSysFromZTKPIDTc, _dzSysFPrintPIDTc },
  { "fgt", 1, _dzSysFromZTKPIDFgt, _dzSysFPrintPIDFgt },
};

static bool _dzSysRegZTKPID(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_pid ) ? true : false;
}

static dzSys *_dzSysFromZTKPID(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_pid ) ) return NULL;
  return dzSysCreatePID( sys, val[0], val[1], val[2], val[3], val[4] );
}

static void _dzSysFPrintPID(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_pid );
}

dzSysCom dz_sys_pid_com = {
  typestr: "PID",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshPID,
  update: dzSysUpdatePID,
  fscan: dzSysFScanPID,
  regZTK: _dzSysRegZTKPID,
  fromZTK: _dzSysFromZTKPID,
  fprint: _dzSysFPrintPID,
};

/* create a PID controller. */
dzSys *dzSysCreatePID(dzSys *sys, double kp, double ki, double kd, double tc, double fgt)
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
/* QPD(Quadratic Proportional and Differential) controller
 * ********************************************************** */

#define __dz_sys_qpd_kq1(s)    ((double *)(s)->prp)[0]
#define __dz_sys_qpd_kq2(s)    ((double *)(s)->prp)[1]
#define __dz_sys_qpd_goal(s)   ((double *)(s)->prp)[2]
#define __dz_sys_qpd_init(s)   ((double *)(s)->prp)[3]
#define __dz_sys_qpd_pgain(s)  ((double *)(s)->prp)[4]
#define __dz_sys_qpd_dgain(s)  ((double *)(s)->prp)[5]
#define __dz_sys_qpd_eps(s)    ((double *)(s)->prp)[6]
#define __dz_sys_qpd_prev(s)   ((double *)(s)->prp)[7]

static bool _dzSysFScanQPD(FILE *fp, void *val, char *buf, bool *success);

void dzSysQPDSetGoal(dzSys *sys, double goal)
{
  __dz_sys_qpd_goal(sys) = goal;
  __dz_sys_qpd_init(sys) = goal - dzSysInputVal(sys,0);
}

void dzSysRefreshQPD(dzSys *sys)
{
  __dz_sys_qpd_prev(sys) = dzSysInputVal(sys,0);
}

zVec dzSysUpdateQPD(dzSys *sys, double dt)
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

bool _dzSysFScanQPD(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "pgain" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "dgain" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
  if( strcmp( buf, "eps" ) == 0 ){
    ((double *)val)[2] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFScanQPD(FILE *fp, dzSys *sys)
{
  double val[] = { 0.0, 0.0, 1.0e-3 };

  zFieldFScan( fp, _dzSysFScanQPD, val );
  return dzSysCreateQPD( sys, val[0], val[1], val[2] );
}

static void *_dzSysFromZTKQPDPGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[0] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKQPDDGain(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[1] = ZTKDouble(ztk);
  return val;
}
static void *_dzSysFromZTKQPDEps(void *val, int i, void *arg, ZTK *ztk){
  ((double*)val)[2] = ZTKDouble(ztk);
  return val;
}

static void _dzSysFPrintQPDPGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_qpd_pgain((dzSys*)prp) );
}
static void _dzSysFPrintQPDDGain(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_qpd_dgain((dzSys*)prp) );
}
static void _dzSysFPrintQPDEps(FILE *fp, int i, void *prp){
  fprintf( fp, "%.10g\n", __dz_sys_qpd_eps((dzSys*)prp) );
}

static ZTKPrp __ztk_prp_dzsys_qpd[] = {
  { "pgain", 1, _dzSysFromZTKQPDPGain, _dzSysFPrintQPDPGain },
  { "dgain", 1, _dzSysFromZTKQPDDGain, _dzSysFPrintQPDDGain },
  { "eps", 1, _dzSysFromZTKQPDEps, _dzSysFPrintQPDEps },
};

static bool _dzSysRegZTKQPD(ZTK *ztk)
{
  return ZTKDefRegPrp( ztk, ZTK_TAG_DZSYS, __ztk_prp_dzsys_qpd ) ? true : false;
}

static dzSys *_dzSysFromZTKQPD(dzSys *sys, ZTK *ztk)
{
  double val[] = { 0.0, 0.0, 1.0e-3 };
  if( !ZTKEncodeKey( val, NULL, ztk, __ztk_prp_dzsys_qpd ) ) return NULL;
  return dzSysCreateQPD( sys, val[0], val[1], val[2] );
}

static void _dzSysFPrintQPD(FILE *fp, dzSys *sys)
{
  ZTKPrpKeyFPrint( fp, sys, __ztk_prp_dzsys_qpd );
}

dzSysCom dz_sys_qpd_com = {
  typestr: "QPD",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshQPD,
  update: dzSysUpdateQPD,
  fscan: dzSysFScanQPD,
  regZTK: _dzSysRegZTKQPD,
  fromZTK: _dzSysFromZTKQPD,
  fprint: _dzSysFPrintQPD,
};

/* create a QPD controller. */
dzSys *dzSysCreateQPD(dzSys *sys, double kp, double kd, double eps)
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
