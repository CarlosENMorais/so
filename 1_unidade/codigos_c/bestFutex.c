/*
best_Futex

Melhoria do código Mutex

Aula dia 08 de Maio de 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <stdatomic.h> 
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>



_Atomic uint32_t trava = 0;
void enter_region(void){
    uint32_t v = 0;
    if(atomic_compare_exchange_strong(&trava, &v, 1)){
        return;
    }

    do{
        if (v ==2 || atomic_compare_exchange_strong(&trava, &v, 2)) {
            syscall(SYS_futex, &trava, FUTEX_WAIT, 2);
        }
        v = 0;
    } while (!atomic_compare_exchange_strong(&trava, &v, 2));
}

void leave_region(void){
    uint32_t v = atomic_fetch_sub(&trava, 1);
    if(v != 1){
        atomic_store(&trava, 0);
        syscall(SYS_futex, &trava, FUTEX_WAKE, 1);
    }
}

static long int g =0;
void *thread(void *arg) {
    printf("olá da thread\n");
    for (int i=0; i< 1000000; i+= 1){
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

    printf("Valor final do contador: %ld\n", g);

    return 0;
}