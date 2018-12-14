#include <dzco/dz_sys.h>

#define DT      0.001
#define STEP 3000

#define A ( 0.1 * 0.1 )
#define B ( 2 * 0.6 * 0.1 )
#define C ( 1.0 )
#define D ( 0.5 )
#define E ( 2.0 )

int main(int argc, char *argv[])
{
  register int i;
  dzSys sol, solpex;
  dzPex *pex;
  double ref;

  /* SOL */
  dzSysCreateSOLGen( &sol, A, B, C, D, E );
  dzSysInputPtr(&sol,0) = &ref;
  /* PEX */
  pex = zAlloc( dzPex, 1 );
  dzPexAlloc( pex, 1, 2 );
  dzPexSetNumElem( pex, 0, E );
  dzPexSetNumElem( pex, 1, D );
  dzPexSetDenElem( pex, 0, C );
  dzPexSetDenElem( pex, 1, B );
  dzPexSetDenElem( pex, 2, A );
  dzSysCreatePex( &solpex, pex );
  dzSysInputPtr(&solpex,0) = &ref;

  ref = argc > 1 ? atof( argv[1] ) : 1.0;

  for( i=0; i<=STEP; i++ ){
    printf( "%f %f %f\n", ref, zVecElem(dzSysUpdate(&sol,DT),0), zVecElem(dzSysUpdate(&solpex,DT),0) );
  }
  dzSysDestroy( &sol );
  dzSysDestroy( &solpex );
  return 0;
}
