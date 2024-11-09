/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_lin - linear system
 */

#ifndef __DZ_LIN_H__
#define __DZ_LIN_H__

#include <dzco/dz_tf.h>

__BEGIN_DECLS

/* ********************************************************** */
/* CLASS: dzLin
 * general linear system
 * ********************************************************** */

ZDEF_STRUCT( __DZCO_CLASS_EXPORT, dzLin ){
  zMat a;    /*!< A matrix */
  zVec b;    /*!< B matrix */
  zVec c;    /*!< C matrix */
  zVec x;    /*!< state variable vector */
  double d;  /*!< direct transmission coefficient */
  /*! \cond */
  zVec _ax;  /* inner working memory space */
  zVec _bu;  /* inner working memory space */
  zODE _ode; /* integrator */
  /*! \endcond */
};

#define dzLinDim(c) zVecSizeNC((c)->x)

/*! \brief initialize a linear system.
 */
__DZCO_EXPORT dzLin *dzLinInit(dzLin *lin);

/*! \brief allocate and destroy work space for general linear system.
 *
 * dzLinAlloc() creates a general linear system \a c desecribed
 * as
 *   dx = A x + b u    (state equation)
 *    y = c x + d u    (output equation)
 * \a n is a dimention of the system.
 *
 * dzLinDestroy() destroys the internal work space of \a c.
 * \return
 * dzLinAlloc() returns the true value if succeeding to create
 * the instance, or the false value otherwise.
 *
 * dzLinDestroy() returns no value.
 */
__DZCO_EXPORT bool dzLinAlloc(dzLin *c, int dim);
__DZCO_EXPORT void dzLinDestroy(dzLin *c);

/*! \brief output and update the inner state of linear system.
 *
 * dzLinStateUpdate() updates the inner state of linear system
 * \a c. \a input is the input to \a c.
 *
 * dzLinObsUpdate() updates the inner state of linear system
 * \a c which functions as an observer.
 * \a k is an observer gain, \a input is the input to \a c,
 * and \a error is an error between the predicted and the real
 * output of the plant (the former minus the latter is \a error).
 *
 * dzLinOutput() computes the output value of \a c by the
 * current state and the input \a input.
 * \return
 * dzLinStateUpdate() and dzLinObsUpdate() returns no value.
 *
 * dzLinOutput() returns the value calculated.
 * \notes
 * dzLin function family does never check if the given system
 * is really a linear system. When a system created by other
 * function than dzLinAlloc() is given, anything might happen.
 * \sa
 * dzLinCreateObs, dzLinStateFeedback
 */
__DZCO_EXPORT void dzLinStateUpdate(dzLin *c, double input, double dt);
__DZCO_EXPORT void dzLinObsUpdate(dzLin *c, zVec k, double input, double error, double dt);
__DZCO_EXPORT double dzLinOutput(dzLin *c, double input);

/*! \brief state feedback for linear system.
 *
 * dzLinStateFeedback() reinputs a state-feedback value, namely,
 * u = -\a f ( x - \a ref ), where \a f is a feedback gain and
 * \a ref is the reference.
 * \return
 * dzLinStateFeedback() returns the computed input value.
 * \notes
 * When the input of \a c is not detected, nothing happens.
 * \sa
 * dzLinPoleAssign, dzLinCreateObs, dzLinLQR
 */
__DZCO_EXPORT double dzLinStateFeedback(dzLin *c, zVec ref, zVec f);

/*! \brief create controllability / observability matrix.
 *
 * dzLinCtrlMat() and dzLinObsMat() make the given matrix \a m
 * to be the controllability / observability matrix of a linear
 * system \a c, respectively.
 * \return
 * dzLinCtrlMat() and dzLinObsMat() return a pointer \a m.
 */
__DZCO_EXPORT zMat dzLinCtrlMat(dzLin *c, zMat m);
__DZCO_EXPORT zMat dzLinObsMat(dzLin *c, zMat m);

/*! \brief values to check if the linear system is controllable / observable.
 */
__DZCO_EXPORT double dzLinCtrlDet(dzLin *c);
__DZCO_EXPORT double dzLinObsDet(dzLin *c);

/*! \brief check if the linear system is controllable / observable.
 *
 * dzLinIsCtrl() checks if the linear system \a c is controllable.
 *
 * dzLinIsObs() checks if the linear system \a c is observable.
 * \return
 * dzLinIsCtrl() returns the true value if \a c is controllable.
 *
 * dzLinIsObs() returns the true value if \a c is observable.
 */
__DZCO_EXPORT bool dzLinIsCtrl(dzLin *c);
__DZCO_EXPORT bool dzLinIsObs(dzLin *c);

/*! \brief pole assignment of linear system control.
 *
 * dzLinCtrlCanon() calculates the "inverse" of a matrix which
 * transforms the given linear system \a c into a controllable
 * canonical form.
 *
 * Suppose the system is expressed as follows.
 *   dx/dt = A x + b u
 *       y = c x + d u
 * Then, it will be tranformed into a controllable
 * canonical form by \a t as follows.
 *   dz/dt = \a t A \a t^-1 z + \a t b u
 *       y = c \a t^-1 z + d u
 *
 * dzLinPoleAssign() calculates a state feedback gain vector
 * \a f which assigns the poles of resultant autonomous system
 * for \a c to the given \a pole.
 *
 * dzLinCreateObs() creates an observer of the given linear
 * system \a c so as to locate the eigenvalues at \a pole. The
 * observer gains calculated is put into \a f. The implementation
 * of it is in accordance with duality, and utilizes dzLinPoleAssign().
 * \return
 * dzLinCtrlCanon() returns a pointer to \a t, if succeeding.
 * Otherwise, the null pointer is returned.
 *
 * dzLinPoleAssign() returns a pointer \a f, if succeeding.
 * Otherwise, the null pointer is returned.
 *
 * dzLinCreateObs() returns a pointer \a f, if succeeding.
 * Otherwise, the null pointer is returned.
 * \notes
 * Note that dzLinCtrlCanon() calculates "the inverse" of a
 * transformation matrix for convenience of implementation.
 * \sa
 * dzLinStateFeedback, dzLinObsUpdate
 */
__DZCO_EXPORT zMat dzLinCtrlCanon(dzLin *c, zMat t);
__DZCO_EXPORT zVec dzLinPoleAssign(dzLin *c, zVec pole, zVec f);
__DZCO_EXPORT zVec dzLinCreateObs(dzLin *c, zVec pole, zVec k);

/*! \brief algebraic matrix Riccati equation solver.
 *
 * Algebraic matrix Riccati equation (AMRE), which forms as
 * follows, is handled:
 *  Q+P A + A^T P - P BRB^T P = 0.
 *
 * dzLinRiccatiErrorDRC() and dzLinRiccatiError() are for
 * internal computation and debug. Basically, they compute
 * Q+P A + A^T P - P BRB^T P, where A and B are held in a
 * linear system \a c. The residual matrix will be stored
 * in \a e, if it is not the null pointer.
 *
 * dzLinRiccatiSolveEuler() solves AMRE by Euler integration
 * of the following differencial equation:
 *  Q+P A + A^T P - P R P = dP/dt
 *
 * A numerical integration is done with a fixed quantized
 * time step Z_RICCATI_DT defined internally.
 *
 * \a tol is a tolerance to finish the iteration. Namely,
 * when the error norm becomes under \a tol, it judges that
 * iteration is finished. In practice, the iteration is done
 * up to \a iter. When zero is given for \a iter,
 * Z_MAX_ITER_NUM defined in zm_misc.h is chosen instead.
 * If the iteration does not be finished even if it repeats
 * more than \a iter times, the function gives it up the
 * calculation.
 *
 * dzLinRiccatiSolveKleinman() solves AMRE by Newton=Raphson
 * method, proposed by D. L. Kleinman (1967). In addition
 * to solve AMRE, the optimal feedback gain \a f of the
 * regulator consisting of \a c is simultaneously computed.
 * \return
 * dzLinRiccatiErrorDRC() and dzLinRiccatiError() return
 * the norm of the error matrix.
 *
 * dzLinRiccatiSolveEuler() and dzLinRiccatiSolveKleinman()
 * return a pointer \a p, if succeed. Otherwise, the null
 * pointer is returned.
 * \sa
 * dzLinLQR
 */
__DZCO_EXPORT double dzLinRiccatiErrorDRC(zMat p, dzLin *c, zMat q, double r, zMat res, zMat tmp, zVec pb);
__DZCO_EXPORT double dzLinRiccatiError(zMat p, dzLin *c, zMat q, double r, zMat e);
__DZCO_EXPORT zMat dzLinRiccatiSolveEuler(zMat p, dzLin *c, zMat q, double r, double tol, int iter);
__DZCO_EXPORT zMat dzLinRiccatiSolveKleinman(zMat p, zVec f, dzLin *c, zMat q, double r, double tol, int iter);

/*! \brief linear optimal regulator of linear system control.
 *
 * dzLinLQR() creates the linear-quadratic optimal regulator
 * of \a c. \a q is the weighting vector to the state vector,
 * where all components should be more than or equal to zero.
 * \a r, which must be positive, is the weighting value to
 * the input.
 * \return
 * dzLinLQR() returns a pointer \a f, if succeeding.
 * Otherwise, the null pointer is returned.
 * \sa
 * dzLinStateFeedback
 */
__DZCO_EXPORT zVec dzLinLQR(dzLin *c, zVec q, double r, zVec f);

/*! \brief conversion from polynomial transfer function to linear system.
 *
 * dzTF2LinCtrlCanon() converts the given polynomial transfer
 * function \a tf to an equivalent linear state equation and
 * output equation in controllable canonical form. The result
 * is put into \a lin.
 *
 * dzTF2LinObsCanon() converts the given polynomial transfer
 * function \a tf to an equivalent linear state equation and
 * output equation in observable canonical form. The result
 * is put into \a lin.
 * \return
 * dzTF2LinCtrlCanon() and dzTF2LinObsCanon() return a
 * pointer \a lin.
 * If the equivalent expression does not exist, they return
 * the null pointer.
 */
__DZCO_EXPORT dzLin *dzTF2LinCtrlCanon(dzTF *tf, dzLin *lin);
__DZCO_EXPORT dzLin *dzTF2LinObsCanon(dzTF *tf, dzLin *lin);

/* ZTK */

#define ZTK_KEY_DZCO_LIN_A "a"
#define ZTK_KEY_DZCO_LIN_B "b"
#define ZTK_KEY_DZCO_LIN_C "c"
#define ZTK_KEY_DZCO_LIN_D "d"

__DZCO_EXPORT dzLin *dzLinFromZTK(dzLin *lin, ZTK *ztk);
__DZCO_EXPORT void dzLinFPrintZTK(FILE *fp, dzLin *lin);

__END_DECLS

#endif /* __DZ_LIN_H__ */
