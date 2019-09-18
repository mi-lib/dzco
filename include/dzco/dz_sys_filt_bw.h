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
 * dzSysBWCreate() creates a Butterworth filter \a sys
 * with the cut-off frequency \a cf and the dimension \a dim.
 * \a dt is the sampling time.
 * \retval
 * dzSysBWCreate() returns a pointer \a sys if succeeding.
 * Or, it returns the null pointer when failing by any reasons.
 */
__EXPORT dzSys *dzSysBWCreate(dzSys *sys, double cf, uint dim);

extern dzSysCom dz_sys_bw_com;

__END_DECLS

#endif /* __DZ_SYS_FILT_BW_H__ */
