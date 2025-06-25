#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1){
        perror("Erro ao criar o pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0){
        perror("Erro ao criar o processo filho");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){
        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);        
        close(fd[1]);
        
        execlp("ls", "ls", "-l", (char *)NULL);

        perror("Erro ao executar execlp");
        exit(EXIT_FAILURE);
    } else {
        close(fd[1]);

        char buffer[BUFFER_SIZE];
        ssize_t bytesRead;        
        while ((bytesRead = read(fd[0], buffer, BUFFER_SIZE - 1)) > 0) {
            buffer[bytesRead] = '\0'; 
            printf("%s", buffer);
            
            printf("Mais um ciclo de leitura! \n");
        }

        wait(NULL);
    }

    return 0;
}
