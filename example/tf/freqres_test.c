/* run this program as
 % ./freqres_test | gnuplot -p -
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
  printf( "plot [%g:%g] '%s' u 1:2 w l t \"gain\", '%s' u 1:4 w l t \"gain\"\n", FRQ_MIN, FRQ_MAX, filename, filename );
  printf( "set origin 0, 0\n" );
  printf( "plot [%g:%g] '%s' u 1:3 w l t \"phase lag\", '%s' u 1:5 w l t \"phase lag\"\n", FRQ_MIN, FRQ_MAX, filename, filename );
}

/* naive conversion from a transfer function to frequency response. */
dzFreqRes *naive_freq_res_from_tf(dzFreqRes *fr, dzTF *tf, double af)
{
  dzFreqRes fr_n, fr_d;
  zComplex n, d, caf;

  zComplexCreate( &caf, 0, af );
  zPexCVal( dzTFNum(tf), &caf, &n );
  dzFreqResFromComplex( &fr_n, &n, af );
  zPexCVal( dzTFDen(tf), &caf, &d );
  dzFreqResFromComplex( &fr_d, &d, af );

  if( !zIsEqual( fr_n.f, fr_d.f, zTOL ) )
    ZRUNWARN( DZ_WARN_TF_FR_NOTCORRESPOND, fr_n.f, fr_d.f );
  fr->f = fr_n.f;
  fr->g = fr_n.g - fr_d.g;
  fr->p = fr_n.p - fr_d.p;
  return fr;
}

int main(int argc, char *argv[])
{
  dzTF g;
  zCVec zero, pole;
  double frq;
  dzFreqRes fr1, fr2;
  FILE *fp;

  zero = zCVecAlloc( 2 );
  pole = zCVecAlloc( 7 );
  zComplexCreate( zCVecElemNC(zero,0), -1, 5 );
  zComplexCreate( zCVecElemNC(zero,1), -1,-5 );
  zComplexCreate( zCVecElemNC(pole,0), -1, 0 );
  zComplexCreate( zCVecElemNC(pole,1), -5, 3 );
  zComplexCreate( zCVecElemNC(pole,2), -5,-3 );
  zComplexCreate( zCVecElemNC(pole,3),-10, 7 );
  zComplexCreate( zCVecElemNC(pole,4),-10,-7 );
  zComplexCreate( zCVecElemNC(pole,5),-50, 0 );
  zComplexCreate( zCVecElemNC(pole,6),-50, 0 );
  dzTFCreateZeroPole( &g, zero, pole, 1 );
  zCVecFree( zero );
  zCVecFree( pole );

  fp = fopen( OUTPUT_FILE, "w" );
  for( frq=FRQ_MIN; frq<FRQ_MAX; frq*=1.2 ){
    dzFreqResFromTF( &fr1, &g, frq );
    naive_freq_res_from_tf( &fr2, &g, frq );
    fprintf( fp, "%f %f %f %f %f\n", frq, fr1.g, zRad2Deg(fr1.p), fr2.g, zRad2Deg(fr2.p) );
  }
  fclose( fp );
  dzTFDestroy( &g );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
