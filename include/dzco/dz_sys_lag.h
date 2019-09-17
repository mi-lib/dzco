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
 * dzSysCreateFOL() creates a first-order-lag system \a sys.
 * \a dt is the sampling time for descrete integration.
 * \a t is the time constant of the system.
 * \a gain is the gain.
 * \return
 * dzSysCreateFOL() returns a pointer \a sys if \a dt is a
 * too short or negative value, or it fails to allocate the
 * internal work space. Otherwise, the true value is returned.
 */
__EXPORT dzSys *dzSysCreateFOL(dzSys *sys, double tc, double gain);

__EXPORT void dzSysFOLSetTC(dzSys *sys, double tc);
__EXPORT void dzSysFOLSetGain(dzSys *sys, double gain);

extern dzSysCom dz_sys_fol_com;

/* ********************************************************** */
/* second-order-lag system
 * ********************************************************** */

/* value map: [t1][t2][zeta][gain][prevout][previn][t1/dt] */

/*! \brief create second-order-lag system.
 *
 * dzSysCreateSOL() creates a second-order-lag system \a sys.
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
 * dzSysCreateSOLGen() is another constructor of a second-order-lag
 * system \a sys. \a a, \a b, \a c, \a d and \a e defines the
 * system by the following transfer function.
 *
 *        \a d s + \a e
 *   ---------------------
 *   \a a s^2 + \a b s + \a c
 *
 * It is preferable when the coefficients are known
 * rather than the characteristic parameters.
 * \return
 * dzSysCreateSOL() and dzSysCreateSOL_Gen() return the null
 * pointer if \a dt is a too short or negative value, or they
 * fail to allocate the internal work space. Otherwise, a
 * pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysCreateSOL(dzSys *sys, double t1, double t2, double damp, double gain);
__EXPORT dzSys *dzSysCreateSOLGen(dzSys *sys, double a, double b, double c, double d, double e);

extern dzSysCom dz_sys_sol_com;

/* ********************************************************** */
/* phase compensator system
 * ********************************************************** */

/* value map: [prev][t1][t2][gain] */

/*! \brief create phase compensator.
 *
 * dzSysCreatePC() creates a phase compensator \a sys.
 * \a dt is the sampling time for descrete integration.
 * The other parameters consist of the following transfer function.
 *                \a t2 s + 1
 *  G(s) = \a gain ----------
 *                \a t1 s + 1
 * When \a t2 < \a t1, \a sys is a phase-lag compensator.
 * When \a t2 > \a t1, \a sys is a phase-lead compensator.
 * \return
 * dzSysCreatePC() returns the null pointer if \a dt is a too
 * short or a negative value, or it fails to allocate the
 * internal work space. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysCreatePC(dzSys *sys, double t1, double t2, double gain);

extern dzSysCom dz_sys_pc_com;

/* ********************************************************** */
/* adaptive system
 * ********************************************************** */

/* value map: [tc][base][offset] */

__EXPORT void dzSysAdaptSetBase(dzSys *sys, double base);

/*!\brief create adaptive system.
 *
 * dzSysCreateAdapt() creates an adaptive system \a sys.
 * \a dt is the sampling time for descrete integration, \a t is
 * the time constant, and \a base is the base value of the output.
 * \retval
 * dzSysCreateAdapt() returns the null pointer if \a dt is too small
 * or negative, or if it fails to allocate the internal work space.
 * Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysCreateAdapt(dzSys *sys, double tc, double base);

extern dzSysCom dz_sys_adapt_com;

__END_DECLS

#endif /* __DZ_SYS_LAG_H__ */
