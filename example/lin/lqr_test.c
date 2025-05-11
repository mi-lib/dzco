#include <dzco/dz_sys.h>

#define DT 0.01
#define STEP 3000

#define DIM 2

dzSys sys;
double input;

void sys_create(void)
{
  dzLin *lin;

  lin = zAlloc( dzLin, 1 );
  dzLinAlloc( lin, DIM );
  zMatSetElem( lin->a, 0, 1, 1 ); /* transition matrix */
  zMatSetElem( lin->a, 1, 0,-2 );
  zMatSetElem( lin->a, 1, 1,-3 );
  zVecSetElem( lin->b, 1, 1 ); /* input coefficient vector */
  zVecSetElem( lin->c, 0, 1 ); /* output coefficient vector */
  lin->d = 0; /* output transfer coefficient */
  if( !dzLinIsCtrl( lin ) ){
    ZRUNERROR( "system uncontrollable" );
    exit( 1 );
  }
  dzSysLinCreate( &sys, lin );
  dzSysInputPtr(&sys,0) = &input;
}

/* regulator gains */
zVec opt_gain;

#define Q1 5.0  /* weight for position regulation */
#define Q2 5.0  /* weight for velocity regulation */
#define R  1.0  /* weight for cart driving force */

void reg_create(void)
{
  zVec q;
  double r;

  q = zVecCreateList( DIM, Q1, Q2 );
  r = R;
  opt_gain = zVecAlloc( DIM );
  if( !dzLinLQR( dzSysLin(&sys), q, r, opt_gain ) ) exit( 1 );
  /* answer: [ 1 1 ] */
  zVecFPrint( stderr, opt_gain );
  zVecFree( q );
}

/* main loop */
int main(int argc, char *argv[])
{
  int i;
  zVec ref;

  sys_create();
  reg_create();
  ref = zVecAlloc( DIM );
  if( argc > 1 ) zVecSetElem( dzSysLin(&sys)->x, 0, atof(argv[1]) );
  for( i=0; i<STEP; i++ ){
    printf( "%g %g %g %g\n", input, zVecElem(ref,0), zVecElem(dzSysLin(&sys)->x,0), dzSysOutputVal(&sys,0) );
    input = dzLinStateFeedback( dzSysLin(&sys), ref, opt_gain );
    dzSysUpdate( &sys, DT );
  }
  zVecFree( opt_gain );
  dzSysDestroy( &sys );
  zVecFree( ref );
  return 0;
}
