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
 * dzSysCreateLin() creates a linear system \a c.
 * \a lin is the linear system parameters assigned to \a c,
 * which should be created by any means in advance.
 * \a dt is a sampling time.
 *
 * dzSysCreateLin() assigns \a lin to \a c, and not newly
 * allocate particular work space for it.
 * Memory for \a lin has to be independently managed.
 * \return
 * dzSysCreateLin() returns the false value if \a dt is too
 * short or negative. Otherwise, the true value is returned.
 */
__EXPORT bool dzSysCreateLin(dzSys *sys, dzLin *lin);

extern dzSysCom dz_sys_lin_com;

__END_DECLS

#endif /* __DZ_SYS_LIN_H__ */
