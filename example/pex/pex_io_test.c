#include <dzco/dz_pex.h>

int main(int argc, char *argv[])
{
  dzPex pex;
  FILE *fp;

  fp = fopen( "pex_test.dzp", "r" );
  dzPexFScan( fp, &pex );
  dzPexPrint( &pex );
  dzPexExpr( &pex );
  dzPexDestroy( &pex );
  fclose( fp );
  return 0;
}
