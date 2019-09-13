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
 * dzSysCreateAdder() and dzSysCreateSubtr() creates an adder and
 * a subtractor, respectively.
 * The system created is stored into \a c.
 * \retval
 * dzSysCreateAdder() and dzSysCreateSubtr() return the true value
 * without exception.
 */
__EXPORT bool dzSysCreateAdder(dzSys *c, int n);

extern dzSysCom dz_sys_adder_com;

__EXPORT bool dzSysCreateSubtr(dzSys *c, int n);

extern dzSysCom dz_sys_subtr_com;

/* ********************************************************** */
/* saturater
 * ********************************************************** */

/*! \brief create saturater.
 *
 * dzSysCreateLimit() creates a saturater \a c. \a min and \a max
 * are the minimum and maximum borders, respectively. The output
 * of \a c is saturated by \a min and \a max.
 * \retval
 * dzSysCreateLimit() returns the false value if it fails to allocate
 * the internal work space. Otherwise, the true value is returned.
 * \notes
 * When \a max is less than \a min, the border is automatically
 * corrected by swapping the two values.
 */
__EXPORT bool dzSysCreateLimit(dzSys *c, double max, double min);

extern dzSysCom dz_sys_limit_com;

__END_DECLS

#endif /* __DZ_SYS_MISC_H__ */
