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

#define __dz_sys_p_gain(s) ( *(double *)(s)->_prm )

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
  return dzSysCreateP( sys, gain ) ? sys : NULL;
}

void dzSysFPrintP(FILE *fp, dzSys *sys)
{
  fprintf( fp, "gain: %g\n", *((double*)sys->_prm) );
}

dzSysMethod dz_sys_p_met = {
  type: "p",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateP,
  fscan: dzSysFScanP,
  fprint: dzSysFPrintP,
};

/* create a proportional amplifier. */
bool dzSysCreateP(dzSys *sys, double gain)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 1 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_p_met;
  dzSysPSetGain( sys, gain );
  return true;
}

void dzSysPSetGain(dzSys *sys, double gain)
{
  __dz_sys_p_gain(sys) = gain;
}

/* ********************************************************** */
/* integrator
 * ********************************************************** */

#define __dz_sys_i_prev(s) ( ((double *)(s)->_prm)[0] )
#define __dz_sys_i_gain(s) ( ((double *)(s)->_prm)[1] )
#define __dz_sys_i_fgt(s)  ( ((double *)(s)->_prm)[2] )

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
  return dzSysCreateI( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFPrintI(FILE *fp, dzSys *sys)
{
  fprintf( fp, "gain: %g\n", __dz_sys_i_gain(sys) );
  fprintf( fp, "fgt: %g\n", __dz_sys_i_fgt(sys) );
}

dzSysMethod dz_sys_i_met = {
  type: "i",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshI,
  update: dzSysUpdateI,
  fscan: dzSysFScanI,
  fprint: dzSysFPrintI,
};

/* create an integrator. */
bool dzSysCreateI(dzSys *sys, double gain, double fgt)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 3 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_i_met;
  __dz_sys_i_gain(sys) = gain;
  __dz_sys_i_fgt(sys) = fgt;
  dzSysRefresh( sys );
  return true;
}

void dzSysISetGain(dzSys *sys, double gain)
{
  __dz_sys_i_gain(sys) = gain;
}

void dzSysISetFgt(dzSys *sys, double fgt)
{
  if( fgt < 0 )
    ZRUNWARN( "negative forgetting factor %f specified", fgt );
  if( fgt > 1 )
    ZRUNWARN( "forgetting factor %f larger than 1 specified", fgt );
  __dz_sys_i_fgt(sys) = fgt;
}

/* ********************************************************** */
/* differentiator
 * ********************************************************** */

/* x[k] = T/(dt+T) x[k-1] + K/(dt+T) (u[k]-u[k-1]) */

#define __dz_sys_d_prev(s) ( ((double*)s->_prm)[0] )
#define __dz_sys_d_gain(s) ( ((double*)s->_prm)[1] )
#define __dz_sys_d_tc(s)   ( ((double*)s->_prm)[2] )

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
  return dzSysCreateD( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFPrintD(FILE *fp, dzSys *sys)
{
  fprintf( fp, "gain: %g\n", __dz_sys_d_gain(sys) );
  fprintf( fp, "tc: %g\n", __dz_sys_d_tc(sys) );
}

dzSysMethod dz_sys_d_met = {
  type: "d",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshD,
  update: dzSysUpdateD,
  fscan: dzSysFScanD,
  fprint: dzSysFPrintD,
};

/* create a differentiator. */
bool dzSysCreateD(dzSys *sys, double gain, double tc)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 3 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_d_met;
  dzSysDSetGain( sys, gain );
  dzSysDSetTC( sys, tc );
  dzSysRefresh( sys );
  return true;
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

#define __dz_sys_pid_pgain(s)   ( ((double*)(s)->_prm)[0] )
#define __dz_sys_pid_intg(s)    ( ((double*)(s)->_prm)[1] )
#define __dz_sys_pid_prev(s)    ( ((double*)(s)->_prm)[2] )
#define __dz_sys_pid_fgt(s)     ( ((double*)(s)->_prm)[3] )
#define __dz_sys_pid_igain(s)   ( ((double*)(s)->_prm)[4] )
#define __dz_sys_pid_dgain(s)   ( ((double*)(s)->_prm)[5] )
#define __dz_sys_pid_tc(s)      ( ((double*)(s)->_prm)[6] )

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
  return dzSysCreatePID( sys, val[0], val[1], val[2], val[3], val[4] ) ? sys : NULL;
}

void dzSysFPrintPID(FILE *fp, dzSys *sys)
{
  fprintf( fp, "pgain: %g\n", __dz_sys_pid_pgain(sys) );
  fprintf( fp, "igain: %g\n", __dz_sys_pid_igain(sys) );
  fprintf( fp, "dgain: %g\n", __dz_sys_pid_dgain(sys) );
  fprintf( fp, "tc: %g\n", __dz_sys_pid_tc(sys) );
  fprintf( fp, "fgt: %g\n", __dz_sys_pid_fgt(sys) );
}

dzSysMethod dz_sys_pid_met = {
  type: "pid",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshPID,
  update: dzSysUpdatePID,
  fscan: dzSysFScanPID,
  fprint: dzSysFPrintPID,
};

/* create a PID controller. */
bool dzSysCreatePID(dzSys *sys, double kp, double ki, double kd, double tc, double fgt)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 7 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_pid_met;
  __dz_sys_pid_pgain(sys) = kp;
  __dz_sys_pid_igain(sys) = ki;
  __dz_sys_pid_dgain(sys) = kd;
  __dz_sys_pid_tc(sys) = tc;
  dzSysPIDSetFgt( sys, fgt );
  dzSysRefresh( sys );
  return true;
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
    ZRUNWARN( "negative forgetting factor %f specified", fgt );
  if( fgt > 1 )
    ZRUNWARN( "forgetting factor %f larger than 1 specified", fgt );
  __dz_sys_pid_fgt(sys) = fgt;
}

/* ********************************************************** */
/* QPD(Quadratic Proportional and Differential) controller
 * ********************************************************** */

#define __dz_sys_qpd_kq1(s)    ((double *)(s)->_prm)[0]
#define __dz_sys_qpd_kq2(s)    ((double *)(s)->_prm)[1]
#define __dz_sys_qpd_goal(s)   ((double *)(s)->_prm)[2]
#define __dz_sys_qpd_init(s)   ((double *)(s)->_prm)[3]
#define __dz_sys_qpd_pgain(s)  ((double *)(s)->_prm)[4]
#define __dz_sys_qpd_dgain(s)  ((double *)(s)->_prm)[5]
#define __dz_sys_qpd_eps(s)    ((double *)(s)->_prm)[6]
#define __dz_sys_qpd_prev(s)   ((double *)(s)->_prm)[7]

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
  return dzSysCreateQPD( sys, val[0], val[1], val[2] ) ? sys : NULL;
}

void dzSysFPrintQPD(FILE *fp, dzSys *sys)
{
  fprintf( fp, "pgain: %g\n", __dz_sys_qpd_pgain(sys) );
  fprintf( fp, "dgain: %g\n", __dz_sys_qpd_dgain(sys) );
  fprintf( fp, "eps: %g\n", __dz_sys_qpd_eps(sys) );
}

dzSysMethod dz_sys_qpd_met = {
  type: "qpd",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshQPD,
  update: dzSysUpdateQPD,
  fscan: dzSysFScanQPD,
  fprint: dzSysFPrintQPD,
};

/* create a QPD controller. */
bool dzSysCreateQPD(dzSys *sys, double kp, double kd, double eps)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 8 ) ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_qpd_met;
  __dz_sys_qpd_kq1(sys) = 2 * ( 1 - eps ) * kp;
  __dz_sys_qpd_kq2(sys) = 0.5 * ( 3 - 2 * eps ) / ( 1 - eps );
  __dz_sys_qpd_goal(sys) = 0.0;
  __dz_sys_qpd_pgain(sys) = kp;
  __dz_sys_qpd_dgain(sys) = kd;
  __dz_sys_qpd_eps(sys) = eps;
  dzSysRefresh( sys );
  return true;
}
