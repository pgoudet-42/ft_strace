#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <signal.h>
#include <sys/uio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/reg.h>
#include <elf.h>
#include "signals.h"
#include "syscall_64.h"
#include "syscalls_32.h"
#include "type.h"


#ifndef FT_STRACE_H
#define FT_STRACE_H


struct i386_user_regs_struct {
	int		ebx;
	int		ecx;
	int		edx;
	int		esi;
	int		edi;
	int		ebp;
	int		eax;
	int		xds;
	int		xes;
	int		xfs;
	int		xgs;
	int		orig_eax;
	int		eip;
	int		xcs;
	int		eflags;
	int		esp;
	int		xss;
};

typedef struct	syscall_handle_s {
	bool	start;
	bool	print;
	bool	result;
	int		mode;
}				syscall_handle_t;

struct		s_errno {
	char	*define;
	char	*desc;
};

extern int						n_envp;
extern const char				*sys_signame[];
extern const syscall_t			x86_64_syscall[];
extern const struct s_errno		g_errno_table[135];

char    *check_args(int argc, char **argv);

void    exec_child_prog(char **argv, char *full_path, char **env);

void    exit_if_ptrace_error(long int ptrace_res, char *mess);
int    wait_for_pid(int child_pid);
void	set_signals(int child_pid, int *status);

int     get_status(int status, int child_pid);

void	print_syscall(pid_t pid, syscall_t syscall, int argc, ...);
void	print_siginfo(siginfo_t *si);
void	print_argv(char **argv);
char	*escape(uint8_t *buffer, size_t size);
void	print_string(pid_t pid, va_list ap);
void	print_flag(int flags);
void	print_ptr(void *ptr);


#endif