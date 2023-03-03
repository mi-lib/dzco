/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_lin - linear system
 */

#ifndef __DZ_SYS_LIN_H__
#define __DZ_SYS_LIN_H__

/* NOTE: never include this header file in user programs. */

#include <dzco/dz_lin.h>

__BEGIN_DECLS

/* ********************************************************** */
/* linear system
 * ********************************************************** */

/*! \brief create linear system.
 *
 * dzSysLinCreate() creates a linear system \a sys.
 * \a lin is the linear system parameters assigned to \a sys,
 * which should be created by any means in advance.
 * \a dt is a sampling time.
 *
 * dzSysLinCreate() assigns \a lin to \a sys, and not newly
 * allocate particular work space for it.
 * Memory for \a lin has to be independently managed.
 * \return
 * dzSysLinCreate() returns the null pointer if \a dt is too
 * short or negative. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysLinCreate(dzSys *sys, dzLin *lin);

__EXPORT dzSysCom dz_sys_lin_com;

__END_DECLS

#endif /* __DZ_SYS_LIN_H__ */
