#include "../includes/ft_strace.h"

void set_signals(int child_pid, int *status) {
	sigset_t		set;

	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);
	waitpid(child_pid, status, 0);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGPIPE);
	sigaddset(&set, SIGTERM);
	sigprocmask(SIG_BLOCK, &set, NULL);
}

int    wait_for_pid(int child_pid) {
	int status = 0;
	int res = 0;
	
	set_signals(child_pid, &status);
	res = get_status(status, child_pid);
	return (res != 0 ? res : 0);
}

void     exit_if_ptrace_error(long int ptrace_res, char *mess) {
    if (ptrace_res == -1) {
        perror(mess);
        exit(1);
    }
}