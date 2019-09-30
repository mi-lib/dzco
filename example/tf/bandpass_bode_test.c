#include <dzco/dz_tf.h>

#define DIM 2

#define T 1.0

int main(int argc, char *argv[])
{
  dzTF lp, hp;
  double frq;
  dzFreqRes fr;
  FILE *fp[2];

  fp[0] = fopen( "lp_bode", "w" );
  fp[1] = fopen( "hp_bode", "w" );

  /* low-pass filter */
#if DIM == 2
  dzTFAlloc( &lp, 0, 2 );
  dzTFSetNumList( &lp, 1.0 );
  dzTFSetDenList( &lp, 1.0, 2*T, T*T );
#else /* DIM == 1 */
  dzTFAlloc( &lp, 0, 1 );
  dzTFSetNumList( &lp, 1.0 );
  dzTFSetDenList( &lp, 1.0, T );
#endif
  /* high-pass filter */
#if DIM == 2
  dzTFAlloc( &hp, 2, 2 );
  dzTFSetNumList( &hp, 0.0, 2*T, T*T );
  dzTFSetDenList( &hp, 1.0, 2*T, T*T );
#else /* DIM == 1 */
  dzTFAlloc( &hp, 1, 1 );
  dzTFSetNumList( &hp, 0.0, T );
  dzTFSetDenList( &hp, 1.0, T );
#endif

  for( frq=0.001; frq<10000; frq*=1.2 ){
    dzFreqResFromTF( &fr, &lp, frq );
    fprintf( fp[0], "%f %f %f\n", frq, fr.g, fr.p );
    dzFreqResFromTF( &fr, &hp, frq );
    fprintf( fp[1], "%f %f %f\n", frq, fr.g, fr.p );
  }
  dzTFDestroy( &lp );
  dzTFDestroy( &hp );
  fclose( fp[0] );
  fclose( fp[1] );
  return 0;
}
