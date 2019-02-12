#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <asm/current.h>
#include <linux/sched.h>

void (* my_audit)(int, int) = 0;
asmlinkage void sys_syscall_audit(int syscall,int  return_status)
{
    if(my_audit) {    
        return (* my_audit)(syscall, return_status);
    }

    //如果钩子函数没有挂在则输出printk信息
    printk("IN KERNEL:%s(%d), syscall:%d, return:%d\n", current->comm, current->pid, syscall, return_status);

    return;
}

int (* my_sysaudit)(u8, u8 *, u16, u8) = 0;

asmlinkage int sys_myaudit(u8 type, u8 * us_buf, u16 us_buf_size, u8 reset)
{
    if(my_sysaudit) {
        return (* my_sysaudit)(type, us_buf, us_buf_size, reset);
    }

    printk("IN KERNEL:my system call sysaudit() working\n");

    return 0;
}

EXPORT_SYMBOL(my_audit);
EXPORT_SYMBOL(my_sysaudit);

