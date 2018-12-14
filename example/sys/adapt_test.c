#include <dzco/dz_sys.h>

#define DT 0.01

int main(int argc, char *argv[])
{
  register int i;
  dzSys adapt1, adapt2;
  double ref;

  dzSysCreateAdapt( &adapt1, 0.5, 0 );
  dzSysCreateAdapt( &adapt2, 0.5, 0 );
  dzSysInputPtr(&adapt1,0) = &ref;
  dzSysInputPtr(&adapt2,0) = &ref;

  for( i=0; i<800; i++ ){
    if( i >= 100 && i < 200 )
      ref =-1.0;
    else
    if( i >= 500 && i < 600 )
      ref = 3.0;
    else
      ref = NAN;
    dzSysAdaptSetBase( &adapt2, 0.5 * sin(2*zPI*i/100) );
    printf( "%f %f %f\n", ref, zVecElem(dzSysUpdate(&adapt1,DT),0), zVecElem(dzSysUpdate(&adapt2,DT),0) );
  }

  dzSysDestroy( &adapt1 );
  dzSysDestroy( &adapt2 );
  return 0;
}
