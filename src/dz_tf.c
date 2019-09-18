/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_tf - transfer function by polynomial rational expression
 */

#include <dzco/dz_tf.h>

/* allocate memory of a polynomial rational transfer function. */
bool dzTFAlloc(dzTF *tf, int nsize, int dsize)
{
  dzTFSetNum( tf, zPexAlloc( nsize ) );
  dzTFSetDen( tf, zPexAlloc( dsize ) );
  if( !dzTFNum(tf) || !dzTFDen(tf) ){
    ZRUNERROR( DZ_ERR_TF_UNABLE_CREATE );
    dzTFDestroy( tf );
    return false;
  }
  return true;
}

/* create a polynomial rational transfer function from zeros and poles. */
bool dzTFCreateZeroPole(dzTF *tf, zVec zero, zVec pole)
{
  dzTFSetNum( tf, zPexExp( zero ) );
  dzTFSetDen( tf, zPexExp( pole ) );
  if( !dzTFNum(tf) || !dzTFDen(tf) ){
    ZRUNERROR( DZ_ERR_TF_UNABLE_CREATE );
    dzTFDestroy( tf );
    return false;
  }
  return true;
}

/* destroy a polynomial rational transfer function. */
void dzTFDestroy(dzTF *tf)
{
  zPexFree( dzTFNum(tf) );
  zPexFree( dzTFDen(tf) );
}

/* set coefficients of numerator of a transfer function. */
void dzTFSetNumList(dzTF *tf, ...)
{
  va_list args;

  va_start( args, tf );
  zVecSetElemVList( dzTFNum(tf), args );
  va_end( args );
}

/* set coefficients of denominator of a transfer function. */
void dzTFSetDenList(dzTF *tf, ...)
{
  va_list args;

  va_start( args, tf );
  zVecSetElemVList( dzTFDen(tf), args );
  va_end( args );
}

/* check if a polynomial rational transfer function is stable. */
static int __next(int i);
int __next(int i){ return i==2 ? 0 : i+1; }
bool dzTFIsStable(dzTF *tf)
{
  zVec v[3];
  register uint i, j, k, m, n;
  bool result = false;

  /* if the dimension of numerator is equal to or larger
     than that of denominator, or the denominator is at least
     not thoroughly positive, the system is unstable. */
  if( dzTFDenDim(tf) <= dzTFNumDim(tf) ) return false;
  for( i=0; i<zVecSizeNC(dzTFDen(tf)); i++ )
    if( zVecElemNC( dzTFDen(tf), i ) <= 0 ) return false;

  /* preparation of the initial tableau */
  i = dzTFDenDim( tf );
  n = ( i + 2 - i % 2 ) / 2;
  v[0] = zVecAlloc( n );
  v[1] = zVecAlloc( n );
  v[2] = zVecAlloc( n );
  if( !v[0] || !v[1] || !v[2] ) goto TERMINATE;
  for( j=i, k=0; j>=0; j-=2, k++ ){
    zVecSetElemNC( v[0], k, zPexCoeff( dzTFDen(tf), j ) );
    zVecSetElemNC( v[1], k, j==0 ? 0 : zPexCoeff( dzTFDen(tf), j-1 ) );
  }
  /* Routh=Hurwitz's method */
  for( i=0, j=1, k=2, n--; n>0; i=j, j=k, k=__next(k) ){
    zVecZero( v[k] );
    for( m=0; m<n; m++ ){
      zVecSetElemNC( v[k], m,
        ( zVecElemNC(v[i],m+1)*zVecElemNC(v[j],0)
          - zVecElemNC(v[i],0)*zVecElemNC(v[j],m+1) )
          / zVecElemNC(v[j],0) );
      if( zVecElemNC( v[k], m ) <= 0 ) goto TERMINATE;
    }
    if( zVecElemNC( v[j], n ) == 0 ) n--;
  }
  result = true;

 TERMINATE:
  zVecFree( v[0] );
  zVecFree( v[1] );
  zVecFree( v[2] );
  return result;
}

/* frequency response of a transfer function. */
zComplex *dzTFFreqRes(dzTF *tf, double frq, zComplex *res)
{
  zComplex n, d, f;

  zComplexCreate( &f, 0, frq );
  zPexCVal( dzTFNum(tf), &f, &n );
  zPexCVal( dzTFDen(tf), &f, &d );
  return zComplexCDiv( &n, &d, res );
}

static void *_dzTFNumFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzTF*)obj)->num = zPexFromZTK( ztk ) ) ? obj : NULL;
}
static void *_dzTFDenFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzTF*)obj)->den = zPexFromZTK( ztk ) ) ? obj : NULL;
}

static void _dzTFNumFPrintZTK(FILE *fp, int i, void *prp){
  zPexFPrint( fp, ((dzTF*)prp)->num );
}
static void _dzTFDenFPrintZTK(FILE *fp, int i, void *prp){
  zPexFPrint( fp, ((dzTF*)prp)->den );
}

static ZTKPrp __ztk_prp_dztf[] = {
  { "num", 1, _dzTFNumFromZTK, _dzTFNumFPrintZTK },
  { "den", 1, _dzTFDenFromZTK, _dzTFDenFPrintZTK },
};

bool dzTFRegZTK(ZTK *ztk, char *tag)
{
  return ZTKDefRegPrp( ztk, tag, __ztk_prp_dztf ) ? true : false;
}

dzTF *dzTFFromZTK(dzTF *tf, ZTK *ztk)
{
  dzTFSetNum( tf, NULL );
  dzTFSetDen( tf, NULL );
  if( !ZTKEncodeKey( tf, NULL, ztk, __ztk_prp_dztf ) ) return NULL;
  if( !dzTFNum(tf) || !dzTFDen(tf) ){
    dzTFDestroy( tf );
    return NULL;
  }
  return tf;
}

/* scan a ZTK file and create a transfer function. */
dzTF *dzTFScanZTK(dzTF *tf, char filename[])
{
  ZTK ztk;

  ZTKInit( &ztk );
  if( !dzTFRegZTK( &ztk, "" ) ) return NULL;
  if( ZTKParse( &ztk, filename ) )
    tf = dzTFFromZTK( tf, &ztk );
  ZTKDestroy( &ztk );
  return tf;
}

void dzTFFPrintZTK(FILE *fp, dzTF *tf)
{
  ZTKPrpKeyFPrint( fp, tf, __ztk_prp_dztf );
}

/* print a transfer function in a fancy style to a file. */
void dzTFFExpr(FILE *fp, dzTF *tf)
{
  zPexFExpr( fp, dzTFNum(tf), 's' );
  fprintf( fp, "------\n" );
  zPexFExpr( fp, dzTFDen(tf), 's' );
}
