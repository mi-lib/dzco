#include <dzco/dz_sys.h>

#define DT     0.01
#define STEP 200

#define OMEGA ( 4*zPI/(STEP*DT) )

bool assert_pid(void)
{
  double u, yp, yi, yd, y;
  dzSys psys, isys, dsys, pidsys;
  int i;
  bool result = true;

  zRandInit();
  dzSysCreateP( &psys, 4 );
  dzSysInputPtr(&psys,0) = &u;
  dzSysCreateI( &isys, 3, 0 );
  dzSysInputPtr(&isys,0) = &u;
  dzSysCreateD( &dsys, 2, 0.0 );
  dzSysInputPtr(&dsys,0) = &u;
  dzSysCreatePID( &pidsys, 4, 3, 2, 0.0, 0.0 );
  dzSysInputPtr(&pidsys,0) = &u;
  for( i=0; i<=STEP; i++ ){
    u = zRandF(-10,10);
    yp = zVecElem(dzSysUpdate(&psys,DT),0);
    yi = zVecElem(dzSysUpdate(&isys,DT),0);
    yd = zVecElem(dzSysUpdate(&dsys,DT),0);
    y  = zVecElem(dzSysUpdate(&pidsys,DT),0);
    if( !zIsTiny( y - (yp+yi+yd) ) ) result = false;
  }
  dzSysDestroy( &psys );
  dzSysDestroy( &isys );
  dzSysDestroy( &dsys );
  dzSysDestroy( &pidsys );
  return result;
}

int main(void)
{
  zAssert( dzSysCreatePID, assert_pid() );
  return EXIT_SUCCESS;
}
