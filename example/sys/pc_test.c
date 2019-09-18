/* phase-compensator test (following Nakano and Mita's text) */
#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 5000

double ref;
dzSys sys1[6], sys2[6], sys3[6], sys4[6], sol1, sol3, sol4;

/*
  ref -> sys[0] -> sys[1] -> sys[2] -> sys[3] -> sys[4] -> sys[5] ->
           P    +   -^        cmp       5.0/s(0.1s+1)(0.5+1)  |
                     |                                        |
                     |----------------------------------------|
 */
void create(dzSys sys[], double *ref)
{
  dzSysPCreate( &sys[0], 1.0 );
  dzSysSubtrCreate( &sys[1], 2 );
  dzSysPCreate( &sys[2], 1.0 );
  dzSysFOLCreate( &sys[3], 0.1, 2.5 );
  dzSysFOLCreate( &sys[4], 0.5, 2.0 );
  dzSysICreate( &sys[5], 1.0, 0 );

  dzSysInputPtr(&sys[0],0) = ref;
  dzSysConnect( &sys[0], 0, &sys[1], 0 );
  dzSysConnect( &sys[5], 0, &sys[1], 1 );
  dzSysConnect( &sys[1], 0, &sys[2], 0 );
  dzSysConnect( &sys[2], 0, &sys[3], 0 );
  dzSysConnect( &sys[3], 0, &sys[4], 0 );
  dzSysConnect( &sys[4], 0, &sys[5], 0 );

  *ref = 1.0;
}

void update(dzSys sys[], double dt)
{
  dzSysUpdate( &sys[0], dt );
  dzSysUpdate( &sys[1], dt );
  dzSysUpdate( &sys[2], dt );
  dzSysUpdate( &sys[3], dt );
  dzSysUpdate( &sys[4], dt );
  dzSysUpdate( &sys[5], dt );
}

void destroy(dzSys sys[])
{
  dzSysDestroy( &sys[0] );
  dzSysDestroy( &sys[1] );
  dzSysDestroy( &sys[2] );
  dzSysDestroy( &sys[3] );
  dzSysDestroy( &sys[4] );
  dzSysDestroy( &sys[5] );
}

int main(void)
{
  register int i;

  create( sys1, &ref );
  create( sys2, &ref );
  create( sys3, &ref );
  create( sys4, &ref );
  dzSysSOLCreate( &sol1, 1.0/2.89, 0, 0.175, 1.0 );
  dzSysInputPtr(&sol1,0) = &ref;
  /* gain compensater */
  dzSysDestroy( &sys2[2] );
  dzSysPCreate( &sys2[2], 0.282 );
  dzSysConnect( &sys2[1], 0, &sys2[2], 0 );
  /* phase-lag compensater */
  dzSysDestroy( &sys3[2] );
  dzSysPCCreate( &sys3[2], 37.47, 8.33, 1.0 );
  dzSysConnect( &sys3[1], 0, &sys3[2], 0 );
  dzSysSOLCreate( &sol3, 1.0/1.53, 0, 0.5, 1.0 );
  dzSysInputPtr(&sol3,0) = &ref;
  /* phase-lead compensater */
  dzSysDestroy( &sys4[2] );
  dzSysPCCreate( &sys4[2], 0.055, 0.5, 1.0 );
  dzSysConnect( &sys4[1], 0, &sys4[2], 0 );
  dzSysSOLCreate( &sol4, 1.0/5.68, 0, 0.5, 1.0 );
  dzSysInputPtr(&sol4,0) = &ref;

  for( i=0; i<=STEP; i++ ){
    update( sys1, DT );
    update( sys2, DT );
    update( sys3, DT );
    update( sys4, DT );
    dzSysUpdate( &sol1, DT );
    dzSysUpdate( &sol3, DT );
    dzSysUpdate( &sol4, DT );
    printf( "%f %f %f %f %f %f %f %f\n", ref, dzSysOutputVal(&sys1[5],0), dzSysOutputVal(&sol1,0), dzSysOutputVal(&sys2[5],0), dzSysOutputVal(&sys3[5],0), dzSysOutputVal(&sol3,0), dzSysOutputVal(&sys4[5],0), dzSysOutputVal(&sol4,0) );
  }
  destroy( sys1 );
  destroy( sys2 );
  destroy( sys3 );
  destroy( sys4 );
  dzSysDestroy( &sol1 );
  dzSysDestroy( &sol3 );
  dzSysDestroy( &sol4 );
  return 0;
}
