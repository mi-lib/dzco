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
 * 'dzSysCreateFOL()' creates a first-order-lag system 'c'.
 * 'dt' is the sampling time for descrete integration.
 * 't' is the time constant.
 * 'gain' is the gain.
 * [RETURN VALUE]
 * 'dzSysCreateFOL()' returns the false value if 'dt'
 * is a too short or negative value, or it fails to
 * allocate the internal work space. Otherwise, the
 * true value is returned.
 */
__EXPORT bool dzSysCreateFOL(dzSys *c, double tc, double gain);

__EXPORT void dzSysFOLSetTC(dzSys *c, double tc);
__EXPORT void dzSysFOLSetGain(dzSys *c, double gain);

extern dzSysMethod dz_sys_fol_met;

/* ********************************************************** */
/* second-order-lag system
 * ********************************************************** */

/* value map: [t1][t2][zeta][gain][prevout][previn][t1/dt] */

/* METHOD:
 * dzSysCreateSOL, dzSysCreateSOLGen
 * - create second-order-lag system.
 *
 * 'dzSysCreateSOL()' creates a second-order-lag system 'g'.
 * 'dt' is the sampling time for descrete integration.
 * 'damp' is the damping coefficient.
 * 't1' is the dominant time constant.
 * 't2' is the subordinate time constant.
 * 'gain' is the gain.
 * As the result, a system represented by the following
 * transfer function is created.
 *
 *        'gain'*( 't2'*s + 1 )
 *   --------------------------------
 *   ('t1' s)^2 + 2*'damp'*'t1' s + 1
 *
 * 'dzSysCreateSOLGen()' is another constructor of a
 * second-order-lag system 'g'. 'a', 'b', 'c', 'd' and
 * 'e' defines the system by the following transfer
 * function.
 *
 *        'd' s + 'e'
 *   ---------------------
 *   'a' s^2 + 'b' s + 'c'
 *
 * It is preferable when the coefficients are known
 * rather than the characteristic parameters.
 * [RETURN VALUE]
 * 'dzSysCreateSOL()' and 'dzSysCreateSOL_Gen()' return
 * the false value if 'dt' is a too short or negative
 * value, or they fail to allocate the internal work
 * space. Otherwise, the true value is returned.
 */
__EXPORT bool dzSysCreateSOL(dzSys *sys, double t1, double t2, double damp, double gain);
__EXPORT bool dzSysCreateSOLGen(dzSys *sys, double a, double b, double c, double d, double e);

extern dzSysMethod dz_sys_sol_met;

/* ********************************************************** */
/* phase compensator system
 * ********************************************************** */

/* value map: [prev][t1][t2][gain] */

/* METHOD:
 * dzSysCreatePC - create phase compensator.
 *
 * 'dzSysCreatePC()' creates a phase compensator 'c'.
 * 'dt' is the sampling time for descrete integration.
 * The other parameters consist of the following
 * transfer function.
 *                't2's + 1
 *  G(s) = 'gain' ----------
 *                't1's + 1
 * When 't2' < 't1', 'c' is a phase-lag compensator.
 * When 't2' > 't1', 'c' is a phase-lead compensator.
 * [RETURN VALUE]
 * 'dzSysCreatePC()' returns the false value if 'dt'
 * is a too short or negative value, or it fails to
 * allocate the internal work space. Otherwise, the
 * true value is returned.
 */
__EXPORT bool dzSysCreatePC(dzSys *c, double t1, double t2, double gain);

extern dzSysMethod dz_sys_pc_met;

/* ********************************************************** */
/* adaptive system
 * ********************************************************** */

/* value map: [tc][base][offset] */

__EXPORT void dzSysAdaptSetBase(dzSys *sys, double base);

/*!\brief create adaptive system.
 *
 * dzSysCreateAdapt() creates an adaptive system \a c.
 * \a dt is the sampling time for descrete integration, \a t is
 * the time constant, and \a base is the base value of the output.
 * \retval false if \a dt is too small or negative, or if it fails
 * to allocate the internal work space.
 * \retval true otherwise.
 */
__EXPORT bool dzSysCreateAdapt(dzSys *c, double tc, double base);

extern dzSysMethod dz_sys_adapt_met;

__END_DECLS

#endif /* __DZ_SYS_LAG_H__ */
