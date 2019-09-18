/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_tf - transfer function by polynomial rational expression
 */

#ifndef __DZ_TF_H__
#define __DZ_TF_H__

#include <zm/zm.h>

#include <dzco/dz_errmsg.h>

__BEGIN_DECLS

/* ********************************************************** */
/*! \class dzTF
 * transfer function by polynomial rational expression
 * ********************************************************** */

typedef struct{
  zPex num; /*!< polynomial for numerator */
  zPex den; /*!< polynomial for denominator */
} dzTF;

#define dzTFNum(tf)            (tf)->num
#define dzTFDen(tf)            (tf)->den
#define dzTFNumDim(tf)         zPexDim( dzTFNum(tf) )
#define dzTFDenDim(tf)         zPexDim( dzTFDen(tf) )
#define dzTFNumElem(tf,i)      zPexCoeff( dzTFNum(tf), i )
#define dzTFDenElem(tf,i)      zPexCoeff( dzTFDen(tf), i )

#define dzTFSetNum(tf,n)       ( dzTFNum(tf) = (n) )
#define dzTFSetDen(tf,d)       ( dzTFDen(tf) = (d) )
#define dzTFSetNumElem(tf,i,e) zPexSetCoeff( dzTFNum(tf), i, e )
#define dzTFSetDenElem(tf,i,e) zPexSetCoeff( dzTFDen(tf), i, e )

/*! \brief allocate and destroy a polynomial rational transfer function.
 *
 * dzTFAlloc() creates the general polynomial transfer function
 * \a tf. \a nsize is the dimention of numerator, and \a dsize is
 * the dimention of denominator.
 *
 * dzTFCreateZeroPole() creates the general polynomial transfer
 * function \a tf from zeros and poles. \a zero is for zeros, and
 * \a pole is for poles.
 *
 * dzTFDestroy() destroys the instance of transfer function \a tf,
 * freeing inner vectors of \a tf.
 * \return
 * dzTFCreate() returns the true value if it succeeds to create
 * an instance, or the false value otherwise.
 *
 * dzTFDestroy() returns no value.
 */
__EXPORT bool dzTFAlloc(dzTF *tf, int nsize, int dsize);
__EXPORT bool dzTFCreateZeroPole(dzTF *tf, zVec zero, zVec pole);
__EXPORT void dzTFDestroy(dzTF *tf);

/*! \brief set coefficients of numerator and denominator of
 *   transfer function.
 *
 * dzTFSetNumList() and dzTFSetDenList() set coefficients of
 * numerator and denominator of a transfer function \a tf,
 * respectively.
 *
 * \a tf is followed by a list of coefficients in ascending
 * ordered of powers.
 *  (i.e. a_0 + a_1 s + a_2 s^2 + a_3 s^3 + ... )
 *
 * They accept variable argument list, but never check the number
 * of arguments. If it differs from the one \a tf stores, anything
 * might happen.
 * \return
 * Neither dzTFSetNumList() nor dzTFSetDenList() return any values.
 */
__EXPORT void dzTFSetNumList(dzTF *tf, ...);
__EXPORT void dzTFSetDenList(dzTF *tf, ...);

/*! \brief check if polynomial system is stable.
 *
 * dzTFIsStable() checks if the polynomial system \a tf is stable
 * by Routh=Hurwitz's method.
 * \return
 * dzTFIsStable() returns the true value if the system is stable,
 * or the false value otherwise.
 * \notes
 * dzTFIsStable() is not available in kernel space.
 */
__EXPORT bool dzTFIsStable(dzTF *tf);

/*! \brief frequency response of transfer function.
 *
 * dzTFFreqRes() calculates the frequency response of the transfer
 * function \a tf. \a frq is the frequency checked. The result is
 * put into \a res.
 *
 * Suppose the transfer function is G(s), and the result is G(jw).
 * \return
 * dzTFFreqRes() returns a pointer to \a res.
 * \notes
 * dzTFFreqRes() is not available in kernel space.
 */
__EXPORT zComplex *dzTFFreqRes(dzTF *tf, double frq, zComplex *res);

__EXPORT bool dzTFRegZTK(ZTK *ztk, char *tag);
__EXPORT dzTF *dzTFFromZTK(dzTF *tf, ZTK *ztk);

/*! \brief scan a ZTK file and create a transfer function. */
__EXPORT dzTF *dzTFScanZTK(dzTF *tf, char filename[]);

__EXPORT void dzTFFPrintZTK(FILE *fp, dzTF *tf);
#define dzTFPrintZTK(tf) dzTFFPrint( stdout, tf )

__EXPORT void dzTFFExpr(FILE *fp, dzTF *tf);
#define dzTFExpr(tf) dzTFFExpr( stdout, tf )

__END_DECLS

#endif /* __DZ_TF_H__ */
