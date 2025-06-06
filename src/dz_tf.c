/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_tf - transfer function by polynomial rational expression
 */

#include <dzco/dz_tf.h>

/* allocate memory of a polynomial rational transfer function. */
bool dzTFAlloc(dzTF *tf, int nsize, int dsize)
{
  dzTFInit( tf );
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
bool dzTFCreateZeroPole(dzTF *tf, zCVec zero, zCVec pole, double gain)
{
  dzTFInit( tf );
  dzTFZero(tf) = zCVecClone( zero );
  dzTFPole(tf) = zCVecClone( pole );
  dzTFSetNum( tf, zPexCExp( zero ) );
  dzTFSetDen( tf, zPexCExp( pole ) );
  if( !dzTFNum(tf) || !dzTFDen(tf) || !dzTFZero(tf) || !dzTFPole(tf) ){
    ZRUNERROR( DZ_ERR_TF_UNABLE_CREATE );
    dzTFDestroy( tf );
    return false;
  }
  zVecMulDRC( dzTFNum(tf), gain );
  return true;
}

/* destroy a polynomial rational transfer function. */
void dzTFDestroy(dzTF *tf)
{
  zPexFree( dzTFNum(tf) );
  zPexFree( dzTFDen(tf) );
  zCVecFree( dzTFZero(tf) );
  zCVecFree( dzTFPole(tf) );
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

static zPex _dzTFConnectPex(zPex *tfpex, zPex ctfpex)
{
  zPex pex;

  if( *tfpex ){
    pex = zPexMul( *tfpex, ctfpex );
    zPexFree( *tfpex );
  } else{
    pex = zPexClone( ctfpex );
  }
  return ( *tfpex = pex );
}

/* connect a transfer function. */
dzTF *dzTFConnect(dzTF *tf, dzTF *ctf)
{
  /* replace numerator */
  _dzTFConnectPex( &dzTFNum(tf), dzTFNum(ctf) );
  /* replace denominator */
  if( !_dzTFConnectPex( &dzTFDen(tf), dzTFDen(ctf) ) ) return NULL;
  if( !dzTFNum(tf) || !dzTFDen(tf) ){
    ZRUNERROR( DZ_ERR_TF_UNABLE_CREATE );
    dzTFDestroy( tf );
    return NULL;
  }
  return tf;
}

/* check if a polynomial rational transfer function is stable. */
static int __next(int i);
int __next(int i){ return i==2 ? 0 : i+1; }
bool dzTFIsStable(dzTF *tf)
{
  zVec v[3];
  int i, j, k, m, n;
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

/* abstract zeros and poles of a transfer function. */
bool dzTFZeroPole(dzTF *tf)
{
  if( dzTFZero(tf) ) zCVecFree( dzTFZero(tf) );
  if( dzTFPole(tf) ) zCVecFree( dzTFPole(tf) );
  if( dzTFNumDim(tf) > 0 ){
    if( !( dzTFZero(tf) = zCVecAlloc( dzTFNumDim(tf) ) ) )
      return false;
    if( !zPexDKA( dzTFNum(tf), dzTFZero(tf), ZM_PEX_EQ_TOL, 0 ) )
      return false;
  }
  if( dzTFDenDim(tf) > 0 ){
    if( !( dzTFPole(tf) = zCVecAlloc( dzTFDenDim(tf) ) ) ){
      zCVecFree( dzTFZero(tf) );
      return false;
    }
    if( !zPexDKA( dzTFDen(tf), dzTFPole(tf), ZM_PEX_EQ_TOL, 0 ) )
      return false;
  }
  return true;
}

/* abstract zeros and poles of a transfer function into real and imaginary values. */
bool dzTFZeroPoleReIm(dzTF *tf, zVec *zero1, zCVec *zero2, zVec *pole1, zCVec *pole2)
{
  if( !dzTFZeroPole( tf ) ) return false;
  return zCVecToReImVec( dzTFZero(tf), zero1, zero2, ZM_PEX_EQ_TOL ) &&
         zCVecToReImVec( dzTFPole(tf), pole1, pole2, ZM_PEX_EQ_TOL ) ? true : false;
}

static bool _dzTFFactorFromZTK(zPex *pex, ZTK *ztk){
  zCVec factor;
  zPex newpex;
  int i;
  bool ret = true;

  if( !( factor = zCVecAlloc( zListSize(&ztk->kf_cp->data.vallist) ) ) ) return false;
  for( i=0; i<zCVecSizeNC(factor); i++, ZTKValNext(ztk) )
    zComplexFromZTK( zCVecElemNC(factor,i), ztk );
  if( !( newpex = zPexCExp( factor ) ) )
    ret = false;
  else
    _dzTFConnectPex( pex, newpex );
  zPexFree( newpex );
  zCVecFree( factor );
  return ret;
}

static void *_dzTFNumFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzTF*)obj)->num = zPexFromZTK( ztk ) ) ? obj : NULL;
}
static void *_dzTFDenFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return ( ((dzTF*)obj)->den = zPexFromZTK( ztk ) ) ? obj : NULL;
}
static void *_dzTFZeroFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return _dzTFFactorFromZTK( &dzTFNum((dzTF*)obj), ztk ) ? obj : NULL;
}
static void *_dzTFPoleFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  return _dzTFFactorFromZTK( &dzTFDen((dzTF*)obj), ztk ) ? obj : NULL;
}
static void *_dzTFGainFromZTK(void *obj, int i, void *arg, ZTK *ztk){
  if( dzTFNum((dzTF*)obj) )
    zVecMulDRC( dzTFNum((dzTF*)obj), ZTKDouble(ztk) );
  else{
    if( !dzTFSetNum( (dzTF*)obj, zPexAlloc( 0 ) ) ) return NULL;
    dzTFSetNumElem( (dzTF*)obj, 0, ZTKDouble(ztk) );
  }
  return obj;
}

static bool _dzTFNumFPrintZTK(FILE *fp, int i, void *prp){
  zPexFPrint( fp, ((dzTF*)prp)->num );
  return true;
}
static bool _dzTFDenFPrintZTK(FILE *fp, int i, void *prp){
  zPexFPrint( fp, ((dzTF*)prp)->den );
  return true;
}

static const ZTKPrp __ztk_prp_dztf[] = {
  { ZTK_KEY_DZCO_TF_NUMERATOR, 1, _dzTFNumFromZTK, _dzTFNumFPrintZTK },
  { ZTK_KEY_DZCO_TF_DENOMINATOR, 1, _dzTFDenFromZTK, _dzTFDenFPrintZTK },
  { ZTK_KEY_DZCO_TF_ZERO, 1, _dzTFZeroFromZTK, NULL },
  { ZTK_KEY_DZCO_TF_POLE, 1, _dzTFPoleFromZTK, NULL },
  { ZTK_KEY_DZCO_TF_GAIN, 1, _dzTFGainFromZTK, NULL },
};

dzTF *dzTFFromZTK(dzTF *tf, ZTK *ztk)
{
  dzTFInit( tf );
  dzTFSetNum( tf, NULL );
  dzTFSetDen( tf, NULL );
  if( !_ZTKEvalKey( tf, NULL, ztk, __ztk_prp_dztf ) ) return NULL;
  if( !dzTFNum(tf) || !dzTFDen(tf) ){
    dzTFDestroy( tf );
    return NULL;
  }
  return tf;
}

void dzTFFPrintZTK(FILE *fp, dzTF *tf)
{
  _ZTKPrpKeyFPrint( fp, tf, __ztk_prp_dztf );
}

/* read a transfer function from a ZTK file. */
dzTF *dzTFReadZTK(dzTF *tf, char filename[])
{
  ZTK ztk;

  ZTKInit( &ztk );
  if( ZTKParse( &ztk, filename ) )
    tf = dzTFFromZTK( tf, &ztk );
  ZTKDestroy( &ztk );
  return tf;
}

/* write a transfer function to a ZTK file. */
bool dzTFWriteZTK(dzTF *tf, char filename[])
{
  FILE *fp;

  if( !( fp = zOpenZTKFile( filename, "w" ) ) ) return false;
  dzTFFPrintZTK( fp, tf );
  fclose(fp);
  return true;
}

/* print a transfer function in a fancy style to a file. */
void dzTFFExpr(FILE *fp, dzTF *tf)
{
  char buf_num[BUFSIZ], buf_den[BUFSIZ];
  int i;

  zPexSExpr( buf_num, BUFSIZ, dzTFNum(tf), 's' );
  zPexSExpr( buf_den, BUFSIZ, dzTFDen(tf), 's' );
  fprintf( fp, "%s\n", buf_num );
  for( i=zMax(strlen(buf_num),strlen(buf_den)); i>0; i-- )
    fputc( '-', fp );
  fprintf( fp, "\n" );
  fprintf( fp, "%s\n", buf_den );
}
