#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

_Atomic int valor = 0;

void* thread(void* arg){
    int i = 1000000;
    while(i--){
        valor++;
    }
}

int main(){
    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1, NULL, thread, NULL);
    pthread_create(&thread2, NULL, thread, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Valor = %d\n", valor);

    return 0;
}
