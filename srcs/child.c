#include "../includes/ft_strace.h"

void    exec_child_prog(char **argv, char *full_path, char **env) {

    execve(full_path, &argv[1], env);
    perror("Erreur d'exécution\n");
    exit(1);
}