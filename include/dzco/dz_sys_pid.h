/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_pid - PID controller
 */

#ifndef __DZ_SYS_PID_H__
#define __DZ_SYS_PID_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/* ********************************************************** */
/* amplifier
 * ********************************************************** */

/* value map: [gain] */

/*! \brief create proportional amplifier.
 *
 * dzSysPCreate() creates a proportional amplifier \a sys.
 * \a gain is the proportional gain.
 *
 * \return
 * dzSysPCreate() returns the null pointer if it fails to allocate
 * the internal working memory. Otherwise, a pointer \a sys is
 * returned.
 */
__EXPORT dzSys *dzSysPCreate(dzSys *sys, double gain);

__EXPORT void dzSysPSetGain(dzSys *sys, double gain);

extern dzSysCom dz_sys_p_com;

/* ********************************************************** */
/* integrator
 * ********************************************************** */

/* value map: [prev][gain][fgt] */

/*! \brief create an integrator.
 *
 * dzSysICreate() creates an integrator \a sys.
 * \a dt is the sampling time for descrete integration.
 * \a gain is the integral gain.
 * \return
 * dzSysICreate() returns the null pointer if \a dt is an invalid
 * value - too short or negative, or if it fails to allocate the
 * internal working memory. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysICreate(dzSys *sys, double gain, double fgt);

__EXPORT void dzSysISetGain(dzSys *sys, double gain);
__EXPORT void dzSysISetFgt(dzSys *sys, double fgt);

extern dzSysCom dz_sys_i_com;

/* ********************************************************** */
/* differentiator
 * ********************************************************** */

/* value map: [prev][gain][tc] */

/*! \brief create a differentiator.
 *
 * dzSysDCreate() creates a differentiator \a sys.
 * \a dt is the sampling time for descrete differentiation.
 * \a gain is the differential gain.
 * \a t is the time constant to filter the signal. A smaller
 * \a t provides fine outputs, while it tends to suffer from
 * signal noise. The feasible, i.e. mathematically valid,
 * smallest amount for \a t is \a dt. A smaller \a t than \a dt,
 * 0 for example, is automatically replaced for \a dt.
 * \return
 * dzSysDCreate() returns the null pointer if \a dt is an invalid
 * value - too short or negative, or if it fails to allocate
 * the internal working memory. Otherwise, a pointer \a sys is
 * returned.
 */
__EXPORT dzSys *dzSysDCreate(dzSys *sys, double gain, double tc);

__EXPORT void dzSysDSetGain(dzSys *sys, double gain);
__EXPORT void dzSysDSetTC(dzSys *sys, double t);

extern dzSysCom dz_sys_d_com;

/* ********************************************************** */
/* PID (Proportional, Integral and Differential) controller
 * ********************************************************** */

/* value map: [pgain][intg][prev][fgt][igain][dgain][tc] */

/*! \brief create PID controller.
 *
 * dzSysPIDCreate() creates a PID controller \a sys.
 * \a dt is the sampling time for descrete control.
 * \a p is the proportional gain.
 * \a i is the integral gain.
 * \a d is the differential gain.
 * \return
 * dzSysPIDCreate() returns the null pointer if \a dt is a too
 * short or negative value, or if it fails to allocate the
 * internal work space. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysPIDCreate(dzSys *sys, double kp, double ki, double kd, double tc, double fgt);

__EXPORT void dzSysPIDSetPGain(dzSys *sys, double kp);
__EXPORT void dzSysPIDSetIGain(dzSys *sys, double ki);
__EXPORT void dzSysPIDSetDGain(dzSys *sys, double kd);
__EXPORT void dzSysPIDSetTC(dzSys *sys, double tc);
__EXPORT void dzSysPIDSetFgt(dzSys *sys, double fgt);

extern dzSysCom dz_sys_pid_com;

/* ********************************************************** */
/* QPD (Quadratic Proportional and Differential) controller
 * ********************************************************** */

/* value map: [kq1=2kp(1-eps)][kq2=(3-2eps)/2(1-eps)][goal][init][dgain][prev] */

/*! \brief create QPD controller.
 *
 * dzSysQPDCreate() creates a QPD controller \a sys.
 * \a dt is the sampling time for descrete control.
 * \a p is the quadratic proportional gain.
 * \a d is the differential gain.
 * \return
 * dzSysQPDCreate() returns the null pointer if \a dt is a too
 * short or negative value, or if it fails to allocate the internal
 * work space. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysQPDCreate(dzSys *sys, double kp, double kd, double eps);

__EXPORT void dzSysQPDSetGoal(dzSys *sys, double goal);

extern dzSysCom dz_sys_qpd_com;

__END_DECLS

#endif /* __DZ_SYS_PID_H__ */
