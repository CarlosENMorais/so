#include <stdio.h>
#include <unistd.h>

int main(){
    printf("My ID is: %d\n", getpid());
    printf("My father ID is: %d\n", getppid());
    return 0;
}