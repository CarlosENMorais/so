#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>

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

#define TAMANHO 10

volatile int dados[TAMANHO];
volatile size_t inserir = 0;
volatile size_t remover = 0;

pthread_mutex_t mutex;

void* produtor(void* arg){
    int v;
    for(v = 1;; v++){
        enter_region();

        while(((inserir +1) % TAMANHO) == remover);

        printf("Produzindo %d\n", v);
        dados[inserir] = v;
        inserir = (inserir +1) % TAMANHO;

        leave_region();
        usleep(500000);
    }
    return NULL;
}

void* consumidor(void* arg){
    for(;;){
        enter_region();

        while(inserir == remover){
            leave_region();
            usleep(10000);
            enter_region();
        };

        printf("%zu: Consumindo %d\n", (size_t)arg, dados[remover]);
        remover = (remover +1) % TAMANHO;

        leave_region();
    }
    return NULL;
}

int main() {
    pthread_t produtor_thread, consumidor_thread1, consumidor_thread2;

    pthread_mutex_init(&mutex, NULL);

    if (pthread_create(&produtor_thread, NULL, produtor, NULL) != 0) {
        perror("Erro ao criar a thread do produtor");
        return 1;
    }

    if (pthread_create(&consumidor_thread1, NULL, consumidor, (void*)3) != 0 ||
        pthread_create(&consumidor_thread2, NULL, consumidor, (void*)4) != 0) {
        perror("Erro ao criar a thread do consumidor");
        return 1;
    }

    pthread_join(produtor_thread, NULL);
    pthread_join(consumidor_thread1, NULL);
    pthread_join(consumidor_thread2, NULL);
}
