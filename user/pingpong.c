#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int pid;
  int pfds[2];                  /* parent to child */
  int cfds[2];                  /* child to parent */
  char buf = 'x';

  if (pipe(pfds) < 0 || pipe(cfds) < 0) {
    fprintf(2, "pingpong: cannot open pipe\n");
    exit(1);
  }

  if ((pid = fork()) < 0) {
    fprintf(2, "pingpong: cannot fork child\n");
    exit(1);
  } else if (pid == 0) {        /* child */
    close(pfds[1]);
    close(cfds[0]);
    if (read(pfds[0], &buf, 1) < 0) {
      fprintf(2, "pingpong: cannot read pipe\n");
      exit(1);
    }
    printf("%d: received ping\n", getpid());
    if (write(cfds[1], &buf, 1) < 0) {
      fprintf(2, "pingpong: cannot write pipe\n");
      exit(1);
    }
  } else {                      /* parent */
    close(pfds[0]);
    close(cfds[1]);
    if (write(pfds[1], &buf, 1) < 0) {
      fprintf(2, "pingpong: cannot write pipe\n");
      exit(1);
    }
    if (read(cfds[0], &buf, 1) < 0) {
      fprintf(2, "pingpong: cannot read pipe\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());
  }
  exit(0);
}
