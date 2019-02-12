
//user_audit.c
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>

typedef unsigned char u8;
typedef unsigned int u32;

#define COMM_SIZE 16

struct syscall_buf {
    u32 serial;
    u32 syscall;
    u32 status;
    pid_t pid;
    uid_t uid;
    u8 comm[COMM_SIZE];
};

#define AUDIT_BUF_SIZE 100*sizeof(struct syscall_buf)

int main(int argc, char *argv[])
{
    u8 col_buf[AUDIT_BUF_SIZE];
    unsigned char reset = 1;
    int num = 0;
    struct syscall_buf *p;
    while(1) {
        num = syscall(317, 0, col_buf, AUDIT_BUF_SIZE, reset);
        printf("num:%d\n",num);
        u8 j = 0;
        int i;
        p = (struct syscall_buf *)col_buf;
        for(i = 0; i < num; i++) {
            printf("num[%d], serial:%d\t",i, p[i].serial);
            printf("syscall:%d\n", p[i].syscall);
            printf("status:%d\n", ((struct syscall_buf *)col_buf)[i].status);
            printf("pid:%d\n", ((struct syscall_buf *)col_buf)[i].pid);
            printf("uid:%d\n", ((struct syscall_buf *)col_buf)[i].uid);
            printf("comm:%s\n", ((struct syscall_buf *)col_buf)[i].comm);
        }
    }
    return 1;
}
