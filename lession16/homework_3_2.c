#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define EXEC_SUCCESS 0
#define EXEC_FAILURE -1

int my_dup2(int des1, int des2) {
    int openfileMax = sysconf(_SC_OPEN_MAX);
    int n, index, i;
    index = 0;
    int stack[openfileMax];
    if (des2 > openfileMax || des2 < 0) {
        printf("invalid filedes2\n");
        return EXEC_FAILURE;
    }
    if (des2 == des1) {
        return des2;
    }
    while ((n = dup(des1)) < des2) {
        if (n == -1) {
            printf("System can not make a friends!\n");
            return EXEC_FAILURE;
        }
        stack[index++] = n;
    }
    close(des2);
    if (dup(des1) == -1) {
        printf("dup function error!\n");
        return EXEC_FAILURE;
    }
    for (i = 0; i < index; i++) {
        close(stack[i]);
    }
    return des2;
}

int main() {
}