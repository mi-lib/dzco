#include <dzco/dz_tf.h>

#define N 100000
#define FREQ_MAX 1000.0

int main(int argc, char *argv[])
{
  dzTF g;
  double frq;
  zComplex c;
  FILE *fp[2];
  register int i;

  fp[0] = fopen( "bode", "w" );
  fp[1] = fopen( "nyquist", "w" );

  dzTFAlloc( &g, 0, 2 );
  dzTFSetNumList( &g, 3.0 );
  dzTFSetDenList( &g, 3.0, 1.0, 1.0 );

  for( i=-N; i<=N; i++ ){
    frq = FREQ_MAX * i / N;
    dzTFToComplex( &g, frq, &c );
    fprintf( fp[0], "%f %f %f\n", frq,
      20*log10(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
    fprintf( fp[1], "%f %f\n", c.re, c.im );
  }
  dzTFDestroy( &g );
  fclose( fp[0] );
  fclose( fp[1] );
  return 0;
}
