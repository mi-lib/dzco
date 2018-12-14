#include <dzco/dz_pex.h>

#define DIM 2

#define T 1.0

int main(int argc, char *argv[])
{
  dzPex lp, hp;
  double frq;
  zComplex c;
  FILE *fp[2];

  fp[0] = fopen( "lp_bode", "w" );
  fp[1] = fopen( "hp_bode", "w" );

  /* low-pass filter */
#if DIM == 2
  dzPexAlloc( &lp, 0, 2 );
  dzPexSetNumList( &lp, 1.0 );
  dzPexSetDenList( &lp, 1.0, 2*T, T*T );
#else /* DIM == 1 */
  dzPexAlloc( &lp, 0, 1 );
  dzPexSetNumList( &lp, 1.0 );
  dzPexSetDenList( &lp, 1.0, T );
#endif
  /* high-pass filter */
#if DIM == 2
  dzPexAlloc( &hp, 2, 2 );
  dzPexSetNumList( &hp, 0.0, 2*T, T*T );
  dzPexSetDenList( &hp, 1.0, 2*T, T*T );
#else /* DIM == 1 */
  dzPexAlloc( &hp, 1, 1 );
  dzPexSetNumList( &hp, 0.0, T );
  dzPexSetDenList( &hp, 1.0, T );
#endif

  for( frq=0.001; frq<10000; frq*=1.2 ){
    dzPexFreqRes( &lp, frq, &c );
    fprintf( fp[0], "%f %f %f\n", frq,
      log(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
    dzPexFreqRes( &hp, frq, &c );
    fprintf( fp[1], "%f %f %f\n", frq,
      log(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
  }
  dzPexDestroy( &lp );
  dzPexDestroy( &hp );
  fclose( fp[0] );
  fclose( fp[1] );
  return 0;
}
