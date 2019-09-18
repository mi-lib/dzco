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
  dzSys pid, intg1, intg2, ctf;
  dzTF *tf;
  double ref, cur;
  double kp, ki, kd;

  kp = KP;
  ki = KI;
  kd = KD;

  /* PID */
  dzSysPIDCreate( &pid, kp, ki, kd, 0, 0 );
  dzSysICreate( &intg1, 1.0, 0.0 );
  dzSysICreate( &intg2, 1.0, 0.0 );
  dzSysInputPtr(&pid,0) = &cur;
  dzSysConnect( &pid, 0, &intg1, 0 );
  dzSysConnect( &intg1, 0, &intg2, 0 );
  /* TF */
  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 2, 3 );
  dzTFSetNumElem( tf, 0, ki );
  dzTFSetNumElem( tf, 1, kp );
  dzTFSetNumElem( tf, 2, kd );
  dzTFSetDenElem( tf, 0, ki );
  dzTFSetDenElem( tf, 1, kp );
  dzTFSetDenElem( tf, 2, kd );
  dzTFSetDenElem( tf, 3, 1 );
  dzSysTFCreate( &ctf, tf );
  dzSysInputPtr(&ctf,0) = &ref;

  ref = argc > 1 ? atoi( argv[1] ) : 1.0;

  for( i=0; i<STEP; i++ ){
    cur = ref - dzSysOutputVal(&intg2,0);
    dzSysUpdate( &pid, DT );
    dzSysUpdate( &intg1, DT );
    dzSysUpdate( &intg2, DT );
    dzSysUpdate( &ctf, DT );
    printf( "%f %f %f %f %f\n", ref, dzSysOutputVal(&pid,0), dzSysOutputVal(&intg1,0), dzSysOutputVal(&intg2,0), dzSysOutputVal(&ctf,0) );
  }
  dzSysDestroy( &pid );
  dzSysDestroy( &intg1 );
  dzSysDestroy( &intg2 );
  dzSysDestroy( &ctf );
  return 0;
}
