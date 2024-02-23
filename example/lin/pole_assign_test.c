#include <dzco/dz_lin.h>

#define DIM 5
#define TOL 1.0e-8

void check(dzLin *c, zVec f, zVec pole)
{
  zMat tmp1, tmp2;
  register int i, j;
  double det;
  int count;

  tmp1 = zMatAllocSqr( dzLinDim(c) );
  tmp2 = zMatAllocSqr( dzLinDim(c) );
  zVecDyad( c->b, f, tmp1 );
  zMatSubDRC( tmp1, c->a );
  for( count=0, i=0; i<zVecSizeNC(pole); i++ ){
    zMatCopy( tmp1, tmp2 );
    for( j=0; j<zMatRowSizeNC(tmp2); j++ )
      zMatElemNC(tmp2,j,j) += zVecElemNC(pole,i);
    if( zIsTol( ( det = zMatDet( tmp2 ) ), TOL ) ){
      count++;
    } else{
      printf( "found non-zero determinant %g.\n", det );
    }
  }
  printf( "%d/%d eigen values confirmed.\n", count, zVecSizeNC(pole) );
  zMatFree( tmp1 );
  zMatFree( tmp2 );
}

int main(void)
{
  dzLin sl;
  zVec f, pole;
  int dim = DIM;

  zRandInit();
  dzLinAlloc( &sl, dim );
  pole = zVecAlloc( dim );
  zMatRandUniform( sl.a, -10.0, 10.0 );
  zVecRandUniform( sl.b, -10.0, 10.0 );
  zVecRandUniform( sl.c, -10.0, 10.0 );
  sl.d = 0;
  zVecRandUniform( pole, -10.0, 10.0 );

  f = zVecAlloc( dim );

  dzLinPoleAssign( &sl, pole, f );
  check( &sl, f, pole );

  dzLinDestroy( &sl );
  zVecFree( pole );
  zVecFree( f );
  return 0;
}
