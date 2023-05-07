#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid, i, p;
  int rfd, fds[2];

  if (pipe(fds) < 0) {
    fprintf(2, "primes: cannot open pipe\n");
    exit(1);
  }

  if ((pid = fork()) < 0) {
    fprintf(2, "primes: cannot fork child\n");
    exit(1);
  } else if (pid == 0) {        /* child */
  child:
    rfd = fds[0];
    close(fds[1]);
    if (read(rfd, (void *)&p, 4) != 4) { /* last */
      close(rfd);
      exit(0);
    }
    printf("prime %d\n", p);
    if (pipe(fds) < 0) {
      fprintf(2, "primes: cannot open pipe\n");
      exit(1);
    }
    if ((pid = fork()) < 0) {
      fprintf(2, "primes: cannot fork child\n");
      exit(1);
    } else if (pid == 0) {      /* child.child */
      goto child;
    } else {                    /* child.parent */
      close(fds[0]);
      while (read(rfd, (void *)&i, 4) == 4)
        if (i % p != 0)
          write(fds[1], (void *)&i, 4);
      close(rfd);
      close(fds[1]);
      wait(0);
      exit(0);
    }
  } else {                      /* parent */
    close(fds[0]);
    for (i = 2; i < 36; i++)
      write(fds[1], (void *)&i, 4);
    close(fds[1]);
    wait(0);
    exit(0);
  }
}
