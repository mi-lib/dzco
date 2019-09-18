#include <zm/zm_rand.h>
#include <dzco/dz_sys.h>

#define T1 2.0
#define T2 1.0
#define TN 0.05

#define FF 0.99
#define CF (0.1/TN)

#define DT 0.001
#define STEP 5000

double func_gen(double t)
{
  return 2*sin(2*zPI/T1*t) + 1*sin(2*zPI/T2*t);
}

double noise_gen(double t)
{
  return zRandMTF(NULL,0,1.0) * sin(2*zPI/TN*t);
}

int main(int argc, char *argv[])
{
  double t, val, val_n, ddt;
  dzSys maf;
  register int i;

  zRandInitMT( NULL );
  dzSysMAFCreate( &maf, FF );
/*
  dzSysMAFSetCF( &maf, CF );
*/
  dzSysInputPtr(&maf,0) = &val_n;

  for( i=0; i<=STEP; i++ ){
    ddt = zRandMTF(NULL,0,DT*3);
    t = i * DT + ddt;
    val   = func_gen( t );
    val_n = val + noise_gen( t );
    printf( "%f %f %f %f\n", t, val, val_n, zVecElem(dzSysUpdate(&maf,DT+ddt),0) );
  }
  dzSysDestroy( &maf );
  return 0;
}
