#include "types.h"
#include "user.h"
#include "pstat.h"

int main(int argc, char *argv[])
{
    int pid;
    if (argc > 1) {
        pid = atoi(argv[1]);
    } else {
        pid = getpid();
    }
    dumppagetable(pid);
    struct pstat *p = malloc(sizeof(struct pstat));
    getpinfo(p);
    dumppagetable(pid);
    getpagetableentry(pid, 0x1000);
    isphysicalpagefree(0x1000);
    exit();
}
