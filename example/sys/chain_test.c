#include <dzco/dz_sys.h>

int main(void)
{
  double i;
  dzSys c1, c2, c3;

  dzSysPCreate( &c1, 2 );
  dzSysPCreate( &c2, 3 );
  dzSysPCreate( &c3, 2 );
  dzSysInputPtr(&c1,0) = &i;
  dzSysChain( 3, &c1, &c2, &c3 );
  for( i=-2; i<=2; i+=0.01 ){
    dzSysUpdate( &c1, 1 );
    dzSysUpdate( &c2, 1 );
    dzSysUpdate( &c3, 1 );
    printf( "%f %f %f %f\n", dzSysInputVal(&c1,0), dzSysOutputVal(&c1,0), dzSysOutputVal(&c2,0), dzSysOutputVal(&c3,0) );
  }
  dzSysDestroy( &c1 );
  dzSysDestroy( &c2 );
  dzSysDestroy( &c3 );
  return 0;
}
