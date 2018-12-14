#include <dzco/dz_sys.h>

#define N 100

bool assert_adder(dzSys *adder, double *v1, double *v2, double *v3)
{
  register int i;

  for( i=0; i<N; i++ ){
    *v1 = zRandF(-10,10);
    *v2 = zRandF(-10,10);
    *v3 = zRandF(-10,10);
    dzSysUpdate( adder, 1 );
    if( !zIsTiny( zVecElem(dzSysOutput(adder),0) - (*v1+*v2+*v3) ) ) return false;
  }
  return true;
}

bool assert_subtr(dzSys *subtr, double *v1, double *v2, double *v3)
{
  register int i;

  for( i=0; i<N; i++ ){
    *v1 = zRandF(-10,10);
    *v2 = zRandF(-10,10);
    *v3 = zRandF(-10,10);
    dzSysUpdate( subtr, 1 );
    if( !zIsTiny( zVecElem(dzSysOutput(subtr),0) - (*v1-*v2-*v3) ) ) return false;
  }
  return true;
}

bool assert_limiter(dzSys *limiter, double *v)
{
  register int i;

  for( i=0; i<N; i++ ){
    *v = zRandF(-2,2);
    dzSysUpdate( limiter, 1 );
    if( zVecElem(dzSysOutput(limiter),0) < -1 || zVecElem(dzSysOutput(limiter),0) > 1 ) return false;
  }
  return true;
}

int main(void)
{
  dzSys adder, subtr, limiter;
  double v1, v2, v3;

  zRandInit();

  dzSysCreateAdder( &adder, 3 );
  dzSysInputPtr(&adder,0) = &v1;
  dzSysInputPtr(&adder,1) = &v2;
  dzSysInputPtr(&adder,2) = &v3;

  dzSysCreateSubtr( &subtr, 3 );
  dzSysInputPtr(&subtr,0) = &v1;
  dzSysInputPtr(&subtr,1) = &v2;
  dzSysInputPtr(&subtr,2) = &v3;

  dzSysCreateLimit( &limiter, 1, -1 );
  dzSysInputPtr(&limiter,0) = &v1;

  zAssert( dzSysCreateAdder, assert_adder( &adder, &v1, &v2, &v3 ) );
  zAssert( dzSysCreateSubtr, assert_subtr( &subtr, &v1, &v2, &v3 ) );
  zAssert( dzSysCreateLimit, assert_limiter( &limiter, &v1 ) );

  dzSysDestroy( &adder );
  dzSysDestroy( &subtr );
  dzSysDestroy( &limiter );

  return EXIT_SUCCESS;
}
