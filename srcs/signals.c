#include "../includes/ft_strace.h"

int get_status(int status, int child_pid) {
    siginfo_t siginfo;
    
    if (WIFEXITED(status) == 1) {
        dprintf(2, "\n+++ Child exited with code %d +++\n", WEXITSTATUS(status));
        exit (WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status) == 1) {
        #ifdef WCOREDUMP
        if (WCOREDUMP(status) == 1) {
            dprintf(2, "Child process error: Coredump\n");
            return (status);
        }
        #endif
        return (status);
    }
    else if (WIFSTOPPED(status) == 1) {
        if (WSTOPSIG(status) == (SIGTRAP | 0x80)) {
            if (ptrace(PTRACE_GETSIGINFO, child_pid, 0, &siginfo) == -1) {
                    perror("Erreur lors de l'obtention des informations sur le signal");
                    exit(1);
                }
                dprintf(2, "Signal reçu : %d\n", siginfo.si_signo);
                dprintf(2, "Code du signal : %d\n", siginfo.si_code);
                dprintf(2, "PID de l'expéditeur du signal : %d\n", siginfo.si_pid);
                dprintf(2, "Nom du signal : %s\n", strsignal(siginfo.si_signo));
            return (status);
        }
    }
    else if (WIFCONTINUED(status) == 1) {
        dprintf(2, "Child process was resumed by delivery of SIGCONT\n");
        return (0);
    }
    return (0);
}