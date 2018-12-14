#include <dzco/dz_sys.h>

int main(void)
{
  dzSysArray arr;
  dzSys *sys;
  char buf[BUFSIZ];
  FILE *fp;
  int i;

  fp = fopen( "read_test.dzs", "r" );
  dzSysArrayFRead( fp, &arr );
  for( i=0; i<zArrayNum(&arr); i++ )
    printf( "sys: %s\n", zName(zArrayElem(&arr,i)) );
  while( 1 ){
    if( !fgets( buf, BUFSIZ, stdin ) );
    zCutNL( buf );
    if( strcmp( buf, "quit" ) == 0 ) break;
    if( ( sys = dzSysArrayNameFind( &arr, buf ) ) )
      dzSysFWrite( stdout, sys );
    else
      printf( "%s not found.\n", buf );
  }

  dzSysArrayDestroy( &arr );
  return 0;
}
