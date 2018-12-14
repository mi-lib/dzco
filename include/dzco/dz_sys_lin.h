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
 * 'dzSysCreateLin()' creates a linear system 'c'.
 * 'lin' is the linear system parameters assigned to 'c',
 * which should be created by any means in advance.
 * 'dt' is a sampling time.
 * #
 * 'dzSysCreateLin()' just assigns 'lin' to 'c', not
 * newly allocating particular work space for it.
 * Memory for 'lin' has to be independently managed.
 * [RETURN VALUE]
 * 'dzSysCreateLin()' returns the false value if 'dt'
 * is too short or negative. Otherwise, the true value
 * is returned.
 */
__EXPORT bool dzSysCreateLin(dzSys *sys, dzLin *lin);

extern dzSysMethod dz_sys_lin_met;

__END_DECLS

#endif /* __DZ_SYS_LIN_H__ */
