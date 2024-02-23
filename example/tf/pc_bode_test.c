/* run this program as
 % ./pc_bode_test | gnuplot -p -
 */

#include <dzco/dz_tf.h>

#define FRQ_MIN 1.0e-3
#define FRQ_MAX 1.0e4

#define OUTPUT_FILE "bode_result.dat"

void output_plotscript(const char *filename)
{
  printf( "clear\n" );
  printf( "set multiplot\n" );
  printf( "set logscale x\n" );
  printf( "set grid\n" );
  printf( "set size 1.0, 0.5\n" );
  printf( "set origin 0, 0.5\n" );
  printf( "plot [%g:%g] '%s' u 1:2 w l t \"gain\"\n", FRQ_MIN, FRQ_MAX, filename );
  printf( "set origin 0, 0\n" );
  printf( "plot [%g:%g] '%s' u 1:3 w l t \"phase lag\"\n", FRQ_MIN, FRQ_MAX, filename );
}

int main(int argc, char *argv[])
{
  dzTF g;
  double frq;
  dzFreqRes fr;
  FILE *fp;

  dzTFAlloc( &g, 1, 1 );
  dzTFSetNumList( &g, 1.0, 1.0 );
  dzTFSetDenList( &g, 1.0, 1.2 );
  fp = fopen( OUTPUT_FILE, "w" );
  for( frq=FRQ_MIN; frq<=FRQ_MAX; frq*=1.2 ){
    dzFreqResFromTF( &fr, &g, frq );
    fprintf( fp, "%f %f %f\n", frq, fr.g, zRad2Deg(fr.p) );
  }
  fclose( fp );
  dzTFDestroy( &g );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
