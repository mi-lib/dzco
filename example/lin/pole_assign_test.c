#include <dzco/dz_lin.h>

void check(dzLin *c, zVec f, zVec pole)
{
  zMat tmp1, tmp2;
  register int i, j;
  double det;

  printf( ">> desired poles\n" );
  zVecWrite( pole );
  printf( ">> obtained feedback gains\n" );
  zVecWrite( f );

  tmp1 = zMatAllocSqr( dzLinDim(c) );
  tmp2 = zMatAllocSqr( dzLinDim(c) );
  zVecDyad( c->b, f, tmp1 );
  zMatSubDRC( tmp1, c->a );
  for( i=0; i<_zVecSize(pole); i++ ){
    zMatCopy( tmp1, tmp2 );
    for( j=0; j<_zMatRowSize(tmp2); j++ )
      zMatElem( tmp2, j, j ) += zVecElem( pole, i );
    det = zMatDet( tmp2 );
    printf( "%g : %g %s one of the eigen values.\n", det, zVecElem(pole,i), zIsTiny(det) ? "is" : "is not" );
  }
  zMatFree( tmp1 );
  zMatFree( tmp2 );
}

int main(void)
{
#define DIM 3
  double a[] = { 0, 1, 2,-5,-4, 1, 0, 0, 3 };
  double b[] = { 0,-1, 1 };
  double c[] = { 2, 1, 1 };
#if 1
  double p[] = { -1, -2, -3 };
#else
  double p[] = { 10, 20, 30 };
#endif
  dzLin sl;
  zVec f, pole;

  dzLinAlloc( &sl, DIM );
  zMatCopyArray( a, DIM, DIM, sl.a );
  zVecCopyArray( b, DIM, sl.b );
  zVecCopyArray( c, DIM, sl.c );
  sl.d = 0;

  pole = zVecCloneArray( p, DIM );
  f = zVecAlloc( DIM );

  dzLinPoleAssign( &sl, pole, f );
  check( &sl, f, pole );

  dzLinDestroy( &sl );
  zVecFree( pole );
  zVecFree( f );
  return 0;
}
