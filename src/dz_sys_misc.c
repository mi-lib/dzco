/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 *
 * dz_sys_misc - miscellany systems
 */

#include <dzco/dz_sys.h>

static bool _dzSysFScanMI(FILE *fp, void *val, char *buf, bool *success);

bool _dzSysFScanMI(FILE *fp, void *val, char *buf, bool *success)
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

dzSys *dzSysFScanAdder(FILE *fp, dzSys *sys)
{
  int n = 2;

  zFieldFScan( fp, _dzSysFScanMI, &n );
  return dzSysCreateAdder( sys, n ) ? sys : NULL;
}

void dzSysFPrintAdder(FILE *fp, dzSys *sys)
{
  fprintf( fp, "in: %d\n", dzSysInputNum(sys) );
}

dzSysCom dz_sys_adder_com = {
  typestr: "adder",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateAdder,
  fscan: dzSysFScanAdder,
  fprint: dzSysFPrintAdder,
};

/* create an adder. */
bool dzSysCreateAdder(dzSys *sys, int n)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, n );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return false;
  }
  sys->com = &dz_sys_adder_com;
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

dzSys *dzSysFScanSubtr(FILE *fp, dzSys *sys)
{
  int n = 2;

  zFieldFScan( fp, _dzSysFScanMI, &n );
  return dzSysCreateSubtr( sys, n ) ? sys : NULL;
}

void dzSysFPrintSubtr(FILE *fp, dzSys *sys)
{
  fprintf( fp, "in: %d\n", dzSysInputNum(sys) );
}

dzSysCom dz_sys_subtr_com = {
  typestr: "subtr",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateSubtr,
  fscan: dzSysFScanSubtr,
  fprint: dzSysFPrintSubtr,
};

/* create a subtractor. */
bool dzSysCreateSubtr(dzSys *sys, int n)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, n );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ){
    ZALLOCERROR();
    return false;
  }
  sys->com = &dz_sys_subtr_com;
  return true;
}

/* ********************************************************** */
/* saturater
 * ********************************************************** */

static bool _dzSysFScanLimit(FILE *fp, void *val, char *buf, bool *success);

zVec dzSysUpdateLimit(dzSys *sys, double dt)
{
  dzSysOutputVal(sys,0) =
    zLimit( dzSysInputVal(sys,0), ((double*)sys->prp)[0], ((double*)sys->prp)[1] );
  return dzSysOutput(sys);
}

bool _dzSysFScanLimit(FILE *fp, void *val, char *buf, bool *success)
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

dzSys *dzSysFScanLimit(FILE *fp, dzSys *sys)
{
  double val[] = { -HUGE_VAL, HUGE_VAL };

  zFieldFScan( fp, _dzSysFScanLimit, val );
  return dzSysCreateLimit( sys, val[0], val[1] ) ? sys : NULL;
}

void dzSysFPrintLimit(FILE *fp, dzSys *sys)
{
  double max, min;

  max = ((double*)sys->prp)[0];
  min = ((double*)sys->prp)[1];
  if( max < min ) zSwap( double, max, min );
  fprintf( fp, "min: %g\n", min );
  fprintf( fp, "max: %g\n", max );
}

dzSysCom dz_sys_limit_com = {
  typestr: "limit",
  destroy: dzSysDestroyDefault,
  refresh: dzSysRefreshDefault,
  update: dzSysUpdateLimit,
  fscan: dzSysFScanLimit,
  fprint: dzSysFPrintLimit,
};

/* create a saturater. */
bool dzSysCreateLimit(dzSys *sys, double max, double min)
{
  dzSysInit( sys );
  dzSysAllocInput( sys, 1 );
  if( dzSysInputNum(sys) == 0 || !dzSysAllocOutput( sys, 1 ) ||
      !( sys->prp = zAlloc( double, 2 ) ) ){
    ZALLOCERROR();
    return false;
  }
  ((double *)sys->prp)[0] = zMin( max, min );
  ((double *)sys->prp)[1] = zMax( max, min );
  sys->com = &dz_sys_limit_com;
  return true;
}
