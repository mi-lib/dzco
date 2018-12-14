/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_misc - miscellany systems
 */

#include <dzco/dz_sys.h>

static bool _dzSysFReadMI(FILE *fp, void *val, char *buf, bool *success);

bool _dzSysFReadMI(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "in" ) == 0 ){
    *(int *)val = zFInt( fp );
    return true;
  }
  return false;
}

/* ********************************************************** */
/* adder
 * ********************************************************** */

zVec dzSysUpdateAdder(dzSys *sys, double dt)
{
  register int i;

  dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  for( i=1; i<dzSysInputNum(sys); i++ )
    dzSysOutputVal(sys,0) += dzSysInputVal(sys,i);
  return dzSysOutput(sys);
}

dzSys *dzSysFReadAdder(FILE *fp, dzSys *sys)
{
  int n = 2;

  zFieldFRead( fp, _dzSysFReadMI, &n );
  return dzSysCreateAdder( sys, n ) ? sys : NULL;
}

void dzSysFWriteAdder(FILE *fp, dzSys *sys)
{
  fprintf( fp, "in: %d\n", dzSysInputNum(sys) );
}

dzSysMethod dz_sys_adder_met = {
  type: "adder",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateAdder,
  fread: dzSysFReadAdder,
  fwrite: dzSysFWriteAdder,
};

/* dzSysCreateAdder
 * - create adder.
 */
bool dzSysCreateAdder(dzSys *sys, int n)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, n );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_adder_met;
  return true;
}

/* ********************************************************** */
/* subtractor
 * ********************************************************** */

zVec dzSysUpdateSubtr(dzSys *sys, double dt)
{
  register int i;

  dzSysOutputVal(sys,0) = dzSysInputVal(sys,0);
  for( i=1; i<dzSysInputNum(sys); i++ )
    dzSysOutputVal(sys,0) -= dzSysInputVal(sys,i);
  return dzSysOutput(sys);
}

dzSys *dzSysFReadSubtr(FILE *fp, dzSys *sys)
{
  int n = 2;

  zFieldFRead( fp, _dzSysFReadMI, &n );
  return dzSysCreateSubtr( sys, n ) ? sys : NULL;
}

void dzSysFWriteSubtr(FILE *fp, dzSys *sys)
{
  fprintf( fp, "in: %d\n", dzSysInputNum(sys) );
}

dzSysMethod dz_sys_subtr_met = {
  type: "subtr",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateSubtr,
  fread: dzSysFReadSubtr,
  fwrite: dzSysFWriteSubtr,
};

/* dzSysCreateSubtr
 * - create subtractor.
 */
bool dzSysCreateSubtr(dzSys *sys, int n)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, n );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return false;
  }
  sys->_met = &dz_sys_subtr_met;
  return true;
}

/* ********************************************************** */
/* saturater
 * ********************************************************** */

static bool _dzSysFReadLimit(FILE *fp, void *val, char *buf, bool *success);

zVec dzSysUpdateLimit(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) =
    zLimit( dzSysInputVal(sys,0), ((double*)sys->_prm)[0], ((double*)sys->_prm)[1] );
  return dzSysOutput(sys);
}

bool _dzSysFReadLimit(FILE *fp, void *val, char *buf, bool *success)
{
  if( strcmp( buf, "min" ) == 0 ){
    ((double *)val)[0] = zFDouble( fp );
  } else
  if( strcmp( buf, "max" ) == 0 ){
    ((double *)val)[1] = zFDouble( fp );
  } else
    return false;
  return true;
}

dzSys *dzSysFReadLimit(FILE *fp, dzSys *sys)
{
  double val[] = { -HUGE_VAL, HUGE_VAL };

  zFieldFRead( fp, _dzSysFReadLimit, val );
  return dzSysCreateLimit( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFWriteLimit(FILE *fp, dzSys *sys)
{
  double max, min;

  max = ((double*)sys->_prm)[0];
  min = ((double*)sys->_prm)[1];
  if( max < min ) zSwap( double, max, min );
  fprintf( fp, "min: %g\n", min );
  fprintf( fp, "max: %g\n", max );
}

dzSysMethod dz_sys_limit_met = {
  type: "limit",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateLimit,
  fread: dzSysFReadLimit,
  fwrite: dzSysFWriteLimit,
};

/* dzSysCreateLimit
 * - create saturater.
 */
bool dzSysCreateLimit(dzSys *sys, double max, double min)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->_prm = zAlloc( double, 2 ) ) ){
    ZALLOCERROR();
    return false;
  }
  ((double *)sys->_prm)[0] = zMin( max, min );
  ((double *)sys->_prm)[1] = zMax( max, min );
  sys->_met = &dz_sys_limit_met;
  return true;
}
