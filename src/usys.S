#include "syscall.h"
#include "traps.h"

// 전처리기가 SYSCALL(read) 들어오면, $SYS_read로 바꿔주고, syscall.h에 define 된걸 또 가져옴. 그래서 eax에 적절한 시스템콜 번호가 들어가게됨
// 유저프로세스는 user.h에 있는 함수 시그니처만 보고 호출하고, 링커가 그 시그니처랑 이 라벨된 어셈블리 트랩 entry랑 연결하는 듯.

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(getreadcount)
SYSCALL(settickets)
SYSCALL(getpinfo)
SYSCALL(thread_create)
