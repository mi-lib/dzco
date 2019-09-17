/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_ident_lag - identify lag system
 */

#ifndef __DZ_IDENT_LAG_H__
#define __DZ_IDENT_LAG_H__

#include <zm/zm_opt.h>

#include <dzco/dz_errmsg.h>

__BEGIN_DECLS

/*! \brief find trigger time stamp for system identification.
 *
 * dzIdentTrig() finds the reference data which is at the trigger
 * time of step response. For example, if the data set \a r with
 * the total number \a n is as follows, the result is 5:
 *   r[] = { 1.0, 1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, ... };
 * It is supposed that the referential value changes only once.
 * Namely, the second and later time of value changes are ignored.
 * \return
 * dzIdentTrig() returns the identifier of the trigger data.
 */
__EXPORT int dzIdentTrig(double r[], int n);

/* ********************************************************** */
/* first-order-lag system identification
 * ********************************************************** */

/*! \brief identify first-order-lag system.
 *
 * dzIdentFOL1() and dzIdentFOL() identify the system parameters
 * which is modelled as a first-order-lag system from a log of
 * the step response.
 * \a t, \a r and \a y is the data set of time stamp, reference
 * and output log. \a n is the number of data.
 * \a trig'th data is regarded as that at the trigger time.
 *
 * The result is represented by a combination of time constant
 * and gain, which are put where are pointed by \a tc and \a gain,
 * respectively.
 *
 * dzIdentFOL1() approximately computes the time constant as the
 * time when the magnitude of data becomes over 1/e of the final
 * value, and the gain as the ratio of referential value and the
 * final value.
 *
 * dzIdentFOL() refines the parameters, applying a nonlinear least
 * square method (downhill simplex method).
 * \return
 * dzIdentFOL1() returns no value.
 *
 * dzIdentFOL() returns the true value if it succeeds to model the
 * system. Or, the false value is returned if it fails to allocate
 * internal workspace.
 */
__EXPORT void dzIdentFOL1(double t[], double r[], double y[], int n, int trig, double *tc, double *gain);
__EXPORT bool dzIdentFOL(double t[], double r[], double y[], int n, int trig, double *tc, double *gain);

/* ********************************************************** */
/* second-order-lag system identification
 * ********************************************************** */

/*! \brief identify second-order-lag system.
 *
 * dzIdentSOL1() and dzIdentSOL() identifies system parameters
 * which is modelled as a second-order-lag system from a log of
 * the step response.
 * \a t, \a r and \a y is the data set of time stamp, reference
 * and output log. \a n is the number of data.
 * \a trig'th data is regarded as that at the trigger time.
 *
 * The result is represented by a combination of time constant,
 * damping coefficient and gain, which are put where pointed by
 * \a tc, \a z and \a gain, respectively.
 *
 * dzIdentSOL1() approximately computes the time constant and
 * the damping coefficient from the peak time of the response,
 * and the gain as the ratio of referential value and the final
 * value.
 *
 * dzIdentSOL() refines the parameters, applying a nonlinear
 * least square method (downhill simplex method).
 * \return
 * dzIdentSOL1() returns no value.
 *
 * dzIdentSOL() returns the true value if it succeeds to model
 * the system. Or, the false value is returned if it fails to
 * allocate internal workspace.
 */
__EXPORT void dzIdentSOL1(double t[], double r[], double y[], int n, int trig, double *tc, double *z, double *gain);
__EXPORT bool dzIdentSOL(double t[], double r[], double y[], int n, int trig, double *tc, double *z, double *gain);

__END_DECLS

#endif /* __DZ_IDENT_LAG_H__ */
