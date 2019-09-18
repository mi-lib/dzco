/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_tf - transfer function by polynomial rational expression
 */

#ifndef __DZ_SYS_TF_H__
#define __DZ_SYS_TF_H__

/* NOTE: never include this header file in user programs. */

#include <dzco/dz_tf.h>

__BEGIN_DECLS

/*! \brief create a transfer function by polynomial rational expression.
 *
 * dzSysTFCreate() creates a transfer function \a sys defined
 * by a polynomial rational expression \a tf as an infinite
 * impulse response system based on zero-order hold.
 * \a dt is a sampling time.
 *
 * It is mathematically equivalent to dzTF2LinCtrlCanon() but
 * is memory-saving.
 * \return
 * dzSysTFCreate() returns the null pointer if it fails to
 * allocate internal working memory, or \a dt is too short or
 * negative. Otherwise, a pointer \a sys is returned.
 */
__EXPORT dzSys *dzSysTFCreate(dzSys *sys, dzTF *tf);

extern dzSysCom dz_sys_tf_com;

__END_DECLS

#endif /* __DZ_SYS_TF_H__ */
