#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 3000

#define T 0.3
#define Z 0.7

int main(void)
{
  register int i;
  dzPex *pex;
  dzLin *lin;
  dzSys sys_pex, sys_lin, sys_sol;
  double ref = 1.0;

  pex = zAlloc( dzPex, 1 );
  dzPexAlloc( pex, 0, 2 );
  dzPexSetNumList( pex, 1.0, 0.0 );
  dzPexSetDenList( pex, 1.0, 2*Z*T, T*T );

  dzSysCreatePex( &sys_pex, pex );
  dzSysInputPtr(&sys_pex,0) = &ref;

  lin = zAlloc( dzLin, 1 );
  dzPex2LinObsCanon( pex, lin );
  dzSysCreateLin( &sys_lin, lin );
  dzSysInputPtr(&sys_lin,0) = &ref;
  dzSysCreateSOLGen( &sys_sol, T*T, 2*Z*T, 1.0, 0.0, 1.0 );
  dzSysInputPtr(&sys_sol,0) = &ref;

  for( i=0; i<=STEP; i++ ){
    dzSysUpdate( &sys_lin, DT );
    dzSysUpdate( &sys_pex, DT );
    dzSysUpdate( &sys_sol, DT );
    printf( "%f %f %f\n", dzSysOutputVal(&sys_lin,0), dzSysOutputVal(&sys_pex,0), dzSysOutputVal(&sys_sol,0) );
  }

  dzSysDestroy( &sys_lin );
  dzSysDestroy( &sys_pex );
  dzSysDestroy( &sys_sol );
  return 0;
}
