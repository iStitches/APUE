#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../err.h"

/*
    采用内存映射I/O 的方式进行文件复制
 */

#define COPYINCR (1024*1024*1024)  //1GB

int main(int argc, char *argv[])
{
    int fdin, fdout;
    void *src, *dst;
    size_t copysz;
    struct stat sbuf;
    off_t fsz = 0;

    if ((fdin = open(argv[1], O_RDONLY)) < 0)
        err_sys("can't open %s for reading", argv[1]);
    
    if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC)) < 0)
        err_sys("can't creat %s for writing", argv[2]);
    
    if (fstat(fdin, &sbuf) < 0)
        err_sys("fstat error");

    if (ftruncate(fdout, sbuf.st_size) < 0)
        err_sys("ftruncate error");
    
    while (fsz < sbuf.st_size)
    {
        if ((sbuf.st_size - fsz) > COPYINCR)
            copysz = COPYINCR;
        else 
            copysz = sbuf.st_size - fsz;
        if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED)
            err_sys("mmap error for input");
        if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, fsz)) == MAP_FAILED)
            err_sys("mmap error for output");
        memcpy(dst, src, copysz);
        munmap(src, copysz);
        munmap(dst, copysz);
        fsz += copysz;
    }
    exit(0);
}