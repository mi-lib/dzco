/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys - system class
 */

#ifndef __DZ_SYS_H__
#define __DZ_SYS_H__

#include <zm/zm.h>

#include <dzco/dz_errmsg.h>

__BEGIN_DECLS

/* ********************************************************** */
/* \class dzSysPort and dzSysPortArray
 * ********************************************************** */

struct _dzSys;

typedef struct{
  struct _dzSys *sp;
  int port;
  double *vp;
} dzSysPort;

zArrayClass( dzSysPortArray, dzSysPort );

/* ********************************************************** */
/* \class dzSys
 * ********************************************************** */

typedef struct{
  const char *typestr;
  void (* _destroy)(struct _dzSys*);
  void (* _refresh)(struct _dzSys*);
  zVec (* _update)(struct _dzSys*, double);
  bool (* _regZTK)(ZTK*);
  struct _dzSys *(* _fromZTK)(struct _dzSys*, ZTK*);
  void (* _fprintZTK)(FILE *fp, struct _dzSys*);
} dzSysCom;

typedef struct _dzSys{
  Z_NAMED_CLASS;
  dzSysPortArray input;
  zVec output;
  void *prp; /* utility for inheritance class of dzSys */
  dzSysCom *com; /* methods */
} dzSys;

#define dzSysInput(s)         ( &(s)->input )
#define dzSysOutput(s)        ( (s)->output )

#define dzSysInputNum(s)      zArraySize( dzSysInput(s) )
#define dzSysOutputNum(s)     zVecSizeNC( dzSysOutput(s) )

#define dzSysAllocInput(s,n)  zArrayAlloc( dzSysInput(s), dzSysPort, n )
#define dzSysAllocOutput(s,n) ( dzSysOutput(s) = zVecAlloc(n) )

#define dzSysInputElem(s,i)   zArrayElem( dzSysInput(s), i )
#define dzSysInputPtr(s,i)    ( dzSysInputElem(s,i)->vp )
#define dzSysInputVal(s,i)    ( dzSysInputPtr(s,i) ? *dzSysInputPtr(s,i) : 0 )
#define dzSysOutputVal(s,i)   zVecElemNC( dzSysOutput(s), i )

#define dzSysInit(s) do{\
  zNameSet( s, NULL );\
  zArrayInit( dzSysInput(s) );\
  dzSysOutput(s) = NULL;\
  (s)->prp = NULL;\
  (s)->com = NULL;\
} while(0)

/*! \brief destroy, refresh and update dynamical systems.
 *
 * dzSys class instance \a c is created by a particular
 * constructor function. Though the implementation of
 * constructor depends on the function of the system, it
 * commonly assigns the destructor, refresher and updater.
 * These methods are called by dzSysDestroy(),
 * dzSysRefresh() and dzSysUpdate(), respectively.
 *
 * dzSysDestroy() destroys the internal work space of \a c.
 *
 * dzSysRefresh() refreshes the internally memorized values
 * of \a c, if necessary.
 *
 * dzSysUpdate() updates the output of \a c in accordance
 * with its dynamics.
 * \return
 * dzSysUpdate() returns the output value of \a c updated.
 *
 * dzSysDestroy() and dzSysRefresh() return no value.
 */
#define dzSysDestroy(s)  (s)->com->_destroy( s )
#define dzSysRefresh(s)  (s)->com->_refresh( s )
#define dzSysUpdate(s,h) (s)->com->_update( s, h )

/*! \brief connect dynamical systems.
 *
 * dzSysConnect() connects the system \a c1 to the other
 * \a c2 as inputs.
 *
 * dzSysChain() connects multiple systems. \a n is the
 * number of the systems to be chained.
 *  Example:
 *   dzSysChain( 4, &c1, &c2, &c3, &c4 );
 * is equivalent to the following calls.
 *   dzSysConnect( &c1, &c2 );
 *   dzSysConnect( &c2, &c3 );
 *   dzSysConnect( &c3, &c4 );
 * \return
 * dzSysConnect() and dzSysChain() return the true value if
 * they succeed to connect the specified systems. Otherwise,
 * the false value is returned.
 */
__EXPORT bool dzSysConnect(dzSys *s1, int p1, dzSys *s2, int p2);
__EXPORT void dzSysChain(int n, ...);

/* default destroying method */
__EXPORT void dzSysDefaultDestroy(dzSys *sys);

/* default refreshing method */
__EXPORT void dzSysDefaultRefresh(dzSys *sys);

#define ZTK_TAG_DZSYS "sys"
#define ZTK_TAG_DZSYS_CONNECT "connect"

__EXPORT void *dzSysFromZTK(dzSys *sys, ZTK *ztk);

__EXPORT void dzSysFPrintZTK(FILE *fp, dzSys *sys);

/* ********************************************************** */
/* \class dzSysArray
 * ********************************************************** */

zArrayClass( dzSysArray, dzSys );

/*! \brief allocate an array of systems. */
__EXPORT dzSysArray *dzSysArrayAlloc(dzSysArray *arr, int size);

/*! \brief destroy an array of systems. */
__EXPORT void dzSysArrayDestroy(dzSysArray *arr);

/*! \brief find a system from array by name. */
__EXPORT dzSys *dzSysArrayNameFind(dzSysArray *arr, const char *name);

/*! \brief update all systems of an array. */
__EXPORT void dzSysArrayUpdate(dzSysArray *arr, double dt);

__EXPORT bool dzSysRegZTK(ZTK *ztk);
__EXPORT dzSysArray *dzSysArrayFromZTK(dzSysArray *sarray, ZTK *ztk);
__EXPORT dzSysArray *dzSysArrayScanZTK(dzSysArray *sarray, char filename[]);

__EXPORT void dzSysArrayFPrintZTK(FILE *fp, dzSysArray *sys);

__END_DECLS

/* built-in system classes */

#include <dzco/dz_sys_misc.h> /* miscellenies */
#include <dzco/dz_sys_pid.h>  /* PID compensators */
#include <dzco/dz_sys_lag.h>  /* first-order and second-order lag systems */
#include <dzco/dz_sys_lin.h>  /* linear systems */
#include <dzco/dz_sys_tf.h>   /* transfer function by polynomial rational expression */

#include <dzco/dz_sys_filt_maf.h> /* moving-average filter */
#include <dzco/dz_sys_filt_bw.h>  /* Butterworth filter */

#include <dzco/dz_sys_fg.h> /* function generators */

__BEGIN_DECLS

/* add a handle to the following list when you create a new system class. */
#define DZ_SYS_COM_ARRAY \
  dzSysCom *_dz_sys_com[] = {\
    &dz_sys_adder_com, &dz_sys_subtr_com, &dz_sys_limit_com,\
    &dz_sys_p_com, &dz_sys_i_com, &dz_sys_d_com, &dz_sys_pid_com, &dz_sys_qpd_com,\
    &dz_sys_fol_com, &dz_sys_sol_com, &dz_sys_pc_com, &dz_sys_adapt_com,\
    &dz_sys_lin_com,\
    &dz_sys_tf_com,\
    &dz_sys_maf_com, &dz_sys_bw_com,\
    &dz_sys_step_com, &dz_sys_ramp_com, &dz_sys_sine_com, &dz_sys_whitenoise_com,\
    NULL,\
  }

__END_DECLS

#endif /* __DZ_SYS_H__ */
