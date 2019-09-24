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

typedef struct{
  double f, g, p;
} dzFreqRes;

__EXPORT zComplex *dzFreqResToComplex(dzFreqRes *fr, zComplex *c, double *af);
__EXPORT dzFreqRes *dzFreqResFromComplex(dzFreqRes *fr, zComplex *c, double *af);

__EXPORT dzFreqRes *dzFreqRes2Closed(dzFreqRes *frin, dzFreqRes *frout);
__EXPORT dzFreqRes *dzFreqRes2Open(dzFreqRes *frin, dzFreqRes *frout);

/* ********************************************************** */
/*! \class dzFreqResList
 * list of sampled frequency responses
 * ********************************************************** */

zListClass( dzFreqResList, dzFreqResListCell, dzFreqRes );

#define dzFreqResListDestroy(list) zListDestroy( dzFreqResListCell, list )

__EXPORT int dzFreqResList2Closed(dzFreqResList *inlist, dzFreqResList *outlist);
__EXPORT int dzFreqResList2Open(dzFreqResList *inlist, dzFreqResList *outlist);
__EXPORT int dzFreqResListConnectTF(dzFreqResList *inlist, dzTF *tf, dzFreqResList *outlist);

__EXPORT int dzFreqResListFScan(FILE *fp, dzFreqResList *list, double fmin, double fmax);
__EXPORT int dzFreqResListFPrint(FILE *fp, dzFreqResList *list, double fmin, double fmax);

__EXPORT int dzFreqResListScanFile(dzFreqResList *list, char filename[], double fmin, double fmax);
__EXPORT int dzFreqResListPrintFile(dzFreqResList *list, char filename[], double fmin, double fmax);

__EXPORT dzTF *dzTFIdentFromFreqRes(dzTF *tf, dzFreqResList *list, int nn, int nd, int iter);

__END_DECLS

#endif /* __DZ_TF_IDENT_FR_H__ */
