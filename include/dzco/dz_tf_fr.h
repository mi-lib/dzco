/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_tf_ident_fr - transfer function: identification from frequency response
 */

#ifndef __DZ_TF_IDENT_FR_H__
#define __DZ_TF_IDENT_FR_H__

/* NOTE: never include this header file in user programs. */

__BEGIN_DECLS

/* ********************************************************** */
/*! \class dzFreqRes
 * frequency response
 * ********************************************************** */

ZDEF_STRUCT( __DZCO_CLASS_EXPORT, dzFreqRes ){
  double f; /*!< frequency */
  double g; /*!< gain */
  double p; /*!< phase lag */
};

__DZCO_EXPORT zComplex *dzFreqResToComplex(dzFreqRes *fr, zComplex *c, double *af);
__DZCO_EXPORT dzFreqRes *dzFreqResFromComplex(dzFreqRes *fr, zComplex *c, double af);

__DZCO_EXPORT dzFreqRes *dzFreqRes2Closed(dzFreqRes *frin, dzFreqRes *frout);
__DZCO_EXPORT dzFreqRes *dzFreqRes2Open(dzFreqRes *frin, dzFreqRes *frout);

/*! \brief frequency response of transfer function.
 *
 * dzFreqResFromTF() calculates the frequency response \a fr of a transfer
 * function \a tf. \a af is an angular frequency.
 *
 * dzTFToComplex converts a transfer function \a tf to a complex number \a c
 * that respresents the frequency response of \a tf to an input angular
 * frequency \a af.
 *
 * Suppose the transfer function is G(s), and the result is G(j \a af).
 * \return
 * dzFreqResFromTF() returns a pointer \a fr.
 * dzTFToComplex() returns a pointer \a c.
 */
__DZCO_EXPORT dzFreqRes *dzFreqResFromTF(dzFreqRes *fr, dzTF *tf, double af);
__DZCO_EXPORT zComplex *dzTFToComplex(dzTF *tf, double af, zComplex *c);

/* ********************************************************** */
/*! \class dzFreqResList
 * list of sampled frequency responses
 * ********************************************************** */

zListClass( dzFreqResList, dzFreqResListCell, dzFreqRes );

#define dzFreqResListDestroy(list) zListDestroy( dzFreqResListCell, list )

__DZCO_EXPORT int dzFreqResList2Closed(dzFreqResList *inlist, dzFreqResList *outlist);
__DZCO_EXPORT int dzFreqResList2Open(dzFreqResList *inlist, dzFreqResList *outlist);
__DZCO_EXPORT int dzFreqResListConnectTF(dzFreqResList *inlist, dzTF *tf, dzFreqResList *outlist);

__DZCO_EXPORT int dzFreqResListFScan(FILE *fp, dzFreqResList *list, double fmin, double fmax);
__DZCO_EXPORT int dzFreqResListFPrint(FILE *fp, dzFreqResList *list, double fmin, double fmax);

__DZCO_EXPORT int dzFreqResListScanFile(dzFreqResList *list, char filename[], double fmin, double fmax);
__DZCO_EXPORT int dzFreqResListPrintFile(dzFreqResList *list, char filename[], double fmin, double fmax);

/* ********************************************************** */
/* identification of a transfer function from frequency response
 * ********************************************************** */

__DZCO_EXPORT dzTF *dzTFIdentFromFreqRes(dzTF *tf, dzFreqResList *list, int nn, int nd, int iter);

__END_DECLS

#endif /* __DZ_TF_IDENT_FR_H__ */
