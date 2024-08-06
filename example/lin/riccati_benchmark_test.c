#include <dzco/dz_lin.h>

void alloc_sample(dzLin *lin, zMat *q, zMat *p, zMat *res, int n, double aarray[], double barray[], double qarray[], double ansarray[])
{
  dzLinAlloc( lin, n );
  *p = zMatAllocSqr( n );
  zMatCopyArray( aarray, n, n, lin->a );
  zVecCopyArray( barray, n, lin->b );
  *q = zMatCloneArray( qarray, n, n );
  *res = zMatAllocSqr( n );
  /* ansarray is not used */
}

void gen_sample1(dzLin *lin, zMat *q, double *r, zMat *p, zMat *res)
{
  double aarray[] = {
    0, 1,
    0, 0, };
  double barray[] = {
    0, 1, };
  double qarray[] = {
    1, 0,
    0, 1, };
  double rval = 1.0;
  double ansarray[] = {
    1.73205, 1.0,
    1.0,     1.73205, };
  int n = 2;
  alloc_sample( lin, q, p, res, n, aarray, barray, qarray, ansarray );
  *r = rval;
}

void gen_sample2(dzLin *lin, zMat *q, double *r, zMat *p, zMat *res)
{
  double aarray[] = {
    0, 1,
    0, 0, };
  double barray[] = {
    0, 1, };
  double qarray[] = {
    4, 0,
    0, 0, };
  double rval = 1.0;
  double ansarray[] = {
    4, 2,
    2, 2, };
  int n = 2;
  alloc_sample( lin, q, p, res, n, aarray, barray, qarray, ansarray );
  *r = rval;
}

void gen_sample3(dzLin *lin, zMat *q, double *r, zMat *p, zMat *res)
{
  double aarray[] = {
    1, 0,
    0,-2, };
  double barray[] = {
    1, 0, };
  double qarray[] = {
    1, 1,
    1, 1, };
  double rval = 1.0;
  double ansarray[] = {
    2.414213562, 0.2928932188,
    0.2928932188, 0.2285533906, };
  int n = 2;
  alloc_sample( lin, q, p, res, n, aarray, barray, qarray, ansarray );
  *r = rval;
}

void gen_sample4(dzLin *lin, zMat *q, double *r, zMat *p, zMat *res)
{
  double aarray[] = {
    0, 1,
    0,-1, };
  double barray[] = {
    0, 1, };
  double qarray[] = {
   25, 0,
    0,25, };
  double rval = 1.0;
  double ansarray[] = {
   30, 5,
    5, 5, };
  int n = 2;
  alloc_sample( lin, q, p, res, n, aarray, barray, qarray, ansarray );
  *r = rval;
};

void (* gen_sample[])(dzLin*,zMat*,double*,zMat*,zMat*) = {
  gen_sample1,
  gen_sample2,
  gen_sample3,
  gen_sample4,
  NULL,
};


int main(void)
{
  dzLin lin;
  zMat q, p, res;
  double r;
  int i;
  bool result_euler, result_kleinman;

  for( i=0; gen_sample[i]; i++ ){
    gen_sample[i]( &lin, &q, &r, &p, &res );
    /* Euler's method */
    dzLinRiccatiSolveEuler( p, &lin, q, r, 0.01*zTOL, 0 );
    dzLinRiccatiError( p, &lin, q, r, res );
    result_euler = zMatIsTiny( res );
    /* Kleinman's method */
    dzLinRiccatiSolveKleinman( p, NULL, &lin, q, r, zTOL, 0 );
    dzLinRiccatiError( p, &lin, q, r, res );
    result_kleinman = zMatIsTiny( res );
    printf( "case #%d: Euler=%s, Kleinman=%s\n", i, zBoolStr(result_euler), zBoolStr(result_kleinman) );
    dzLinDestroy( &lin );
    zMatFreeAtOnce( 3, q, p, res );
  }
  return 0;
}
