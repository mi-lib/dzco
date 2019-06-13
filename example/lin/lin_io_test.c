#include <dzco/dz_lin.h>

int main(void)
{
  dzLin lin;
  FILE *fp;

  fp = fopen( "lin_test.dzl", "r" );
  dzLinFScan( fp, &lin );
  dzLinPrint( &lin );
  fclose( fp );
  return 0;
}
