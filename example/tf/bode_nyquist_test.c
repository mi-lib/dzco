/* run this program as
 % ./bode_nyquist_test | gnuplot -p -
 */

#include <dzco/dz_tf.h>

#define N 100000
#define FRQ_MAX 1.0e3

#define OUTPUT_FILE "result.dat"

void output_plotscript(const char *filename)
{
  printf( "clear\n" );
  printf( "set multiplot\n" );
  printf( "set logscale x\n" );
  printf( "set grid\n" );
  printf( "set size 0.5, 0.5\n" );
  printf( "set origin 0, 0.5\n" );
  printf( "plot [1.0e-3:%g] '%s' u 1:4 w l t \"gain\"\n", FRQ_MAX, filename );
  printf( "set origin 0, 0\n" );
  printf( "plot [1.0e-3:%g] '%s' u 1:5 w l t \"phase lag\"\n", FRQ_MAX, filename );
  printf( "unset logscale\n" );
  printf( "set size 0.5, 1.0\n" );
  printf( "set origin 0.5, 0\n" );
  printf( "plot '%s' u 2:3 w l t \"Nyquist\"\n", filename );
}

int main(int argc, char *argv[])
{
  dzTF g;
  double frq;
  zComplex c;
  FILE *fp;
  int i;

  dzTFAlloc( &g, 0, 2 );
  dzTFSetNumList( &g, 3.0 );
  dzTFSetDenList( &g, 3.0, 1.0, 1.0 );

  fp = fopen( OUTPUT_FILE, "w" );
  for( i=-N; i<=N; i++ ){
    frq = FRQ_MAX * i / N;
    dzTFToComplex( &g, frq, &c );
    fprintf( fp, "%f %f %f %f %f\n", frq, c.re, c.im, 20*log10(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
  }
  fclose( fp );
  dzTFDestroy( &g );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
