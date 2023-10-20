#include "../includes/ft_strace.h"

void			print_syscall(pid_t pid, syscall_t syscall, int argc, ...) {
	va_list			ap;

	dprintf(2, "%s(", syscall.name);
	va_start(ap, argc);
	for (int i = 0; i < argc; i++)
	{
		if (syscall.type_args[i] == INT)
			dprintf(2, "%d", va_arg(ap, int));
		else if (syscall.type_args[i] == ULONG)
			dprintf(2, "%ld", va_arg(ap, unsigned long));
		else if (syscall.type_args[i] == ARGV)
			print_argv(va_arg(ap, char **));
		else if (syscall.type_args[i] == ENVP)
			dprintf(2, "%p /* %d vars */", va_arg(ap, void *), n_envp);
		else if (syscall.type_args[i] == STR)
			print_string(pid, ap);
		else if (syscall.type_args[i] == PTR)
			print_ptr(va_arg(ap, void *));
		else if (syscall.type_args[i] == SIGNAL)
		{
			int signo = va_arg(ap, int);
			if (signo < SYS_SIGNAME_COUNT)
				dprintf(2, "%s", sys_signame[signo]);
			else
				dprintf(2, "%d", i);
		}
		else if (syscall.type_args[i] == FLAG_OPEN)
			print_flag(va_arg(ap, int));
		else
			dprintf(2, "%#lx", va_arg(ap, unsigned long));
		if (i != argc - 1)
			dprintf(2, ", ");
	}
	va_end(ap);
}