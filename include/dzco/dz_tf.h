/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_tf - transfer function by polynomial rational expression
 */

#ifndef __DZ_TF_H__
#define __DZ_TF_H__

#include <dzco/dz_misc.h>

__BEGIN_DECLS

/* ********************************************************** */
/*! \class dzTF
 * transfer function by polynomial rational expression
 * ********************************************************** */

ZDEF_STRUCT( __DZCO_CLASS_EXPORT, dzTF ){
  zPex num; /*!< polynomial for numerator */
  zPex den; /*!< polynomial for denominator */
  /*! \cond */
  zCVec zero; /* zeros */
  zCVec pole; /* poles */
  /*! \endcond */
};

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

#define dzTFZero(tf)           (tf)->zero
#define dzTFPole(tf)           (tf)->pole

/*! \brief initialize a polynomial rational transfer function. */
#define dzTFInit(tf) do{\
  dzTFNum(tf) = dzTFDen(tf) = NULL;\
  dzTFZero(tf) = dzTFPole(tf) = NULL;\
} while(0)

/*! \brief allocate and destroy a polynomial rational transfer function.
 *
 * dzTFAlloc() creates the general polynomial transfer function
 * \a tf. \a nsize is the dimention of numerator, and \a dsize is
 * the dimention of denominator.
 *
 * dzTFCreateZeroPole() creates the general polynomial transfer
 * function \a tf from complex zeros and poles. \a zero is for zeros,
 * and \a pole is for poles. The numerator is multiplied by \a gain.
 *
 * dzTFDestroy() destroys the instance of transfer function \a tf,
 * freeing inner vectors of \a tf.
 * \return
 * dzTFCreate() returns the true value if it succeeds to create
 * an instance, or the false value otherwise.
 *
 * dzTFDestroy() returns no value.
 */
__DZCO_EXPORT bool dzTFAlloc(dzTF *tf, int nsize, int dsize);
__DZCO_EXPORT bool dzTFCreateZeroPole(dzTF *tf, zCVec zero, zCVec pole, double gain);
__DZCO_EXPORT void dzTFDestroy(dzTF *tf);

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
__DZCO_EXPORT void dzTFSetNumList(dzTF *tf, ...);
__DZCO_EXPORT void dzTFSetDenList(dzTF *tf, ...);

/*! \brief connect a transfer function.
 *
 * dzTFConnect() connects a transfer function \a ctf to \a tf.
 * \a tf is directly updated.
 * \return
 * dzTFConnect() returns a pointer \a tf, or the null pointer if
 * it fails to allocate memory for the new transfer function.
 */
__DZCO_EXPORT dzTF *dzTFConnect(dzTF *tf, dzTF *ctf);

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
__DZCO_EXPORT bool dzTFIsStable(dzTF *tf);

/*! \brief abstract zeros and poles of a transfer function.
 *
 * dzTFZeroPole() computes zeros and poles of a transfer function
 * \a tf, and puts them into newly allocated complex vectors that
 * are accessible from referring dzTFZero() and dzTFPole().
 * \return
 * dzTFZeroPole() returns the true value if succeedings. Otherwise,
 * the false value is returned.
 */
__DZCO_EXPORT bool dzTFZeroPole(dzTF *tf);

/*! \brief abstract zeros and poles of a transfer function into real and imaginary values.
 *
 * dzTFZeroPoleReIm() abstracts zeros and poles of a transfer function
 * \a tf, and puts them into newly allocated real and complex vectors.
 * The addresses of those vectors are stored in \a zero1, \a zero2,
 * \a pole1 and \a pole2, respectively, where \a zero1 and \a pole1
 * are for real values and \a zero2 and \a pole2 for imaginary values.
 * \return
 * dzTFZeroPoleReIm() returns the true value if succeedings. Otherwise,
 * the false value is returned.
 */
__DZCO_EXPORT bool dzTFZeroPoleReIm(dzTF *tf, zVec *zero1, zCVec *zero2, zVec *pole1, zCVec *pole2);

/* ZTK */

#define ZTK_KEY_DZCO_TF_NUMERATOR   "num"
#define ZTK_KEY_DZCO_TF_DENOMINATOR "den"
#define ZTK_KEY_DZCO_TF_ZERO        "zero"
#define ZTK_KEY_DZCO_TF_POLE        "pole"
#define ZTK_KEY_DZCO_TF_GAIN        "gain"

__DZCO_EXPORT dzTF *dzTFFromZTK(dzTF *tf, ZTK *ztk);
__DZCO_EXPORT void dzTFFPrintZTK(FILE *fp, dzTF *tf);

/*! \brief read a transfer function from a ZTK file. */
__DZCO_EXPORT dzTF *dzTFReadZTK(dzTF *tf, char filename[]);
/*! \brief write a transfer function to a ZTK file. */
__DZCO_EXPORT bool dzTFWriteZTK(dzTF *tf, char filename[]);

__DZCO_EXPORT void dzTFFExpr(FILE *fp, dzTF *tf);
#define dzTFExpr(tf) dzTFFExpr( stdout, tf )

__END_DECLS

#include <dzco/dz_tf_fr.h> /* frequency response */

#endif /* __DZ_TF_H__ */
