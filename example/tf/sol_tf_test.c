/* run this program as
 % ./sol_tf_test | gnuplot -p -
 */

#include <dzco/dz_sys.h>

#define DT      0.001
#define STEP 3000

#define A ( 0.1 * 0.1 )
#define B ( 2 * 0.6 * 0.1 )
#define C ( 1.0 )
#define D ( 0.5 )
#define E ( 2.0 )

#define OUTPUT_FILE "sol_tf.dat"

void output_plotscript(const char *filename)
{
  printf( "set grid\n" );
  printf( "plot '%s' u 1:2 w l t \"reference\", '%s' u 1:3 w l t \"SOL\", '%s' u 1:4 w l t \"SOL-TF\"\n", filename, filename, filename );
}

int main(int argc, char *argv[])
{
  int i;
  dzSys sol, soltf;
  dzTF *tf;
  double ref;
  FILE *fp;

  /* SOL */
  dzSysSOLCreateGeneric( &sol, A, B, C, D, E );
  dzSysInputPtr(&sol,0) = &ref;
  /* TF */
  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 1, 2 );
  dzTFSetNumElem( tf, 0, E );
  dzTFSetNumElem( tf, 1, D );
  dzTFSetDenElem( tf, 0, C );
  dzTFSetDenElem( tf, 1, B );
  dzTFSetDenElem( tf, 2, A );
  dzSysTFCreate( &soltf, tf );
  dzSysInputPtr(&soltf,0) = &ref;

  ref = argc > 1 ? atof( argv[1] ) : 1.0;

  fp = fopen( OUTPUT_FILE, "w" );
  for( i=0; i<=STEP; i++ ){
    dzSysUpdate( &sol,   DT );
    dzSysUpdate( &soltf, DT );
    fprintf( fp, "%f %f %f %f\n", DT*i, ref, dzSysOutputVal(&sol,0), dzSysOutputVal(&soltf,0) );
  }
  fclose( fp );
  dzSysDestroy( &sol );
  dzSysDestroy( &soltf );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
