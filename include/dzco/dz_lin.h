/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_lin - linear system
 */

#ifndef __DZ_LIN_H__
#define __DZ_LIN_H__

#include <dzco/dz_pex.h>

__BEGIN_DECLS

/* ********************************************************** */
/* CLASS: dzLin
 * general linear system
 * ********************************************************** */

typedef struct{
  zMat a;
  zVec b, c, x;
  double d;
  zVec _ax, _bu; /* inner working memory space */
  zODE _ode;     /* integrator */
} dzLin;

#define dzLinDim(c) zVecSizeNC((c)->x)

/*! \brief initialize a linear system.
 */
__EXPORT dzLin *dzLinInit(dzLin *lin);

/* METHOD:
 * dzLinAlloc, dzLinDestroy
 * - allocate and destroy work space for general linear system.
 *
 * 'dzLinAlloc()' creates a general linear system 'c'
 * desecribed as
 *   dx = A x + b u    (state equation)
 *    y = c x + d u    (output equation)
 * 'n' is a dimention of the system.
 *
 * 'dzLinDestroy()' destroys the internal work space of 'c'.
 * [RETURN VALUE]
 * 'dzLinAlloc()' returns the true value if succeeding
 * to create the instance, or the false value otherwise.
 *
 * 'dzLinDestroy()' returns no value.
 */
__EXPORT bool dzLinAlloc(dzLin *c, int dim);
__EXPORT void dzLinDestroy(dzLin *c);

/* METHOD:
 * dzLinStateUpdate, dzLinObsUpdate, dzLinOutput
 * - output and update the inner state of linear system.
 *
 * 'dzLinStateUpdate()' updates the inner state of
 * linear system 'c'. 'input' is the input to 'c'.
 *
 * 'dzLinObsUpdate()' updates the inner state of
 * linear system 'c' which functions as an observer.
 * 'k' is an observer gain, 'input' is the input to
 * 'c', and 'error' is an error between the predicted
 * and the real output of the plant (the former minus
 * the latter is 'error').
 *
 * 'dzLinOutput()' computes the output value of
 * 'c' by the current state and the input 'input'.
 * [RETURN VALUE]
 * 'dzLinStateUpdate()' and 'dzLinObsUpdate()' returns
 * no value.
 *
 * 'dzLinOutput()' returns the value calculated.
 * [NOTES]
 * dzLin function family does never check if the given
 * system is really a linear system. When a system
 * created by other function than 'dzLinAlloc()' is
 * given, anything might happen.
 * [SEE ALSO]
 * dzLinCreateObs, dzLinStateFeedback
 */
__EXPORT void dzLinStateUpdate(dzLin *c, double input, double dt);
__EXPORT void dzLinObsUpdate(dzLin *c, zVec k, double input, double error, double dt);
__EXPORT double dzLinOutput(dzLin *c, double input);

/* METHOD:
 * dzLinStateFeedback - state feedback for linear system.
 *
 * 'dzLinStateFeedback()' reinputs a state-feedback
 * value, namely, u = -'f' ( x - 'ref' ), where 'f'
 * is a feedback gain and 'ref' is the reference.
 * [RETURN VALUE]
 * 'dzLinStateFeedback()' returns the input value
 * calculated.
 * [NOTE]
 * When the input of 'c' is not detected, nothing
 * happens.
 * [SEE ALSO]
 * dzLinPoleAssign, dzLinCreateObs, dzLinLQR
 */
__EXPORT double dzLinStateFeedback(dzLin *c, zVec ref, zVec f);

/* METHOD:
 * dzLinCtrlMat, dzLinObsMat
 * - create controllable matrix and observable matrix.
 *
 * 'dzLinCtrlMat()' and 'dzLinObsMat()' make the
 * given matrix 'm' to be the controllable matrix
 * and the observable matrix of a linear system 'c',
 * respectively.
 * [RETURN VALUE]
 * 'dzLinCtrlMat()' and 'dzLinObsMat()' return a
 * pointer 'm'.
 */
__EXPORT zMat dzLinCtrlMat(dzLin *c, zMat m);
__EXPORT zMat dzLinObsMat(dzLin *c, zMat m);

/* METHOD:
 * dzLinIsCtrl, dzLinIsObs
 * - check if the linear system is controllable and observable.
 *
 * 'dzLinIsCtrl()' checks if the linear system 'c'
 * is controllable.
 *
 * 'dzLinIsObs()' checks if the linear system 'c'
 * is observable.
 * [RETURN VALUE]
 * 'dzLinIsCtrl()' returns the true value if 'c' is
 * controllable.
 *
 * 'dzLinIsObs()' returns the true value if 'c' is
 * observable.
 */
__EXPORT bool dzLinIsCtrl(dzLin *c);
__EXPORT bool dzLinIsObs(dzLin *c);

/* METHOD:
 * dzLinCtrlCanon, dzLinPoleAssign, dzLinCreateObs
 * - pole assignment of linear system control.
 *
 * 'dzLinCtrlCanon()' calculates the "inverse" of a
 * matrix which transforms the given linear system
 * 'c' into a controllable canonical form.
 *
 * Suppose the system is expressed as follows.
 *   dx/dt = A x + b u
 *       y = c x + d u
 * Then, it will be tranformed into a controllable
 * canonical form by 't' as follows.
 *   dz/dt = t A t^-1 z + t b u
 *       y = c t^-1 z + d u
 *
 * 'dzLinPoleAssign()' calculates a state feedback gain
 * vector 'f' which assigns the poles of resultant
 * autonomous system for 'c' to the given 'pole'.
 *
 * 'dzLinCreateObs()' creates an observer of the
 * given linear system 'c' so as to locate the eigenvalues
 * at 'pole'. The observer gains calculated is put
 * into 'f'. The implementation of it is in accordance
 * with duality, and utilizes 'dzLinPoleAssign()'.
 * [RETURN VALUES]
 * 'dzLinCtrlCanon()' returns a pointer to 't', if
 * succeeding, or the null pointer, otherwise.
 *
 * 'dzLinPoleAssign()' returns a pointer to 'f', if succeeding,
 * or the null pointer, otherwise.
 *
 * 'dzLinCreateObs()' returns a pointer to 'f', if
 * succeeding, or the null pointer otherwise.
 * [NOTES]
 * Note that 'dzLinCtrlCanon()' calculates "the inverse"
 * of a transformation matrix for convenience of the
 * implementation.
 * [SEE ALSO]
 * dzLinStateFeedback, dzLinObsUpdate
 */
__EXPORT zMat dzLinCtrlCanon(dzLin *c, zMat t);
__EXPORT zVec dzLinPoleAssign(dzLin *c, zVec pole, zVec f);
__EXPORT zVec dzLinCreateObs(dzLin *c, zVec pole, zVec k);

/* METHOD:
 * dzLinRiccatiErrorDRC, dzLinRiccatiError,
 * dzLinRiccatiSolveEuler, dzLinRiccatiSolveKleinman
 * - algebraic matrix Riccati equation solver.
 *
 * Algebraic matrix Riccati equation (AMRE), which forms as
 * follows, is handled:
 *  'Q+P A + A^T P - P BRB^T P = 0'.
 *
 * 'dzLinRiccatiErrorDRC()' and 'dzLinRiccatiError()'
 * are for internal computation and debugs. Basically,
 * they compute 'Q+P A + A^T P - P BRB^T P', where
 * A and B are held in a linear system 'c'.
 * the residual matrix will be stored in 'e', if it is
 * not the null pointer.
 *
 * 'dzLinRiccatiSolveEuler()' solves AMRE by Euler
 * integration of the following differencial equation:
 *  'Q+P A + A^T P - P R P = dP/dt'
 *
 * A numerical integration is done with a fixed
 * quantized time step Z_RICCATI_DT defined internally.
 *
 * 'tol' is a tolerance to finish the iteration. Namely,
 * when the error norm becomes under 'tol', it judges
 * that iteration is finished. In practice, the iteration
 * is done up to 'iter'. When zero is given for 'iter',
 * Z_MAX_ITER_NUM defined in 'zm_misc.h' is chosen instead.
 * If the iteration does not be finished even if it
 * repeats more than 'iter' times, the function gives
 * it up the calculation.
 *
 * 'dzLinRiccatiSolveKleinman()' solves AMRE by Newton=Raphson
 * method, proposed by D. L. Kleinman (1967). In addition
 * to solve AMRE, the optimal feedback gain 'f' of the
 * regulator consisting of 'c' is simultaneously comptued.
 * [RETURN VALUES]
 * 'dzLinRiccatiErrorDRC()' and 'dzLinRiccatiError()'
 * return the norm of the error matrix.
 *
 * 'dzLinRiccatiSolveEuler()' and 'dzLinRiccatiSolveKleinman()'
 * return a pointer 'p', if succeed. Otherwise, the null
 * pointer is returned.
 * [SEE ALSO]
 * dzLinLQR
 */
__EXPORT double dzLinRiccatiErrorDRC(zMat p, dzLin *c, zMat q, double r, zMat res, zMat tmp, zVec pb);
__EXPORT double dzLinRiccatiError(zMat p, dzLin *c, zMat q, double r, zMat e);
__EXPORT zMat dzLinRiccatiSolveEuler(zMat p, dzLin *c, zMat q, double r, double tol, int iter);
__EXPORT zMat dzLinRiccatiSolveKleinman(zMat p, zVec f, dzLin *c, zMat q, double r, double tol, int iter);

/* METHOD:
 * dzLinLQR - linear optimal regulator of linear system control.
 *
 * 'dzLinLQR()' creates the linear-quadratic optimal
 * regulator of 'c'. 'q' is the weighting vector onto
 * the state vector, where every components should be
 * more than or equal to zero.
 * 'r', which must be positive, is the weighting value
 * to the input.
 * [RETURN VALUES]
 * 'dzLinLQR()' returns a pointer to 'f', if succeeding,
 * or the null pointer, otherwise.
 * [SEE ALSO]
 * dzLinStateFeedback
 */
__EXPORT zVec dzLinLQR(dzLin *c, zVec q, double r, zVec f);

/* METHOD:
 * dzPex2LinCtrlCanon, dzPex2LinObsCanon
 * - conversion from polynomial transfer function to linear system.
 *
 * 'dzPex2LinCtrlCanon()' converts the given
 * polynomial transfer function 'sp' to an equivalent linear
 * state equation and output equation in controllable canonical
 * form. The result is put into 'sl'.
 *
 * 'dzPex2LinObsCanon()' converts the given
 * polynomial transfer function 'sp' to an equivalent linear
 * state equation and output equation in observable canonical
 * form. The result is put into 'sl'.
 * [RETURN VALUE]
 * Each of 'dzPex2LinCtrlCanon()' and
 * 'dzPex2LinObsCanon()' returns a pointer
 * to 'sl'.
 * If the equivalent expression does not exist, they return
 * the null pointer.
 */
__EXPORT dzLin *dzPex2LinCtrlCanon(dzPex *sp, dzLin *sl);
__EXPORT dzLin *dzPex2LinObsCanon(dzPex *sp, dzLin *sl);

/*! \brief read and write a linear system.
 */
__EXPORT dzLin *dzLinFRead(FILE *fp, dzLin *lin);
#define dzLinRead(l)   dzLinFRead( stdin, (l) )
__EXPORT void dzLinFWrite(FILE *fp, dzLin *lin);
#define dzLinWrite(l)  dzLinFWrite( stdout, (l) )

__END_DECLS

#endif /* __DZ_LIN_H__ */
