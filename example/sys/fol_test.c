#include <dzco/dz_sys.h>

#define DT      0.001
#define STEP 3000

void output_step(dzSys *fol, double x0)
{
  register int i;

  dzSysOutputVal(fol,0) = x0;
  for( i=0; i<=STEP; i++ )
    printf( "%f %f\n", dzSysInputVal(fol,0), zVecElem(dzSysUpdate(fol,DT),0) );
}

int main(int argc, char *argv[])
{
  dzSys fol;
  double ref;

  dzSysCreateFOL( &fol, 0.5, 1.0 );
  dzSysInputPtr(&fol,0) = &ref;

  ref = 1.0;
  output_step( &fol, 0.0 );
  output_step( &fol, 4.0 );
  output_step( &fol,-1.0 );

  dzSysDestroy( &fol );
  return 0;
}
