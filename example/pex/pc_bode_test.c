#include <dzco/dz_pex.h>

int main(int argc, char *argv[])
{
  dzPex g;
  double frq;
  zComplex c;
  FILE *fp;

  fp = fopen( "bode", "w" );

  dzPexAlloc( &g, 1, 1 );
  dzPexSetNumList( &g, 1.0, 1.0 );
  dzPexSetDenList( &g, 1.0, 1.2 );

  for( frq=0.001; frq<10000; frq*=1.2 ){
    dzPexFreqRes( &g, frq, &c );
    fprintf( fp, "%f %f %f\n", frq,
      log(zComplexAbs(&c)), zRad2Deg(zComplexArg(&c)) );
  }
  dzPexDestroy( &g );
  fclose( fp );
  return 0;
}
