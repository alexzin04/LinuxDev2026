#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static int error(int code, const char *what) {
  fprintf(stderr, "move: %s: %s\n", what, strerror(errno));
  return code;
}
int main(int argc, char **argv) {
  int in = -1, out = -1, rc = 0, owned = 0;
  struct stat src, dst;
  char buf[65536];
  ssize_t n;
  if (argc != 3) {
    fprintf(stderr, "Usage: move infile outfile\n");
    return 1;
  }
#define FAIL(code, msg)                                                        \
  do {                                                                         \
    rc = error(code, msg);                                                     \
    goto cleanup;                                                              \
  } while (0)
  in = open(argv[1], O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
  if (in < 0)
    FAIL(2, "open source");
  if (fstat(in, &src) < 0)
    FAIL(3, "stat source");
  if (!S_ISREG(src.st_mode)) {
    errno = EINVAL;
    FAIL(4, "source is not regular");
  }
  out = open(argv[2], O_WRONLY | O_CREAT | O_NOFOLLOW | O_NONBLOCK, 0666);
  if (out < 0)
    FAIL(5, "open destination");
  if (fstat(out, &dst) < 0)
    FAIL(6, "stat destination");
  if (src.st_dev == dst.st_dev && src.st_ino == dst.st_ino) {
    errno = EINVAL;
    FAIL(7, "source and destination are the same file");
  }
  if (!S_ISREG(dst.st_mode)) {
    errno = EINVAL;
    FAIL(8, "destination is not regular");
  }
  owned = 1;
  if (ftruncate(out, 0) < 0)
    FAIL(9, "truncate destination");
  while ((n = read(in, buf, sizeof(buf))) != 0) {
    if (n < 0)
      FAIL(10, "read source");
    for (ssize_t off = 0; off < n;) {
      ssize_t k = write(out, buf + off, (size_t)(n - off));
      if (k < 0)
        FAIL(11, "write destination");
      if (k == 0) {
        errno = EIO;
        FAIL(12, "zero-length write");
      }
      off += k;
    }
  }
  if (fsync(out) < 0)
    FAIL(13, "sync destination");
  {
    int fd = out;
    out = -1;
    if (close(fd) < 0)
      FAIL(14, "close destination");
  }
  {
    int fd = in;
    in = -1;
    if (close(fd) < 0)
      FAIL(15, "close source");
  }
  if (unlink(argv[1]) < 0)
    FAIL(16, "remove source");
  return 0;
cleanup:
  if (out >= 0 && close(out) < 0)
    rc = error(17, "cleanup: close destination");
  if (in >= 0 && close(in) < 0)
    rc = error(18, "cleanup: close source");
  if (owned && unlink(argv[2]) < 0)
    rc = error(19, "cleanup: remove destination");
  return rc;
}
