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

    printf(1, "page entry for address 5 is %p\n", getpagetableentry(pid, 5));
    printf(1, "page entry for address 4096 is %p\n", getpagetableentry(pid, 4096));
    printf(1, "page entry for address 5000 is %p\n", getpagetableentry(pid, 5000));
    printf(1, "page entry for address %p is %p\n", p,getpagetableentry(pid, (int)p));
    isphysicalpagefree(0x1000);
    exit();
}
