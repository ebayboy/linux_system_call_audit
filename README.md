# linux_system_call_audit
linux_system_call_audit

article:
https://blog.csdn.net/bboxhe/article/details/50011899

pre todo:
load linux kernel and unzip to dir /usr/src/linux

1. cp sys_call_audit /usr/src/linux/arch/x86/kernel/myaudit.c

2. edit kernel Makefile:

obj-y               += vsmp_64.o
obj-y               += myaudit.o    #add line

3. add system call number to arch/x86/syscalls/syscall_64.tbl
315 common  sched_getattr       sys_sched_getattr

#the next is new add 
316 common  mysyscall       sys_syscall_audit
317 common  myaudit         sys_myaudit

4. add declare to include/linux/syscalls.h
#add myaudit
asmlinkage void sys_syscall_audit(int syscall,int return_status);
asmlinkage int sys_myaudit(u8 type, u8 * us_buf, u16 us_buf_size, u8 reset);

5. build new kernel source 
