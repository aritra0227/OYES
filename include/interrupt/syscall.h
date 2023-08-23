#ifndef INCLUDE_SYSCALL_H
#define INCLUDE_SYSCALL_H

int sys_exec(unsigned int * program_vaddr);
unsigned int sys_get_pid(void);
unsigned int sys_fork();

#endif