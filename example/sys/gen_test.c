#include <dzco/dz_sys.h>

#define DT 0.01
#define STEP 1000

int main(void)
{
  dzSys step, ramp, sine, wn, adder;
  register int i;

  dzSysCreateStep( &step, 1.0, 0.6, 2.0 );
  dzSysCreateRamp( &ramp, 1.0, 0.6, 2.0 );
  dzSysCreateSine( &sine, 1.0, 0.0, 2.0 );
  dzSysCreateWhitenoise( &wn, 0.2, 0.5, 2.0 );
  dzSysCreateAdder( &adder, 4 );
  dzSysConnect( &step, 0, &adder, 0 );
  dzSysConnect( &ramp, 0, &adder, 1 );
  dzSysConnect( &sine, 0, &adder, 2 );
  dzSysConnect( &wn,   0, &adder, 3 );
  for( i=0; i<=STEP; i++ ){
    dzSysUpdate( &step, DT );
    dzSysUpdate( &ramp, DT );
    dzSysUpdate( &sine, DT );
    dzSysUpdate( &wn, DT );
    dzSysUpdate( &adder, DT );
    printf( "%g %g %g %g %g %g\n", DT*i, dzSysOutputVal(&step,0), dzSysOutputVal(&ramp,0), dzSysOutputVal(&sine,0), dzSysOutputVal(&wn,0), dzSysOutputVal(&adder,0) );
  }
  dzSysDestroy( &step );
  dzSysDestroy( &ramp );
  dzSysDestroy( &sine );
  dzSysDestroy( &wn );
  dzSysDestroy( &adder );
  return 0;
}
