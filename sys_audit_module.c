
/**
 * system call kernel module.
 **/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/time.h>
#include <linux/cred.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

#define COMM_SIZE 16

struct syscall_buf {
    u32 serial;
    //u32 ts_sec;
    //u32 ts_micro;
    u32 syscall;
    u32 status;
    pid_t pid;
    uid_t uid;
    u8 comm[COMM_SIZE];
};
DECLARE_WAIT_QUEUE_HEAD(buffer_wait);

#define AUDIT_BUF_SIZE 100
static struct syscall_buf audit_buf[AUDIT_BUF_SIZE];
static int current_pos = 0;
static u32 serial = 0;

/** FUNCTION: save this time system call **/
void syscall_audit(int syscall, int return_status)
{
    struct syscall_buf * ppb_temp;

    if(current_pos < AUDIT_BUF_SIZE) {
        ppb_temp = &audit_buf[current_pos];
        ppb_temp->serial = serial++;
        //ppb_temp->ts_sec = xtime.tv_sec;
        //ppb_temp->ts_micro = xtime.tv_usec;
        ppb_temp->syscall = syscall;
        ppb_temp->status = return_status;
        ppb_temp->pid = current->pid;
        //ppb_temp->uid = current->uid;
        ppb_temp->uid = current_uid().val;

        memcpy(ppb_temp->comm, current->comm, COMM_SIZE);

        if(current_pos++ == AUDIT_BUF_SIZE*8/10) {
            printk("IN MODULE_audit:yes, it near full\n");
            wake_up_interruptible(&buffer_wait);
        }
    }
}

/** fetch systemcall record **/
int sys_audit(u8 type, u8 * us_buf, u16 us_buf_size, u8 reset)
{
    int ret = 0;
    if(!type) {
        if(__clear_user(us_buf, us_buf_size)) {
            printk("Eror:claer_user\n");
            return 0;
        }
        printk("IN MOUDLE_systemcall:starting...\n");
        ret = wait_event_interruptible(buffer_wait, current_pos >= AUDIT_BUF_SIZE*8/10);
        printk("IN MOUDLE_systemcall:over, current_pos is %d\n", current_pos);
        if(__copy_to_user(us_buf, audit_buf, (current_pos)*sizeof(struct syscall_buf))) {
            printk("Error:copy error\n");
            return 0;
        }
        ret = current_pos - 1;
        current_pos = 0;
    }
    return ret;
}

extern void (*my_audit)(int, int);
extern int (*my_sysaudit)(unsigned char, unsigned char *, unsigned short, unsigned char);

static int __init audit_init(void)
{
    my_sysaudit = sys_audit;
    my_audit = syscall_audit;
    printk("Starting System Call Auditing\n");
    return 0;
}

static void __exit audit_exit(void)
{
    my_audit = NULL;
    my_sysaudit = NULL;
    printk("Exiting System Call Auditing\n");
    return;
}

module_init(audit_init);
module_exit(audit_exit);
MODULE_LICENSE("GPL");


