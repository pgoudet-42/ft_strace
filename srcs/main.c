#include "../includes/ft_strace.h"

int						n_envp;
const char				*sys_signame[] = SYS_SIGNAME;
const syscall_t			syscall_64[] = SYSCALL_64;
const syscall_t			syscall_32[] = SYSCALL_32;



int syscall_is_called_64(struct user_regs_struct *regs, int child_pid, int status) {
	struct iovec x86_io;
	int res = 0;

	x86_io.iov_base = regs;
	x86_io.iov_len = sizeof(struct user_regs_struct);
	exit_if_ptrace_error(ptrace(PTRACE_SYSCALL, child_pid, NULL, status), "Erreur lors de la reprise du processus");
	res = wait_for_pid(child_pid);
	if (res != 0)
		return (res);
	exit_if_ptrace_error(ptrace(PTRACE_GETREGSET, child_pid, (void*)NT_PRSTATUS, &x86_io), "Erreur lors de l'obtention des registres");
	if ((long long int)regs->rax < 0 && (long long int)regs->rax > -135) {
		dprintf(2, ") = %d %s %s\n", (int)regs->rax, g_errno_table[-1 * (int)regs->rax].define, g_errno_table[-1 * (int)regs->rax].desc);
	}
	else if (syscall_64[regs->orig_rax].type_ret == INT) {
		dprintf(2, ") = %d\n", (int)regs->rax);
	}
	else
		dprintf(2, ") = %#lx\n", (long unsigned int)regs->rax);
	return (0);
}

int syscall_is_called_32(struct i386_user_regs_struct *regs, int child_pid){
	struct iovec x86_io;
	int res = 0;

	x86_io.iov_base = regs;
	x86_io.iov_len = sizeof(struct i386_user_regs_struct);
	exit_if_ptrace_error(ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL), "Erreur lors de la reprise du processus");
	res = wait_for_pid(child_pid);
	if (res != 0)
		return (res);
	exit_if_ptrace_error(ptrace(PTRACE_GETREGSET, child_pid, (void*)NT_PRSTATUS, &x86_io), "Erreur lors de l'obtention des registres");
	if (syscall_32[regs->orig_eax].type_ret == INT)
		dprintf(2, ") = %d\n", (int)regs->eax);
	else
		dprintf(2, ") = %#lx\n", (long unsigned int)regs->eax);
	return (0);
}

int	handle_syscalls(int child_pid, int status) {
	struct user_regs_struct *regs;
	struct i386_user_regs_struct *regs_32;
	char buff[216];
	struct iovec x86_io;

	x86_io.iov_base = buff;
	x86_io.iov_len = 216;
	if(ptrace(PTRACE_GETREGSET, child_pid, (void*)NT_PRSTATUS, (void *)&x86_io) < 0)
		return (1);
	if (x86_io.iov_len == sizeof(struct user_regs_struct)) {
		regs = (struct user_regs_struct *)buff;
		if (regs->orig_rax < 334 && regs->rax == (long long unsigned int)-38) {
			print_syscall(child_pid, syscall_64[regs->orig_rax], syscall_64[regs->orig_rax].argc, regs->rdi, regs->rsi, regs->rdx, regs->r10, regs->r8, regs->r9);
			return (syscall_is_called_64(regs, child_pid, status));
		}
    } 
	else if (x86_io.iov_len == sizeof(struct i386_user_regs_struct)) {
		regs_32 = (struct i386_user_regs_struct *)buff;
		if (regs_32->orig_eax < 402 && regs_32->eax == -38) {
			print_syscall(child_pid, syscall_32[regs_32->orig_eax], syscall_32[regs_32->orig_eax].argc, regs_32->ebx, regs_32->ecx, regs_32->edx, regs_32->esi, regs_32->edi, regs_32->ebp);
			return (syscall_is_called_32(regs_32, child_pid));
		}
    }
	else {
		dprintf(2, "Error: Arch not supported\n");
		return (1);
	}
	return (0);
}

int    get_syscalls(pid_t child_pid) {
	siginfo_t	si;
	int status = 0;
	int res = 0;

	while (1) {
		if (ptrace(PTRACE_SYSCALL, child_pid, NULL, status) < 0)
			break;
		res = wait_for_pid(child_pid);
		if (res != 0)
			return (res);
		if (ptrace(PTRACE_GETSIGINFO, child_pid, NULL, &si) == 0 && si.si_signo != SIGTRAP) {
			status = si.si_signo;
			print_siginfo(&si);
		}
		else
			status = 0;
		if (handle_syscalls(child_pid, status) == 1)
			break;
	}
	return (0);
}

int    ptrace_calls(pid_t child_pid) {
	int res = 0;

	exit_if_ptrace_error(ptrace(PTRACE_SEIZE, child_pid, NULL, NULL), "Erreur lors de la saisie du processus");
	exit_if_ptrace_error(ptrace(PTRACE_INTERRUPT, child_pid, NULL, NULL), "Erreur lors de l'interruption du processus");
	res = wait_for_pid(child_pid);
	if (res != 0)
		return (res);
	exit_if_ptrace_error(ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACE_O_EXITKILL), "Erreur lors du passage des options\n");
	res = get_syscalls(child_pid);
	if (WIFSIGNALED(res)) {
		fprintf(stderr, "+++ killed by %s +++\n", sys_signame[WTERMSIG(res)]);
		kill(getpid(), WTERMSIG(res));
	}
	else
		fprintf(stderr, "+++ exited with %d +++\n", WEXITSTATUS(res));
	return (WEXITSTATUS(res));
}

int main(int argc, char **argv, char **env) {
	char *full_path;
	pid_t child_pid;
	int res = 0;

	full_path = check_args(argc, argv);
	if (full_path == NULL) {
		dprintf(2, "ft_strace: Can't stat \'%s\': No such file or directory\n", argv[1]);
		return (1);
	}

	child_pid = fork();

	if (child_pid == 0)
		exec_child_prog(argv, full_path, env);
	else
		res = ptrace_calls(child_pid);
	return (res);
}