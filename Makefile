SRCS = srcs/main.c srcs/errors.c srcs/child.c srcs/utils.c srcs/signals.c srcs/print.c srcs/print_types.c
NAME = ft_strace
FLAGS = -Wall -Wextra -Werror -D_GNU_SOURCE
INCLUDES = includes/ft_strace.h includes/signals.h includes/syscall_64.h includes/type.h


OBJS = ${SRCS:.c=.o}

%.o : %.c ${INCLUDES}
	gcc ${FLAGS} -c $< -o $@ 

${NAME}: ${OBJS}
	gcc $(FLAGS) $(SRCS) -o ${NAME} -lm

all: $(NAME)

clean :
	rm -f $(OBJS)

fclean :	clean
	rm -f $(NAME)

re: fclean all

.PHONY:	all fclean clean re
