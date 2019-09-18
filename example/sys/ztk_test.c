#include <dzco/dz_sys.h>

int main(int argc, char *argv[])
{
  dzSysArray arr;

  dzSysArrayScanZTK( &arr, argc > 1 ? argv[1] : "scan_test.ztk" );
  dzSysArrayFPrintZTK( stdout, &arr );
  dzSysArrayDestroy( &arr );
  return 0;
}
