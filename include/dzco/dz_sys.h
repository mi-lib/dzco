/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys - system class
 */

#ifndef __DZ_SYS_H__
#define __DZ_SYS_H__

#include <zm/zm.h>

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
  const char *type;
  void (*destroy)(struct _dzSys*);
  void (*refresh)(struct _dzSys*);
  zVec (*update)(struct _dzSys*, double dt);
  struct _dzSys *(*fscan)(FILE *fp, struct _dzSys*);
  void (*fprint)(FILE *fp, struct _dzSys*);
} dzSysMethod;

typedef struct _dzSys{
  Z_NAMED_CLASS;
  dzSysPortArray input;
  zVec output;
  void *_prm; /* utility for inheritance class of dzSys */
  dzSysMethod *_met; /* method */
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
  (s)->_prm = NULL;\
  (s)->_met = NULL;\
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
#define dzSysDestroy(s)  (s)->_met->destroy( s )
#define dzSysRefresh(s)  (s)->_met->refresh( s )
#define dzSysUpdate(s,h) (s)->_met->update( s, h )

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
__EXPORT void dzSysDestroyDefault(dzSys *sys);

/* default refreshing method */
__EXPORT void dzSysRefreshDefault(dzSys *sys);

#define DZ_SYS_TAG "sys"
#define DZ_SYS_CONNECT_TAG "connect"
__EXPORT dzSys *dzSysFScan(FILE *fp, dzSys *sys);
__EXPORT void dzSysFPrint(FILE *fp, dzSys *sys);

/* ********************************************************** */
/* \class dzSysArray
 * ********************************************************** */

zArrayClass( dzSysArray, dzSys );

__EXPORT void dzSysArrayDestroy(dzSysArray *arr);

__EXPORT dzSys *dzSysArrayNameFind(dzSysArray *arr, const char *name);

__EXPORT void dzSysArrayUpdate(dzSysArray *arr, double dt);

__EXPORT bool dzSysArrayFScan(FILE *fp, dzSysArray *sys);
__EXPORT void dzSysArrayFPrint(FILE *fp, dzSysArray *sys);

__END_DECLS

/* built-in system classes */

#include <dzco/dz_sys_misc.h> /* miscellenies */
#include <dzco/dz_sys_pid.h>  /* PID compensators */
#include <dzco/dz_sys_lag.h>  /* first-order and second-order lag systems */
#include <dzco/dz_sys_lin.h>  /* linear systems */
#include <dzco/dz_sys_pex.h>  /* polynomial rational transfer functions */

#include <dzco/dz_sys_filt_maf.h> /* moving-average filter */
#include <dzco/dz_sys_filt_bw.h>  /* Butterworth filter */

#include <dzco/dz_sys_gen.h> /* function generators */

#endif /* __DZ_SYS_H__ */
