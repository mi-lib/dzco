#include <dzco/dz_lin.h>

#define TEST 4

int main(void)
{
#if TEST == 1
  /* answer:
     1.73205 1.0
     1.0     1.73205
   */
  double aarray[] = {
    0, 1,
    0, 0, };
  double barray[] = {
    0, 1, };
  double qarray[] = {
    1, 0,
    0, 1, };
  double r = 1.0;
  int n = 2;
#elif TEST == 2
  /* answer:
     4 2
     2 2
   */
  double aarray[] = {
    0, 1,
    0, 0, };
  double barray[] = {
    0, 1, };
  double qarray[] = {
    4, 0,
    0, 0, };
  double r = 1.0;
  int n = 2;
#elif TEST == 3
  /* answer:
     2.414213562 0.2928932188
     0.2928932188 0.2285533906
   */
  double aarray[] = {
    1, 0,
    0,-2, };
  double barray[] = {
    1, 0, };
  double qarray[] = {
    1, 1,
    1, 1, };
  double r = 1.0;
  int n = 2;
#else
  /* answer:
     30 5
      5 5
   */
  double aarray[] = {
    0, 1,
    0,-1, };
  double barray[] = {
    0, 1, };
  double qarray[] = {
   25, 0,
    0,25, };
  double r = 1.0;
  int n = 2;
#endif
  dzLin lin;
  zMat p, q, res;

  dzLinAlloc( &lin, n );
  p = zMatAllocSqr( n );
  zMatCopyArray( aarray, n, n, lin.a );
  zVecCopyArray( barray, n, lin.b );
  q = zMatCloneArray( qarray, n, n );

  dzLinRiccatiSolveEuler( p, &lin, q, r, zTOL, 0 );
  zMatPrint( p );

  printf( "(error matrix)\n" );
  res = zMatAllocSqr( n );
  dzLinRiccatiError( p, &lin, q, r, res );
  zMatPrint( res );
  zMatFree( res );

  dzLinDestroy( &lin );
  zMatFree( p );
  zMatFree( q );
  return 0;
}
