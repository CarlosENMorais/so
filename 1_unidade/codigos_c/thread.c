#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h> 

// Declaração da variável global

// pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para proteger a variável global

void *thread(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        // Bloqueia o acesso à variável global
        // pthread_mutex_lock(&counter_mutex);
        counter++; // Incrementa a variável global
        // Desbloqueia o acesso à variável global
        // pthread_mutex_unlock(&counter_mutex);
    }
    pthread_exit(NULL); // É uma boa prática explicitar o exit da thread
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

    printf("Valor final do contador: %d\n", counter);

    // Destrói o mutex (libera os recursos)
    // pthread_mutex_destroy(&counter_mutex);

    return 0;
}