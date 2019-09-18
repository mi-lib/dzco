#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 5000

int main(int argc, char *argv[])
{
  register int i;
  dzTF *tf;
  dzLin *lin;
  dzSys fol, fol_tf;
  double ref;

  dzSysFOLCreate( &fol, 2.0, 1.0 );
  dzSysInputPtr(&fol,0) = &ref;

  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 0, 1 );
  dzTFSetNumList( tf, 1.0 );
  dzTFSetDenList( tf, 1.0, 2.0 );
  lin = zAlloc( dzLin, 1 );
  dzTF2LinCtrlCanon( tf, lin );
  dzSysLinCreate( &fol_tf, lin );
  dzSysInputPtr(&fol_tf,0) = &ref;

  for( i=0; i<STEP; i++ ){
    ref = sin( 6*zPI*i/STEP );
    dzSysUpdate( &fol, DT );
    dzSysUpdate( &fol_tf, DT );
    printf( "%f %f %f\n", ref, dzSysOutputVal(&fol,0), dzSysOutputVal(&fol_tf,0) );
  }
  dzSysDestroy( &fol );
  dzSysDestroy( &fol_tf );
  return 0;
}
