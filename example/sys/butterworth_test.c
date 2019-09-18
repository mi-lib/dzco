#include <dzco/dz_sys.h>

#define FREQ_LOW  0.1    /* lower cutoff frequency */
#define FREQ_HIGH 0.5    /* upper cutoff frequency */
#define DT        0.01   /* sampling time */
#define STEP      10000

#define N 5

int main(void)
{
  register int i, j;
  dzSys bwf[N], maf[N];
  double val;

  /* first-order ... forth-order butterworth filters */
  for( j=0; j<N; j++ ){
    dzSysBWCreate( &bwf[j], FREQ_HIGH, j+1 );
    dzSysInputPtr(&bwf[j],0) = &val;
    dzSysMAFCreate( &maf[j], 1.0 ); /* 1.0 = dummy forgetting factor */
    dzSysMAFSetCF( &maf[j], FREQ_HIGH, DT );
    if( j == 0 )
      dzSysInputPtr(&maf[j],0) = &val;
    else
      dzSysConnect( &maf[j-1], 0, &maf[j], 0 );
  }
  zRandInit();

  /* signal processing */
  for( i=0; i<=STEP; i++ ){
    /* noized sine wave */
    val = sin( 0.5 * DT * i ) + zRandF(-1.0,1.0) + 1.0;
    printf( "%f ", val );
    /* signal filtering */
    for( j=0; j<N; j++ )
      printf( "%f ", zVecElem(dzSysUpdate(&bwf[j],DT),0) );
    for( j=0; j<N; j++ )
      printf( "%f ", zVecElem(dzSysUpdate(&maf[j],DT),0) );
    printf( "\n" );
  }

  for( j=0; j<N; j++ ){
    dzSysDestroy( &bwf[j] );
    dzSysDestroy( &maf[j] );
  }
  return 0;
}
