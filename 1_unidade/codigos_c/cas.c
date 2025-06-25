/*
cas - 

Melhoria do código Mutex

Aula dia 08 de Maio de 2025
*/



#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdatomic.h> 

uint64_t valor =0;
atomic_bool trava = false;
static long int g =0;


void enter_region(void){
    bool v;
    do{
        v = false;
    } while(!atomic_compare_exchange_strong(&trava, &v, true));
}

void leave_region(void){
    atomic_store(&trava, false);
}

void *thread(void *arg) {
    printf("olá da thread\n");
    for (int i=0; i< 1e8; i+= 1){
        enter_region();
        g = g + 1;
        leave_region();
    }
    return NULL;
}

int main(void) {
    pthread_t th1;
    pthread_t th2;

    int rc1 = pthread_create(&th1, NULL, thread, NULL);
    int rc2 = pthread_create(&th2, NULL, thread, NULL);

    if (rc1 != 0) {
        fprintf(stderr, "Erro ao criar thread1: %d\n", rc1);
        exit(1);
    }
    if (rc2 != 0) {
        fprintf(stderr, "Erro ao criar thread2: %d\n", rc2);
        exit(1);
    }

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    printf("Valor final do contador: %d\n", g);

    return 0;
}