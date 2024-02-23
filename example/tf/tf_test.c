/* run this program as
 % ./tf_test | gnuplot -p -
 */

#include <dzco/dz_sys.h>

#define DT 0.001
#define STEP 3000

#define T 0.3
#define Z 0.7

#define OUTPUT_FILE "pid_tf.dat"

void output_plotscript(const char *filename)
{
  printf( "set grid\n" );
  printf( "plot '%s' u 1:2 w l t \"LS\", '%s' u 1:3 w l t \"TF\", '%s' u 1:4 w l t \"SOL\"\n", filename, filename, filename );
}

int main(void)
{
  int i;
  dzTF *tf;
  dzLin *lin;
  dzSys sys_tf, sys_lin, sys_sol;
  double ref = 1.0;
  FILE *fp;

  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 0, 2 );
  dzTFSetNumList( tf, 1.0, 0.0 );
  dzTFSetDenList( tf, 1.0, 2*Z*T, T*T );

  dzSysTFCreate( &sys_tf, tf );
  dzSysInputPtr(&sys_tf,0) = &ref;

  lin = zAlloc( dzLin, 1 );
  dzTF2LinObsCanon( tf, lin );
  dzSysLinCreate( &sys_lin, lin );
  dzSysInputPtr(&sys_lin,0) = &ref;
  dzSysSOLCreateGeneric( &sys_sol, T*T, 2*Z*T, 1.0, 0.0, 1.0 );
  dzSysInputPtr(&sys_sol,0) = &ref;

  fp = fopen( OUTPUT_FILE, "w" );
  for( i=0; i<=STEP; i++ ){
    dzSysUpdate( &sys_lin, DT );
    dzSysUpdate( &sys_tf, DT );
    dzSysUpdate( &sys_sol, DT );
    fprintf( fp, "%f %f %f %f\n", DT*i, dzSysOutputVal(&sys_lin,0), dzSysOutputVal(&sys_tf,0), dzSysOutputVal(&sys_sol,0) );
  }
  fclose( fp );
  dzSysDestroy( &sys_lin );
  dzSysDestroy( &sys_tf );
  dzSysDestroy( &sys_sol );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
