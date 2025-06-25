#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if(pid == 0){
        printf("Novo processo: %d\n", getpid());
        sleep(60);
    }else{
        printf("Processo original: %d\n", getpid());
        wait(NULL);
    }
    return 0;
}