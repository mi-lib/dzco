#include <dzco/dz_pex.h>

#define N 100000
#define FREQ_MAX 1000.0

int main(int argc, char *argv[])
{
  dzPex g;
  double frq;
  zComplex c;
  FILE *fp[2];
  register int i;

  fp[0] = fopen( "bode", "w" );
  fp[1] = fopen( "nyquist", "w" );

  dzPexAlloc( &g, 0, 2 );
  dzPexSetNumList( &g, 3.0 );
  dzPexSetDenList( &g, 3.0, 1.0, 1.0 );

  for( i=-N; i<=N; i++ ){
    frq = FREQ_MAX * i / N;
    dzPexFreqRes( &g, frq, &c );
    fprintf( fp[0], "%f %f %f\n", frq,
      log(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
    fprintf( fp[1], "%f %f\n", c.re, c.im );
  }
  dzPexDestroy( &g );
  fclose( fp[0] );
  fclose( fp[1] );
  return 0;
}
