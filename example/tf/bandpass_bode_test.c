/* run this program as
 % ./bandpass_bode_test | gnuplot -p -
 */

#include <dzco/dz_tf.h>

#define FRQ_MIN 1.0e-3
#define FRQ_MAX 1.0e4

#define T 1.0

#define OUTPUT_FILE "bode_result.dat"

void output_plotscript(const char *filename)
{
  printf( "clear\n" );
  printf( "set multiplot\n" );
  printf( "set logscale x\n" );
  printf( "set grid\n" );
  printf( "set size 1.0, 0.5\n" );
  printf( "set origin 0, 0.5\n" );
  printf( "plot [%g:%g] '%s' u 1:2 w l t \"low-pass filter gain\", '%s' u 1:4 w l t \"high-pass filter gain\"\n", FRQ_MIN, FRQ_MAX, filename, filename );
  printf( "set origin 0, 0\n" );
  printf( "plot [%g:%g] '%s' u 1:3 w l t \"low-pass filter phase lag\", '%s' u 1:5 w l t \"high-pass filter phase lag\"\n", FRQ_MIN, FRQ_MAX, filename, filename );
}

int main(int argc, char *argv[])
{
  dzTF lp, hp;
  double frq;
  dzFreqRes fr_l, fr_h;
  FILE *fp;

  /* low-pass filter */
  dzTFAlloc( &lp, 0, 2 );
  dzTFSetNumList( &lp, 1.0 );
  dzTFSetDenList( &lp, 1.0, 2*T, T*T );
  /* high-pass filter */
  dzTFAlloc( &hp, 2, 2 );
  dzTFSetNumList( &hp, 0.0, 2*T, T*T );
  dzTFSetDenList( &hp, 1.0, 2*T, T*T );

  fp = fopen( OUTPUT_FILE, "w" );
  for( frq=FRQ_MIN; frq<FRQ_MAX; frq*=1.2 ){
    dzFreqResFromTF( &fr_l, &lp, frq );
    dzFreqResFromTF( &fr_h, &hp, frq );
    fprintf( fp, "%f %f %f %f %f\n", frq, fr_l.g, fr_l.p, fr_h.g, fr_h.p );
  }
  fclose( fp );
  dzTFDestroy( &lp );
  dzTFDestroy( &hp );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
