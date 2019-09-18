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
  dzSys sol, soltf;
  dzTF *tf;
  double ref;

  /* SOL */
  dzSysSOLCreateGeneric( &sol, A, B, C, D, E );
  dzSysInputPtr(&sol,0) = &ref;
  /* TF */
  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 1, 2 );
  dzTFSetNumElem( tf, 0, E );
  dzTFSetNumElem( tf, 1, D );
  dzTFSetDenElem( tf, 0, C );
  dzTFSetDenElem( tf, 1, B );
  dzTFSetDenElem( tf, 2, A );
  dzSysTFCreate( &soltf, tf );
  dzSysInputPtr(&soltf,0) = &ref;

  ref = argc > 1 ? atof( argv[1] ) : 1.0;

  for( i=0; i<=STEP; i++ ){
    printf( "%f %f %f\n", ref, zVecElem(dzSysUpdate(&sol,DT),0), zVecElem(dzSysUpdate(&soltf,DT),0) );
  }
  dzSysDestroy( &sol );
  dzSysDestroy( &soltf );
  return 0;
}
