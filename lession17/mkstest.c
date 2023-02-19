#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include "../err.h"

void make_temp(char *template);

int main() {
    char good_template[] = "/tmp/dirxxxx";
    char *bad_template = "/tmp/dirxxx1x";

    printf("trying to create first temp file\n");
    make_temp(good_template);
}

void make_temp(char *template) {
    int fd;
    struct stat sbuf;
    if ((fd = stat(template, &sbuf)) < 0) 
        err_ret("")
}