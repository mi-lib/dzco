#include <dzco/dz_tf.h>

bool cval_set_equal(zCVec v1, zCVec v2)
{
  register int i;
  bool ret = true;

  for( i=0; i<zCVecSizeNC(v2); i++ )
    if( !zCVecValIsIncluded( v1, zCVecElemNC(v2,i) ) ) ret = false;
  for( i=0; i<zCVecSizeNC(v1); i++ )
    if( !zCVecValIsIncluded( v2, zCVecElemNC(v1,i) ) ) ret = false;
  return ret;
}

#define NUM_ZEROS 2
#define NUM_POLES 5

void assert_zeropole(void)
{
  zCVec zero_src, pole_src;
  zCVec zero_est, pole_est;
  dzTF tf;

  zRandInit();
  zero_src = zCVecAlloc( NUM_ZEROS );
  pole_src = zCVecAlloc( NUM_POLES );

  zComplexCreate( zCVecElemNC(zero_src,0), zRandF(-10,0), 0 );
  zComplexCreate( zCVecElemNC(zero_src,1), zRandF(0,10), 0 );
  zComplexCreate( zCVecElemNC(pole_src,0), zRandF(-10,0), 0 );
  zComplexCreate( zCVecElemNC(pole_src,1), zRandF(-10,0), zRandF(-10,10) );
  zComplexConj( zCVecElemNC(pole_src,1), zCVecElemNC(pole_src,2) );
  zComplexCreate( zCVecElemNC(pole_src,3), zRandF(-10,0), zRandF(-10,10) );
  zComplexConj( zCVecElemNC(pole_src,3), zCVecElemNC(pole_src,4) );

  dzTFCreateZeroPole( &tf, zero_src, pole_src );
  dzTFZeroPole( &tf, &zero_est, &pole_est );

  zAssert( dzTFCreateZeroPole + dzTFZeroPole,
    cval_set_equal( zero_src, zero_est ) &&
    cval_set_equal( pole_src, pole_est ) );

  dzTFDestroy( &tf );
  zCVecFree( zero_src );
  zCVecFree( pole_src );
  zCVecFree( zero_est );
  zCVecFree( pole_est );
}

int main(int argc, char *argv[])
{
  assert_zeropole();
  return 0;
}
