#include <dzco/dz_sys.h>

#define DT      0.001
#define STEP 3000

int main(int argc, char *argv[])
{
  register int i;
  dzSys sol, solgen;
  double ref;

  ref = argc > 1 ? atof( argv[1] ) : 1.0;
  dzSysSOLCreate( &sol, 0.1, 0.0, 0.8, 2.0 );
  dzSysSOLCreateGeneric( &solgen, 0.1*0.1, 2*0.8*0.1, 1.0, 0.0, 2.0 );
  dzSysInputPtr(&sol,0) = &ref;
  dzSysInputPtr(&solgen,0) = &ref;
  for( i=0; i<=STEP; i++ )
    printf( "%f %f %f\n", ref, zVecElem(dzSysUpdate(&sol,DT),0), zVecElem(dzSysUpdate(&solgen,DT),0) );

  dzSysDestroy( &sol );
  dzSysDestroy( &solgen );
  return 0;
}
