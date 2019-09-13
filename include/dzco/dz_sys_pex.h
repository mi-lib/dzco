/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_pex - polynomial transfer function
 */

#ifndef __DZ_SYS_PEX_H__
#define __DZ_SYS_PEX_H__

/* NOTE: never include this header file in user programs. */

#include <dzco/dz_pex.h>

__BEGIN_DECLS

/*! \brief create a polynomial transfer function.
 *
 * dzSysCreatePex() creates a transfer function \a sys defined
 * by a polynomial rational expression \a pex as an infinite
 * impulse response system based on zero-order hold.
 * \a dt is a sampling time.
 *
 * It is mathematically equivalent to dzPex2LinCtrlCanon()
 * but is memory-saving.
 * \return
 * dzSysCreatePex() returns the false value if it fails
 * to allocate internal working memory, or \a dt is too
 * short or negative. Otherwise, the true value is returned.
 */
__EXPORT bool dzSysCreatePex(dzSys *sys, dzPex *pex);

extern dzSysCom dz_sys_pex_com;

__END_DECLS

#endif /* __DZ_SYS_PEX_H__ */
