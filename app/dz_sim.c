#include <dzco/dz_sys.h>

enum{
  OPT_SYSFILE=0, OPT_OUTPUTFILE, OPT_SCRIPTFILE,
  OPT_DT, OPT_T,
  OPT_OUTSYS,
  OPT_HELP,
  OPT_INVALID
};
zOption opt[] = {
  { "s", "sys", "<.ztk file>", "system definition file", NULL, false },
  { "o", "out", "<output file>", "result output file", (char *)"sim_out", false },
  { "s", "script", "<script file>", "script file to plot the result on gnuplot", (char *)"sim.plot", false },
  { "dt", "dt", "<value>", "discretized time step", (char *)"0.001", false },
  { "t",  "t", "<value>", "total simulation time", (char *)"1.0", false },
  { "os", "outsys", "<name>", "the name of output system", NULL, false },
  { "h", "help", NULL, "show this message", NULL, false },
  { NULL, NULL, NULL, NULL, NULL, false },
};

void dz_sim_usage(const char *arg)
{
  eprintf( "Usage: %s [option] <.ztk file> [output file] [script file]\n", arg );
  eprintf( "<options>\n" );
  zOptionHelp( opt );
  eprintf( "In order to plot the result, execute gnuplot and load the script file.\n" );
  exit( 0 );
}

bool dz_sim_commandarg(int argc, char *argv[])
{
  zStrAddrList arglist;

  if( !zOptionRead( opt, argv, &arglist ) ) return false;
  if( opt[OPT_HELP].flag ) dz_sim_usage( "dz_sim" );
  if( !zListIsEmpty(&arglist) ){
    opt[OPT_SYSFILE].flag = true;
    opt[OPT_SYSFILE].arg  = zListTail(&arglist)->data;
  }
  if( !opt[OPT_SYSFILE].flag ){
    ZRUNERROR( "system not specified" );
    return false;
  }
  zStrAddrListDestroy( &arglist );
  return true;
}

bool dz_sim_output(FILE *fp, dzSysArray *arr)
{
  double dt, t, term;
  dzSys *sys_out;
  int i;

  if( zIsTiny( ( dt = atof( opt[OPT_DT].arg ) ) ) ){
    ZRUNERROR( "too small discrete time step %g", dt );
    return false;
  }
  term = atof( opt[OPT_T].arg );
  if( !opt[OPT_OUTSYS].arg ||
      !( sys_out = dzSysArrayNameFind( arr, opt[OPT_OUTSYS].arg ) ) )
    sys_out = zArrayHead( arr );
  for( t=0; t<=term; t+=dt ){
    dzSysArrayUpdate( arr, dt );
    fprintf( fp, "%g", t );
    for( i=0; i<dzSysOutputNum(sys_out); i++ )
      fprintf( fp, " %g", dzSysOutputVal(sys_out,i) );
    fprintf( fp, "\n" );
  }
  return true;
}

void dz_sim_script(FILE *fp, char *logfile, double t)
{
  fprintf( fp, "clear\n" );
  fprintf( fp, "set grid\n" );
  fprintf( fp, "unset key\n" );
  fprintf( fp, "plot [0:%f] 1.0, '%s' u 1:2 w l\n", t, logfile );
}

int main(int argc, char *argv[])
{
  dzSysArray arr;
  FILE *fp;

  if( argc < 2 ) dz_sim_usage( argv[0] );
  if( !dz_sim_commandarg( argc, argv+1 ) ) return 1;
  if( !dzSysArrayReadZTK( &arr, opt[OPT_SYSFILE].arg ) ) return 1;

  fp = fopen( opt[OPT_OUTPUTFILE].arg, "w" );
  dz_sim_output( fp, &arr );
  fclose( fp );

  dzSysArrayDestroy( &arr );
  fp = fopen( opt[OPT_SCRIPTFILE].arg, "w" );
  dz_sim_script( fp, opt[OPT_OUTPUTFILE].arg, atof(opt[OPT_T].arg) );
  fclose( fp );
  return 0;
}
