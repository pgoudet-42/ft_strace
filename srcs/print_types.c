#include "../includes/ft_strace.h"

void			print_siginfo(siginfo_t *si) {
	char	*si_c;
	char	*si_code[] = SI_CODE;
	char	*ill_si_code[] = ILL_SI_CODE;
	char	*fpe_si_code[] = FPE_SI_CODE;
	char	*segv_si_code[] = SEGV_SI_CODE;
	char	*bus_si_code[] = BUS_SI_CODE;
	char	*trap_si_code[] = TRAP_SI_CODE;
	char	*cld_si_code[] = CLD_SI_CODE;
	char	*poll_si_code[] = POLL_SI_CODE;

	if (si->si_signo == SIGILL && si->si_code < (int)(sizeof(ill_si_code) / sizeof(char *)))
		si_c = ill_si_code[si->si_code];
	else if (si->si_signo == SIGFPE && si->si_code < (int)(sizeof(fpe_si_code) / sizeof(char *)))
		si_c = fpe_si_code[si->si_code];
	else if (si->si_signo == SIGSEGV && si->si_code < (int)(sizeof(segv_si_code) / sizeof(char *)))
		si_c = segv_si_code[si->si_code];
	else if (si->si_signo == SIGBUS && si->si_code < (int)(sizeof(bus_si_code) / sizeof(char *)))
		si_c = bus_si_code[si->si_code];
	else if (si->si_signo == SIGTRAP && si->si_code < (int)(sizeof(trap_si_code) / sizeof(char *)))
		si_c = trap_si_code[si->si_code];
	else if (si->si_signo == SIGCHLD && si->si_code < (int)(sizeof(cld_si_code) / sizeof(char *)))
		si_c = cld_si_code[si->si_code];
	else if (si->si_signo == SIGPOLL && si->si_code < (int)(sizeof(poll_si_code) / sizeof(char *)))
		si_c = poll_si_code[si->si_code];
	else
	{
		if (si->si_code == SI_USER)
			si_c = si_code[1];
		else if (si->si_code == SI_KERNEL)
			si_c = si_code[2];
		else if (si->si_code == SI_QUEUE)
			si_c = si_code[3];
		else if (si->si_code == SI_TIMER)
			si_c = si_code[4];
		else if (si->si_code == SI_MESGQ)
			si_c = si_code[5];
		else if (si->si_code == SI_ASYNCIO)
			si_c = si_code[6];
		else if (si->si_code == SI_SIGIO)
			si_c = si_code[7];
		else if (si->si_code == SI_TKILL)
			si_c = si_code[8];
		else
			si_c = si_code[0];
	}
	dprintf(2, "--- %s si_signo=%s, si_code=%s, si_addr=%p ---\n", sys_signame[si->si_signo], sys_signame[si->si_signo], si_c, si->si_addr);
}

void			print_argv(char **argv) {
	int i = 0;
	dprintf(2, "[");
	while (argv[i])
	{
		if (i != 0)
			dprintf(2, ", ");
		dprintf(2, "\"%s\"", argv[i]);
		i++;
	}
	dprintf(2, "]");
}

char			*escape(uint8_t *buffer, size_t size) {
	int			l;
	char		*dest;

	dest = calloc(size * 4 + 1, sizeof(char));
	if (!dest)
		return (NULL);
	l = 0;
	for (size_t i = 0; i < size; i++) {
		if (buffer[i] == '\n')
			l += sprintf(dest + l, "\\n");
		else if (buffer[i] == '\t')
			l += sprintf(dest + l, "\\t");
		else if (buffer[i] == '\r')
			l += sprintf(dest + l, "\\r");
		else if (buffer[i] == '\v')
			l += sprintf(dest + l, "\\v");
		else if (buffer[i] == '\f')
			l += sprintf(dest + l, "\\f");
		else if (buffer[i] < 32 || buffer[i] > 126)
			l += sprintf(dest + l, "\\%d", buffer[i]);
		else
			dest[l++] = buffer[i];
	}
	return (dest);
}

void			print_string(pid_t pid, va_list ap) {
	ssize_t			nread;
	struct iovec	local[1];
	struct iovec	remote[1];

	local[0].iov_base = calloc(4096, sizeof(char));
	if (!local[0].iov_base)
		return ;
	local[0].iov_len = 4096;
	remote[0].iov_base = (void *)va_arg(ap, void *);
	remote[0].iov_len = 4096;
	nread = process_vm_readv(pid, local, 1, remote, 1, 0);
	if (nread < 0)
		dprintf(2, "%#lx", (long unsigned int)remote[0].iov_base);
	else
	{
		int len = memchr(local[0].iov_base, 0, 4096) - local[0].iov_base;
		char *escaped = escape(local[0].iov_base, len);
		if (!escaped) {
			free(local[0].iov_base);
			return ;
		}
		if (len > 48)
			dprintf(2, "\"%.32s\"...", escaped);
		else
			dprintf(2, "\"%s\"", escaped);
		free(escaped);
	}
	free(local[0].iov_base);
}

void			print_flag(int flags) {
	bool	first = true;
	int		flag_list[] = {O_APPEND, O_ASYNC, O_CLOEXEC, O_CREAT, O_DIRECT,
							O_DIRECTORY, O_DSYNC, O_EXCL, O_LARGEFILE, O_NOATIME, O_NOCTTY, O_NOFOLLOW,
							O_NONBLOCK, O_PATH, O_SYNC, O_TMPFILE, O_TRUNC};
	char	*str[] = {"O_APPEND", "O_ASYNC", "O_CLOEXEC", "O_CREAT", "O_DIRECT",
						"O_DIRECTORY", "O_DSYNC", "O_EXCL", "O_LARGEFILE", "O_NOATIME", "O_NOCTTY",
						"O_NOFOLLOW", "O_NONBLOCK", "O_PATH", "O_SYNC", "O_TMPFILE", "O_TRUNC"};

	for (size_t i = 0; i < sizeof(flag_list) / sizeof(int); i++)
	{
		if (flags & flag_list[i])
		{
			if (!first)
				dprintf(2, "|%s", str[i]);
			else
				dprintf(2, "%s", str[i]);
			first = false;
		}
	}
	if (!first)
		dprintf(2, "|");
	if (flags & O_WRONLY)
		dprintf(2, "O_WRONLY");
	else if (flags & O_RDWR)
		dprintf(2, "O_RDWR");
	else
		dprintf(2, "O_RDONLY");
}

void			print_ptr(void *ptr) {
	if (!ptr)
		dprintf(2, "NULL");
	else
		dprintf(2, "%#lx", (long unsigned int)ptr);
}