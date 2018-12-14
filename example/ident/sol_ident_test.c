#include <dzco/dz_sys.h>
#include <dzco/dz_ident_lag.h>

#define DT 0.01
#define N 500
double t[N], r[N], y[N];

void sample(dzSys *sol, int trig)
{
  register int i;

  for( i=0; i<N; i++ ){
    t[i] = DT*i;
    if( i > trig ){
      dzSysUpdate( sol, DT );
      r[i] = dzSysInputVal(sol,0);
      y[i] = dzSysOutputVal(sol,0);
    } else
      r[i] = y[i] = 0;
  }
}

void test(dzSys *org, dzSys *isys)
{
  register int i;

  for( i=0; i<N; i++ ){
    dzSysUpdate( org, DT );
    dzSysUpdate( isys, DT );
    printf( "%f %f %f %f\n", DT*i, dzSysInputVal(org,0), dzSysOutputVal(org,0), dzSysOutputVal(isys,0) );
  }
}

int main(int argc, char *argv[])
{
  dzSys sol, isys;
  double ref = 1.0, k, tc, z;

  z = argc > 1 ? atof( argv[1] ) : 0.7;
  dzSysCreateSOL( &sol, 0.5, 0.0, z, 2.0 );
  dzSysInputPtr(&sol,0) = &ref;
  sample( &sol, 200 );
  dzIdentSOL( t, r, y, N, dzIdentTrig(r,N), &tc, &z, &k );
  dzSysCreateSOL( &isys, tc, 0.0, z, k );
  dzSysRefresh( &sol );
  dzSysInputPtr(&isys,0) = &ref;
  test( &sol, &isys );
  dzSysDestroy( &isys );
  dzSysDestroy( &sol );
  return 0;
}
