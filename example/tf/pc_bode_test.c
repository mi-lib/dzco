#include <dzco/dz_tf.h>

int main(int argc, char *argv[])
{
  dzTF g;
  double frq;
  dzFreqRes fr;
  FILE *fp;

  fp = fopen( "bode", "w" );

  dzTFAlloc( &g, 1, 1 );
  dzTFSetNumList( &g, 1.0, 1.0 );
  dzTFSetDenList( &g, 1.0, 1.2 );

  for( frq=0.001; frq<10000; frq*=1.2 ){
    dzFreqResFromTF( &fr, &g, frq );
    fprintf( fp, "%f %f %f\n", frq, fr.g, fr.p );
  }
  dzTFDestroy( &g );
  fclose( fp );
  return 0;
}
