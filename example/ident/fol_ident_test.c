#include <dzco/dz_sys.h>
#include <dzco/dz_ident_lag.h>

#define DT 0.01
#define N 500
double t[N], r[N], y[N];

void sample(dzSys *fol, int trig)
{
  register int i;

  zRandInit();
  for( i=0; i<N; i++ ){
    t[i] = DT*i;
    if( i > trig ){
      dzSysUpdate( fol, DT );
      r[i] = dzSysInputVal(fol,0);
      y[i] = dzSysOutputVal(fol,0) + zRandF(-0.1,0.1);
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
    printf( "%f %f %f %f %f\n", DT*i, dzSysInputVal(org,0), dzSysOutputVal(org,0), y[i], dzSysOutputVal(isys,0) );
  }
}

int main(int argc, char *argv[])
{
  dzSys fol, isys;
  double ref, k, tc;

  dzSysFOLCreate( &fol, 0.5, 2.0 );
  dzSysInputPtr(&fol,0) = &ref;
  ref = argc > 1 ? atof( argv[1] ) : 1;
  sample( &fol, 200 );
  dzIdentFOL( t, r, y, N, dzIdentTrig(r,N), &tc, &k );
  dzSysFOLCreate( &isys, tc, k );
  dzSysInputPtr(&isys,0) = &ref;
  dzSysRefresh( &fol );
  test( &fol, &isys );
  dzSysDestroy( &fol );
  dzSysDestroy( &isys );
  return 0;
}
