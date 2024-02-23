/* run this program as
 % ./fol_ident_test | gnuplot -p -
 */

#include <dzco/dz_sys.h>
#include <dzco/dz_ident_lag.h>

#define DT    0.01
#define N   500
#define LAG 200
double t[N], r[N], y[N];

#define OUTPUT_FILE "fol_result.dat"

void sample(dzSys *fol, int trig)
{
  int i;

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

void test(const char *filename, dzSys *org, dzSys *isys)
{
  FILE *fp;
  int i;

  fp = fopen( filename, "w" );
  for( i=LAG; i<N; i++ ){
    dzSysUpdate( org, DT );
    dzSysUpdate( isys, DT );
    fprintf( fp, "%f %f %f %f\n", DT*i, dzSysOutputVal(org,0), y[i], dzSysOutputVal(isys,0) );
  }
  fclose( fp );
}

void output_plotscript(const char *filename)
{
  printf( "set grid\n" );
  printf( "plot '%s' u 1:2 w l t \"original system\", '%s' u 1:3 w p t \"samples\", '%s' u 1:4 w l t \"identified system\"\n", filename, filename, filename );
}

int main(int argc, char *argv[])
{
  dzSys fol, isys;
  double ref, k, tc;

  zRandInit();
  dzSysFOLCreate( &fol, 0.5, 2.0 );
  dzSysInputPtr(&fol,0) = &ref;
  ref = argc > 1 ? atof( argv[1] ) : 1;
  sample( &fol, LAG );
  dzIdentFOL( t, r, y, N, dzIdentTrig(r,N), &tc, &k );
  dzSysFOLCreate( &isys, tc, k );
  dzSysInputPtr(&isys,0) = &ref;
  dzSysRefresh( &fol );
  test( OUTPUT_FILE, &fol, &isys );
  dzSysDestroy( &fol );
  dzSysDestroy( &isys );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
