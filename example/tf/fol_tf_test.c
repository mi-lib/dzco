/* run this program as
 % ./fol_tf_test | gnuplot -p -
 */

#include <dzco/dz_sys.h>

#define DT  0.001
#define DIV 10000
#define NW  5

#define OUTPUT_FILE "fol_tf.dat"

void output_plotscript(const char *filename)
{
  printf( "set grid\n" );
  printf( "plot '%s' u 1:2 w l t \"reference\", '%s' u 1:3 w l t \"FOL\", '%s' u 1:4 w l t \"FOL-TF\"\n", filename, filename, filename );
}

int main(int argc, char *argv[])
{
  int i;
  dzTF *tf;
  dzLin *lin;
  dzSys fol, fol_tf;
  double ref;
  FILE *fp;

  dzSysFOLCreate( &fol, 2.0, 1.0 );
  dzSysInputPtr(&fol,0) = &ref;

  tf = zAlloc( dzTF, 1 );
  dzTFAlloc( tf, 0, 1 );
  dzTFSetNumList( tf, 1.0 );
  dzTFSetDenList( tf, 1.0, 2.0 );
  lin = zAlloc( dzLin, 1 );
  dzTF2LinCtrlCanon( tf, lin );
  dzSysLinCreate( &fol_tf, lin );
  dzSysInputPtr(&fol_tf,0) = &ref;

  fp = fopen( OUTPUT_FILE, "w" );
  for( i=0; i<DIV; i++ ){
    ref = sin( NW*2*zPI*i/DIV );
    dzSysUpdate( &fol, DT );
    dzSysUpdate( &fol_tf, DT );
    fprintf( fp, "%g %f %f %f\n", DT*i, ref, dzSysOutputVal(&fol,0), dzSysOutputVal(&fol_tf,0) );
  }
  fclose( fp );
  dzSysDestroy( &fol );
  dzSysDestroy( &fol_tf );
  output_plotscript( OUTPUT_FILE );
  return 0;
}
