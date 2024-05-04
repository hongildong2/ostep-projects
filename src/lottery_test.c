#include "types.h"
#include "pstat.h"
#include "user.h"

int main(void)
{
    char* p = 0;
    printf(1, "%c\n", *p);
    
    printf(1, "my pid is %d\n", getpid());
    
    int child = fork();
    int ticket = 300;
    int sleep_time = 40;
    if (child == -1) {
        exit();
    } else if (child == 0) {
        // i am child
        ticket = 500;
        sleep_time = 100;
    } else {
        // i am your parent, ticket is 500
        ticket = 6000;
    }

    if (settickets(ticket) < 0) {
        printf(1, "fuck you\n");
        return 1;
    }

    for (int i = 0; i < 80; ++i) {
        printf(1, "MY PID IS %d\n", child);
        sleep(sleep_time);
    }

    struct pstat info;
    if (getpinfo(&info) < 0) {
        printf(1, "fuck you pstat\n");
        exit();
    }

    for (int i = 0; i < 20; ++i) {
        printf(1, "pstat-> inuse:%d, pid:%d, tickets:%d, ticks:%d\n", info.inuse[i], info.pid[i], info.tickets[i], info.ticks[i]);
        if (info.inuse[i] == 1) {
            break;
        }
    }

    // 무적권 일엏개해야됨
    exit();
}