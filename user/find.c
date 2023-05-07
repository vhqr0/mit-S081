#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void
find(char *buf, int len, int cap, char *filename) {
  int fd, dlen;
  struct dirent de;
  struct stat st;

  if ((fd = open(buf, 0)) < 0) {
    fprintf(2, "find: cannot open %s\n", buf);
    exit(1);
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", buf);
    close(fd);
    return;
  }
  if (st.type != T_DIR) {
    close(fd);
    return;
  }

  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0)
      continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;
    dlen = strlen(de.name);
    if (dlen > cap - len - 1) {
      printf("find: path too long\n");
      continue;
    }
    buf[len] = '/';
    memcpy(buf + len + 1, de.name, dlen);
    buf[len + dlen + 1] = 0;
    if (strcmp(de.name, filename) == 0)
      printf("%s\n", buf);
    find(buf, len + dlen + 1, cap, filename);
  }
}

int
main(int argc, char *argv[])
{
  int len;
  char buf[512];

  if (argc != 3) {
    fprintf(2, "Usage: find path filename\n");
    exit(1);
  }

  len = strlen(argv[1]);
  if (len > sizeof(buf) - 1) {
    printf("find: path too long\n");
    exit(0);
  }

  memcpy(buf, argv[1], len);
  buf[len] = 0;
  find(buf, len, sizeof(buf), argv[2]);
  exit(0);
}
