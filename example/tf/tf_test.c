#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 3000

#define T 0.3
#define Z 0.7

int main(void)
{
  register int i;
  dzTF *tf;
  dzLin *lin;
  dzSys sys_tf, sys_lin, sys_sol;
  double ref = 1.0;

  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 0, 2 );
  dzTFSetNumList( tf, 1.0, 0.0 );
  dzTFSetDenList( tf, 1.0, 2*Z*T, T*T );

  dzSysTFCreate( &sys_tf, tf );
  dzSysInputPtr(&sys_tf,0) = &ref;

  lin = zAlloc( dzLin, 1 );
  dzTF2LinObsCanon( tf, lin );
  dzSysLinCreate( &sys_lin, lin );
  dzSysInputPtr(&sys_lin,0) = &ref;
  dzSysSOLCreateGeneric( &sys_sol, T*T, 2*Z*T, 1.0, 0.0, 1.0 );
  dzSysInputPtr(&sys_sol,0) = &ref;

  for( i=0; i<=STEP; i++ ){
    dzSysUpdate( &sys_lin, DT );
    dzSysUpdate( &sys_tf, DT );
    dzSysUpdate( &sys_sol, DT );
    printf( "%f %f %f\n", dzSysOutputVal(&sys_lin,0), dzSysOutputVal(&sys_tf,0), dzSysOutputVal(&sys_sol,0) );
  }

  dzSysDestroy( &sys_lin );
  dzSysDestroy( &sys_tf );
  dzSysDestroy( &sys_sol );
  return 0;
}
