/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_lag - lag system
 */

#ifndef __DZ_SYS_LAG_H__
#define __DZ_SYS_LAG_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/* ********************************************************** */
/* first-order-lag system
 * ********************************************************** */

/* value map: [tc][gain] */

/*!\brief create first-order-lag system.
 *
 * dzSysFOLCreate() creates a first-order-lag system \a sys.
 * \a dt is the sampling time for descrete integration.
 * \a t is the time constant of the system.
 * \a gain is the gain.
 * \return
 * dzSysFOLCreate() returns a pointer \a sys if \a dt is a
 * too short or negative value, or it fails to allocate the
 * internal work space. Otherwise, the true value is returned.
 */
__EXPORT dzSys *dzSysFOLCreate(dzSys *sys, double tc, double gain);

__EXPORT void dzSysFOLSetTC(dzSys *sys, double tc);
__EXPORT void dzSysFOLSetGain(dzSys *sys, double gain);

__EXPORT dzSysCom dz_sys_fol_com;

/* ********************************************************** */
/* second-order-lag system
 * ********************************************************** */

/* value map: [t1][t2][zeta][gain][prevout][previn][t1/dt] */

/*! \brief create second-order-lag system.
 *
 * dzSysSOLCreate() creates a second-order-lag system \a sys.
 * \a dt is the sampling time for descrete integration.
 * \a damp is the damping coefficient.
 * \a t1 is the dominant time constant.
 * \a t2 is the subordinate time constant.
 * \a gain is the gain.
 * As the result, a system represented by the following
 * transfer function is created.
 *
 *        \a gain*( \a t2*s + 1 )
 *   --------------------------------
 *   (\a t1 s)^2 + 2*\a damp*\a t1 s + 1
 *
 * dzSysSOLCreateGeneric() is another constructor of a second-order-lag
 * system \a sys. \a a, \a b, \a c, \a d and \a e defines the
 * system by the following transfer function.
 *
 *        \a d s + \a e
 *   ---------------------
 *   \a a s^2 + \a b s + \a c
 *
 * It suits a case where the coefficients are known
 * rather than the characteristic parameters.
 * \return
 * dzSysSOLCreate() and dzSysSOLCreateGeneric() return the null
 * pointer if \a dt is a too short or negative value, or they
 * fail to allocate the internal work space. Otherwise, a
 * pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysSOLCreate(dzSys *sys, double t1, double t2, double damp, double gain);
__EXPORT dzSys *dzSysSOLCreateGeneric(dzSys *sys, double a, double b, double c, double d, double e);

__EXPORT dzSysCom dz_sys_sol_com;

/* ********************************************************** */
/* phase compensator system
 * ********************************************************** */

/* value map: [prev][t1][t2][gain] */

/*! \brief create phase compensator.
 *
 * dzSysPCCreate() creates a phase compensator \a sys.
 * \a dt is the sampling time for descrete integration.
 * The other parameters consist of the following transfer function.
 *                \a t2 s + 1
 *  G(s) = \a gain ----------
 *                \a t1 s + 1
 * When \a t2 < \a t1, \a sys is a phase-lag compensator.
 * When \a t2 > \a t1, \a sys is a phase-lead compensator.
 * \return
 * dzSysPCCreate() returns the null pointer if \a dt is a too
 * short or a negative value, or it fails to allocate the
 * internal work space. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysPCCreate(dzSys *sys, double t1, double t2, double gain);

__EXPORT dzSysCom dz_sys_pc_com;

/* ********************************************************** */
/* adaptive system
 * ********************************************************** */

/* value map: [tc][base][offset] */

__EXPORT void dzSysAdaptSetBase(dzSys *sys, double base);

/*!\brief create adaptive system.
 *
 * dzSysAdaptCreate() creates an adaptive system \a sys.
 * \a dt is the sampling time for descrete integration, \a t is
 * the time constant, and \a base is the base value of the output.
 * \retval
 * dzSysAdaptCreate() returns the null pointer if \a dt is too small
 * or negative, or if it fails to allocate the internal work space.
 * Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysAdaptCreate(dzSys *sys, double tc, double base);

__EXPORT dzSysCom dz_sys_adapt_com;

__END_DECLS

#endif /* __DZ_SYS_LAG_H__ */
