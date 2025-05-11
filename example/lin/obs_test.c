/* observer test (following Nakano and Mita's text) */
#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 3000

#define DIM 2

dzLin *create_sl(void)
{
  dzLin *lin;

  lin = zAlloc( dzLin, 1 );
  dzLinAlloc( lin, DIM );
  zMatSetElem( lin->a, 0, 1, 1 );
  zMatSetElem( lin->a, 1, 1,-1 );
  zVecSetElem( lin->b, 1, 1 );
  zVecSetElem( lin->c, 0, 1 );
  lin->d = 0;
  return lin;
}

void create_obs(dzLin *lin, zVec k)
{
  zVec pole;

  pole = zVecCreateList( DIM, -4.0, -5.0 );
  if( !dzLinCreateObs( lin, pole, k ) ) exit( 1 );
  zVecFree( pole );
}

void output(dzLin *l1, dzLin *l2)
{
  printf( "%f %f %f %f\n", zVecElem(l1->x,0), zVecElem(l1->x,1), zVecElem(l2->x,0), zVecElem(l2->x,1) );
}

/* main loop */
#define THETA0 1.0
int main(int argc, char *argv[])
{
  dzLin *sl1, *sl2;
  dzSys sys;
  double input = 1.0;
  int i;
  zVec k; /* observer gain */

  sl1 = create_sl();
  sl2 = create_sl(); /* observer */
  dzSysLinCreate( &sys, sl1 );
  dzSysInputPtr(&sys,0) = &input;
  k = zVecAlloc( DIM );
  create_obs( sl2, k ); /* answer = [8 12] */
  zVecSetElem( sl2->x, 1, 1.0 );

  for( i=0; i<STEP; i++ ){
    output( sl1, sl2 );
    dzSysUpdate( &sys, DT );
    dzLinObsUpdate( sl2, k, dzSysInputVal(&sys,0), dzLinOutput(sl2,dzSysInputVal(&sys,0))-dzSysOutputVal(&sys,0), DT );
  }
  dzSysDestroy( &sys );
  dzLinDestroy( sl2 );
  zFree( sl2 );
  zVecFree( k );
  return 0;
}
