/* state feedbakc test (following Nakano and Mita's text) */
#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 4000

dzLin *create_sl(void)
{
#define DIM 2
  double a[] = {-1, 1,-4,-1 };
  double b[] = { 0, 1 };
  double c[] = { 1, 0 };
  dzLin *sl;

  sl = zAlloc( dzLin, 1 );
  dzLinAlloc( sl, DIM );
  zMatCopyArray( a, DIM, DIM, sl->a );
  zVecCopyArray( b, DIM, sl->b );
  zVecCopyArray( c, DIM, sl->c );
  sl->d = 0;
  return sl;
}

int main(void)
{
  dzLin *sl1, *sl2;
  dzSys s1, s2;
  double input1, input2;
  zVec f, pole;
  double p[] = {-6,-7 };
  register int i;

  sl1 = create_sl();
  sl2 = create_sl();
  pole = zVecCloneArray( p, DIM );
  f = zVecAlloc( DIM );
  dzLinPoleAssign( sl2, pole, f );
  dzSysLinCreate( &s1, sl1 );
  dzSysLinCreate( &s2, sl2 );
  dzSysInputPtr(&s1,0) = &input1;
  dzSysInputPtr(&s2,0) = &input2;
  zVecSetElemList( sl1->x, 1.0,-1.0 );
  zVecSetElemList( sl2->x, 1.0,-1.0 );

  input1 = input2 = 0;
  for( i=0; i<=STEP; i++ ){
    printf( "%f %f %f %f\n", zVecElem(sl1->x,0), zVecElem(sl1->x,1), zVecElem(sl2->x,0), zVecElem(sl2->x,1) );
    input2 = dzLinStateFeedback( sl2, NULL, f );
    dzSysUpdate( &s1, DT );
    dzSysUpdate( &s2, DT );
  }

  dzSysDestroy( &s1 );
  dzSysDestroy( &s2 );
  zVecFree( pole );
  zVecFree( f );
  return 0;
}
