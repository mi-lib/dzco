/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_pex - polynomial rational expression of transfer functions
 */

#ifndef __DZ_PEX_H__
#define __DZ_PEX_H__

#include <zm/zm.h>

__BEGIN_DECLS

/* ********************************************************** */
/* CLASS: dzPex
 * general polynomial transfer function
 * ********************************************************** */

typedef struct{
  zPex num; /*!< numerator polynomial */
  zPex den; /*!< denominator polynomial */
} dzPex;

#define dzPexNum(p)            (p)->num
#define dzPexDen(p)            (p)->den
#define dzPexNumDim(p)         zPexDim( dzPexNum(p) )
#define dzPexDenDim(p)         zPexDim( dzPexDen(p) )
#define dzPexNumElem(p,i)      zPexCoeff( dzPexNum(p), i )
#define dzPexDenElem(p,i)      zPexCoeff( dzPexDen(p), i )

#define dzPexSetNum(p,n)       ( dzPexNum(p) = (n) )
#define dzPexSetDen(p,d)       ( dzPexDen(p) = (d) )
#define dzPexSetNumElem(p,i,e) zPexSetCoeff( dzPexNum(p), i, e )
#define dzPexSetDenElem(p,i,e) zPexSetCoeff( dzPexDen(p), i, e )

/*! \brief allocate and destroy a polynomial rational transfer function.
 *
 * dzPexAlloc() creates the general polynomial transfer function
 * \a c. \a nsize is the dimention of numerator, and \a dsize is
 * the dimention of denominator.
 *
 * dzPexCreateZeroPole() creates the general polynomial transfer
 * function \a c from zeros and poles. \a zero is for zeros, and
 * \a pole is for poles.
 *
 * dzPexDestroy() destroys the instance of transfer function \a c,
 * freeing inner vectors of \a c.
 * \return
 * dzPexCreate() returns the true value if it succeeds to create
 * an instance, or the false value otherwise.
 *
 * dzPexDestroy() returns no value.
 */
__EXPORT bool dzPexAlloc(dzPex *pex, int nsize, int dsize);
__EXPORT bool dzPexCreateZeroPole(dzPex *pex, zVec zero, zVec pole);
__EXPORT void dzPexDestroy(dzPex *pex);

/*! \brief set coefficients of numerator and denominator of
 *   transfer function.
 *
 * dzPexSetNumList() and dzPexSetDenList() set coefficients of
 * numerator and denominator of a transfer function \a c,
 * respectively.
 *
 * \a c is followed by the list of coefficients in ascending
 * ordered of powers.
 *  (i.e. a_0 + a_1 s + a_2 s^2 + a_3 s^3 + ... )
 *
 * They accept variable argument list, but never check the number
 * of arguments. If it differs from the one \a c stores, anything
 * might happen.
 * \return
 * Neither dzPexSetNumList() nor dzPexSetDenList() return any values.
 */
__EXPORT void dzPexSetNumList(dzPex *pex, ...);
__EXPORT void dzPexSetDenList(dzPex *pex, ...);

/*! \brief check if polynomial system is stable.
 *
 * dzPexIsStable() checks if the polynomial system \a c is stable
 * by Routh=Hurwitz's method.
 * \return
 * dzPexIsStable() returns the true value if the system is stable,
 * or the false value otherwise.
 * \notes
 * dzPexIsStable() is not available in kernel space.
 */
__EXPORT bool dzPexIsStable(dzPex *pex);

/*! \brief frequency response of transfer function.
 *
 * dzPexFreqRes() calculates the frequency response of the transfer
 * function \a c. \a frq is the frequency checked. The result is
 * put into \a res.
 *
 * Suppose the transfer function is G(s), and the result is G(jw).
 * \return
 * dzPexFreqRes() returns a pointer to \a res.
 * \notes
 * dzPexFreqRes() is not available in kernel space.
 */
__EXPORT zComplex *dzPexFreqRes(dzPex *pex, double frq, zComplex *res);

/*! \brief scan and print a polynomial transfer function from file.
 */
__EXPORT dzPex *dzPexFScan(FILE *fp, dzPex *pex);
#define dzPexScan(p)  dzPexFScan( stdin, p )
__EXPORT void dzPexFPrint(FILE *fp, dzPex *pex);
#define dzPexPrint(p) dzPexFPrint( stdout, p )
__EXPORT void dzPexFExpr(FILE *fp, dzPex *pex);
#define dzPexExpr(p) dzPexFExpr( stdout, p )

__END_DECLS

#endif /* __DZ_PEX_H__ */
