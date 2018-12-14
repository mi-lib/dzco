#include <dzco/dz_sys.h>

/*
   Kd s^2 + Kp s + Ki
------------------------
s^3 + Kd s^2 + Kp s + Ki
 */

#define DT     0.01
#define STEP 500

#define KP 10.0
#define KI  2.0
#define KD  5.0

int main(int argc, char *argv[])
{
  register int i;
  dzSys pid, intg1, intg2, cpex;
  dzPex *pex;
  double ref, cur;
  double kp, ki, kd;

  kp = KP;
  ki = KI;
  kd = KD;

  /* PID */
  dzSysCreatePID( &pid, kp, ki, kd, 0, 0 );
  dzSysCreateI( &intg1, 1.0, 0.0 );
  dzSysCreateI( &intg2, 1.0, 0.0 );
  dzSysInputPtr(&pid,0) = &cur;
  dzSysConnect( &pid, 0, &intg1, 0 );
  dzSysConnect( &intg1, 0, &intg2, 0 );
  /* PEX */
  pex = zAlloc( dzPex, 1 );
  dzPexAlloc( pex, 2, 3 );
  dzPexSetNumElem( pex, 0, ki );
  dzPexSetNumElem( pex, 1, kp );
  dzPexSetNumElem( pex, 2, kd );
  dzPexSetDenElem( pex, 0, ki );
  dzPexSetDenElem( pex, 1, kp );
  dzPexSetDenElem( pex, 2, kd );
  dzPexSetDenElem( pex, 3, 1 );
  dzSysCreatePex( &cpex, pex );
  dzSysInputPtr(&cpex,0) = &ref;

  ref = argc > 1 ? atoi( argv[1] ) : 1.0;

  for( i=0; i<STEP; i++ ){
    cur = ref - dzSysOutputVal(&intg2,0);
    dzSysUpdate( &pid, DT );
    dzSysUpdate( &intg1, DT );
    dzSysUpdate( &intg2, DT );
    dzSysUpdate( &cpex, DT );
    printf( "%f %f %f %f %f\n", ref, dzSysOutputVal(&pid,0), dzSysOutputVal(&intg1,0), dzSysOutputVal(&intg2,0), dzSysOutputVal(&cpex,0) );
  }
  dzSysDestroy( &pid );
  dzSysDestroy( &intg1 );
  dzSysDestroy( &intg2 );
  dzSysDestroy( &cpex );
  return 0;
}
