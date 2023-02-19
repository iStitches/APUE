#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../err.h"

typedef int Myfunc(const char*, const struct stat*, int);

static Myfunc myfunc;
static int myftw(char*, Myfunc*);
static int dopath(Myfunc*);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[]) {

}

static char* fullpath;
static size_t pathlen;
#define FTW_F 1
#define FTW_D 2
#define FTW_DNR 3
#define FTW_NS 4

char* path_alloc(size_t *len) {
    char* res = (char*)calloc(1024 , sizeof(char));
    *len = 1024;
    return res;
}

static int myftw(char *pathname, Myfunc *func) {
    fullpath = path_alloc(&pathlen);
    if (pathlen <= strlen(pathname)) {
        pathlen = strlen(pathname) * 2;
        if (fullpath = realloc(fullpath, pathlen) == NULL) {
            err_sys("realloc failed");
        }
    }
    strcpy(fullpath, pathname);
    return dopath(func);
}

static int dopath(Myfunc *func) {
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;

}


static int myfunc(const char *pathname, const struct stat *statptr, int type) {
    switch (type)
    {
    case FTW_F:
        switch (statptr->st_mode & S_IFMT)
        {
        case __S_IFREG: nreg++; break;
        case __S_IFBLK: nblk++; break;
        case __S_IFCHR: nchr++; break;
        case __S_IFIFO: nfifo++; break;
        case __S_IFLNK: nslink++; break;
        case __S_IFSOCK: nsock++; break;
        case __S_IFDIR: err_sys("for S_ISDIR for %s", pathname);
        }
        break;
    case FTW_D:
        ndir++;
        break;
    case FTW_DNR:
        err_ret("can't read directory %s", pathname);
        break;
    case FTW_NS:
        err_ret("stat error for %s", pathname);
        break;
    default:
        err_sys("unknown type %d for pathname %s", type, pathname);
    }
    return 0;
}
