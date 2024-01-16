#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
  if (argc == 2) {
    printf(1, "%s - Trace Enabled %d\n", argv[1], trace(argv[1]));
  }
  printf(1, "Trace Count %d\n", gettracecount());
  // printf(1, "Hello World %d\n", getreadcount());
  exit();
}
