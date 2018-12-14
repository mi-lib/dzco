#include <dzco/dz_sys.h>

#define DT 0.01
#define STEP 500

int main(int argc, char *argv[])
{
  dzSysArray arr;
  dzSys *sys_out;
  FILE *fp;
  register int i;

  fp = fopen( "connect_test.dzs", "r" );
  dzSysArrayFRead( fp, &arr );
  dzSysArrayFWrite( stderr, &arr );
  sys_out = dzSysArrayNameFind( &arr, "intg2" );
  for( i=0; i<=STEP; i++ ){
    dzSysArrayUpdate( &arr, DT );
    printf( "%g %g\n", DT*i, dzSysOutputVal(sys_out,0) );
  }
  dzSysArrayDestroy( &arr );
  return 0;
}
