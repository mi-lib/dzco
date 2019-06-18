/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_pex - polynomial rational expression of transfer functions
 */

#include <dzco/dz_pex.h>

/* ********************************************************** */
/* CLASS: dzPex
 * general polynomial transfer function
 * ********************************************************** */

static bool _dzPexFScan(FILE *fp, void *instance, char *buf, bool *success);

/* allocate memory of a polynomial rational transfer function. */
bool dzPexAlloc(dzPex *pex, int nsize, int dsize)
{
  dzPexSetNum( pex, zPexAlloc( nsize ) );
  dzPexSetDen( pex, zPexAlloc( dsize ) );
  if( !dzPexNum(pex) || !dzPexDen(pex) ){
    ZRUNERROR( "cannot create a polynomial transfer function" );
    dzPexDestroy( pex );
    return false;
  }
  return true;
}

/* create a polynomial rational transfer function from zeros and poles. */
bool dzPexCreateZeroPole(dzPex *pex, zVec zero, zVec pole)
{
  dzPexSetNum( pex, zPexExp( zero ) );
  dzPexSetDen( pex, zPexExp( pole ) );
  if( !dzPexNum(pex) || !dzPexDen(pex) ){
    ZRUNERROR( "cannot create a transfer function" );
    dzPexDestroy( pex );
    return false;
  }
  return true;
}

/* destroy a polynomial rational transfer function. */
void dzPexDestroy(dzPex *pex)
{
  zPexFree( dzPexNum( pex ) );
  zPexFree( dzPexDen( pex ) );
}

/* set coefficients of numerator of a transfer function. */
void dzPexSetNumList(dzPex *pex, ...)
{
  va_list args;

  va_start( args, pex );
  zVecSetElemVList( dzPexNum(pex), args );
  va_end( args );
}

/* set coefficients of denominator of a transfer function. */
void dzPexSetDenList(dzPex *pex, ...)
{
  va_list args;

  va_start( args, pex );
  zVecSetElemVList( dzPexDen(pex), args );
  va_end( args );
}

/* check if a polynomial rational transfer function is stable. */
static int __next(int i);
int __next(int i){ return i==2 ? 0 : i+1; }
bool dzPexIsStable(dzPex *pex)
{
  zVec v[3];
  register uint i, j, k, m, n;
  bool result = false;

  /* if the dimension of numerator is equal to or larger
     than that of denominator, or the denominator is at least
     not thoroughly positive, the system is unstable. */
  if( dzPexDenDim(pex) <= dzPexNumDim(pex) ) return false;
  for( i=0; i<zVecSizeNC(dzPexDen(pex)); i++ )
    if( zVecElemNC( dzPexDen(pex), i ) <= 0 ) return false;

  /* preparation of the initial tableau */
  i = dzPexDenDim( pex );
  n = ( i + 2 - i % 2 ) / 2;
  v[0] = zVecAlloc( n );
  v[1] = zVecAlloc( n );
  v[2] = zVecAlloc( n );
  if( !v[0] || !v[1] || !v[2] ){
    ZRUNERROR( "cannot check stability" );
    goto TERMINATE;
  }
  for( j=i, k=0; j>=0; j-=2, k++ ){
    zVecSetElemNC( v[0], k, zPexCoeff( dzPexDen(pex), j ) );
    zVecSetElemNC( v[1], k, j==0 ? 0 : zPexCoeff( dzPexDen(pex), j-1 ) );
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
zComplex *dzPexFreqRes(dzPex *pex, double frq, zComplex *res)
{
  zComplex n, d, f;

  zComplexCreate( &f, 0, frq );
  zPexCVal( dzPexNum(pex), &f, &n );
  zPexCVal( dzPexDen(pex), &f, &d );
  return zComplexCDiv( &n, &d, res );
}

/* (static)
 * scan a polynomial transfer function from a file. */
bool _dzPexFScan(FILE *fp, void *instance, char *buf, bool *success)
{
  if( strcmp( buf, "num" ) == 0 ){
    dzPexNum((dzPex *)instance) = zPexFScan( fp );
  } else
  if( strcmp( buf, "den" ) == 0 ){
    dzPexDen((dzPex *)instance) = zPexFScan( fp );
  } else
    return false;
  return true;
}

/* scan a polynomial transfer function from a file. */
dzPex *dzPexFScan(FILE *fp, dzPex *pex)
{
  dzPexSetNum( pex, NULL );
  dzPexSetDen( pex, NULL );
  zFieldFScan( fp, _dzPexFScan, pex );
  if( !dzPexNum(pex) || !dzPexDen(pex) ){
    ZALLOCERROR();
    dzPexDestroy( pex );
    return NULL;
  }
  return pex;
}

/* print a polynomial transfer function to a file. */
void dzPexFPrint(FILE *fp, dzPex *pex)
{
  fprintf( fp, "num: " );
  zPexFPrint( fp, dzPexNum(pex) );
  fprintf( fp, "den: " );
  zPexFPrint( fp, dzPexDen(pex) );
}

/* print a polynomial transfer function in a fancy style to a file. */
void dzPexFExpr(FILE *fp, dzPex *pex)
{
  zPexFExpr( fp, dzPexNum(pex), 's' );
  fprintf( fp, "------\n" );
  zPexFExpr( fp, dzPexDen(pex), 's' );
}
