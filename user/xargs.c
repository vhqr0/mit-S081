#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  int i, cargc = argc - 1, len = 0, last_is_ws = 1, pid;
  char *cargv[MAXARG], buf[512], c;

  if (argc < 2) {
    fprintf(2, "Usage: xargs program args...\n");
    exit(1);
  }

  for (i = 0; i < cargc; i++)
    cargv[i] = argv[i + 1];

  while (read(0, &c, 1) == 1) {
    if (c == '\n' || c == '\t' || c == ' ') { /* ws */
      if (!last_is_ws) {
        buf[len++] = 0;
        last_is_ws = 1;
      }
      if (c == '\n') {          /* newline */
        if (cargc >= MAXARG) {
          fprintf(2, "xargs: args too many\n");
          exit(1);
        }
        cargv[cargc] = 0;
        if ((pid = fork()) < 0) {
          fprintf(2, "xargs: cannot fork child\n");
          exit(1);
        } else if (pid == 0) {  /* child */
          exec(argv[1], cargv);
        } else {                /* parent */
          wait(0);
          cargc = argc - 1;
          len = 0;
        }
      }
    } else {                    /* non-ws */
      buf[len++] = c;
      if (last_is_ws) {
        if (cargc >= MAXARG) {
          fprintf(2, "xargs: args too many\n");
          exit(1);
        }
        cargv[cargc++] = buf + len - 1;
      }
      last_is_ws = 0;
    }
  }
  if (len != 0) {
    if (cargc >= MAXARG) {
      fprintf(2, "xargs: args too many\n");
      exit(1);
    }
    cargv[cargc] = 0;
    if ((pid = fork()) < 0) {
      fprintf(2, "xargs: cannot fork child\n");
      exit(1);
    } else if (pid == 0) {      /* child */
      exec(argv[1], cargv);
    } else {                    /* parent */
      wait(0);
    }
  }
  exit(0);
}
