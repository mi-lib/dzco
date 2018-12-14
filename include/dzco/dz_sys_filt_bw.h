/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_filt_bw - Butterworth filter
 */

#ifndef __DZ_SYS_FILT_BW_H__
#define __DZ_SYS_FILT_BW_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/* ********************************************************** */
/* CLASS: dzBW
 * Butterworth filter
 * ********************************************************** */

/*! \brief create a Butterworth filter.
 *
 * dzSysCreateBW() creates a Butterworth filter \a sys
 * with the cut-off frequency \a cf and the dimension \a dim.
 * \a dt is the sampling time.
 * \retval
 * dzBWCreate() returns the true value, if succeeding.
 * Or, it returns the false value when failing by some
 * reasons.
 */
__EXPORT bool dzSysCreateBW(dzSys *sys, double cf, uint dim);

extern dzSysMethod dz_sys_bw_met;

__END_DECLS

#endif /* __DZ_SYS_FILT_BW_H__ */
