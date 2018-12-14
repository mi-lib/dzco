#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 5000

int main(int argc, char *argv[])
{
  register int i;
  dzPex *pex;
  dzLin *lin;
  dzSys fol, fol_pex;
  double ref;

  dzSysCreateFOL( &fol, 2.0, 1.0 );
  dzSysInputPtr(&fol,0) = &ref;

  pex = zAlloc( dzPex, 1 );
  dzPexAlloc( pex, 0, 1 );
  dzPexSetNumList( pex, 1.0 );
  dzPexSetDenList( pex, 1.0, 2.0 );
  lin = zAlloc( dzLin, 1 );
  dzPex2LinCtrlCanon( pex, lin );
  dzSysCreateLin( &fol_pex, lin );
  dzSysInputPtr(&fol_pex,0) = &ref;

  for( i=0; i<STEP; i++ ){
    ref = sin( 6*zPI*i/STEP );
    dzSysUpdate( &fol, DT );
    dzSysUpdate( &fol_pex, DT );
    printf( "%f %f %f\n", ref, dzSysOutputVal(&fol,0), dzSysOutputVal(&fol_pex,0) );
  }
  dzSysDestroy( &fol );
  dzSysDestroy( &fol_pex );
  return 0;
}
