#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <string.h>

int unlink(const char *path)
{
    int (*real_unlink)(const char *);
    const char *name = strrchr(path, '/');
    name = name ? name + 1 : path;
    if (strstr(name, "PROTECT")) { errno = EPERM; return -1; }
    /* POSIX specifies conversion of dlsym's result to a function pointer. */
    *(void **)(&real_unlink) = dlsym(RTLD_NEXT, "unlink");
    if (!real_unlink) { errno = ENOSYS; return -1; }
    return real_unlink(path);
}
