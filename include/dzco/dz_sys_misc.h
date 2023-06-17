/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_misc - miscellanies
 */

#ifndef __DZ_SYS_MISC_H__
#define __DZ_SYS_MISC_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/* ********************************************************** */
/* value confluenter
 * ********************************************************** */

/*! \brief create and connect inputs of confluenter.
 *
 * dzSysAdderCreate() and dzSysSubtrCreate() create an adder and
 * a subtractor, respectively.
 * The system created is stored into \a sys.
 * \retval
 * dzSysAdderCreate() and dzSysSubtrCreate() return the null poiter
 * if they fail to allocate internal memory. Otherwise, a pointer
 * \a sys is returned.
 */
__DZCO_EXPORT dzSys *dzSysAdderCreate(dzSys *sys, int n);

__DZCO_EXPORT dzSysCom dz_sys_adder_com;

__DZCO_EXPORT dzSys *dzSysSubtrCreate(dzSys *sys, int n);

__DZCO_EXPORT dzSysCom dz_sys_subtr_com;

/* ********************************************************** */
/* saturater
 * ********************************************************** */

/*! \brief create saturater.
 *
 * dzSysLimitCreate() creates a saturater \a sys. \a min and \a max
 * are the minimum and maximum borders, respectively. The output
 * of \a sys is saturated by \a min and \a max.
 * \retval
 * dzSysLimitCreate() returns the null pointer if it fails to allocate
 * the internal work space. Otherwise, a pointer \a sys is returned.
 * \notes
 * When \a max is less than \a min, the border is automatically
 * corrected by swapping the two values.
 */
__DZCO_EXPORT dzSys *dzSysLimitCreate(dzSys *sys, double min, double max);

__DZCO_EXPORT dzSysCom dz_sys_limit_com;

__END_DECLS

#endif /* __DZ_SYS_MISC_H__ */
