#include <dzco/dz_sys.h>

#define DT     0.01
#define STEP 500

void output_step(dzSys *qpd, dzSys *intg1, dzSys *intg2, double goal)
{
  int i;

  dzSysQPDSetGoal( qpd, goal );
  for( i=0; i<STEP; i++ ){
    dzSysUpdate(qpd,DT);
    dzSysUpdate(intg1,DT);
    dzSysUpdate(intg2,DT);
    printf( "%f %f %f %f\n", goal, dzSysOutputVal(qpd,0), dzSysOutputVal(intg1,0), dzSysOutputVal(intg2,0) );
  }
}

int main(int argc, char *argv[])
{
  dzSys qpd, intg1, intg2;

  dzSysQPDCreate( &qpd, 10.0, 12.0, 0.01 );
  dzSysICreate( &intg1, 1.0, 0 );
  dzSysICreate( &intg2, 1.0, 0 );
  dzSysConnect( &qpd, 0, &intg1, 0 );
  dzSysConnect( &intg1, 0, &intg2, 0 );
  dzSysConnect( &intg2, 0, &qpd, 0 );

  output_step( &qpd, &intg1, &intg2, 2.0 );
  output_step( &qpd, &intg1, &intg2,-1.0 );
  output_step( &qpd, &intg1, &intg2, 1.0 );

  dzSysDestroy( &qpd );
  dzSysDestroy( &intg1 );
  dzSysDestroy( &intg2 );
  return 0;
}
