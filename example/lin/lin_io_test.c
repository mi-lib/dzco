#include <dzco/dz_lin.h>

int main(void)
{
  dzLin lin;
  FILE *fp;

  fp = fopen( "lin_test.dzl", "r" );
  dzLinFRead( fp, &lin );
  dzLinWrite( &lin );
  fclose( fp );
  return 0;
}
