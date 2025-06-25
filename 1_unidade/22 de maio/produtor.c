void *produtor(void *arg){
    int v;
    for(v=1;;v++){
        travar(&mutex_fila);
        while((inserir +1) %TAMANHO) == remover){
            destravar(&mutex_fila);
            travar(&mutex_fila);
        }
        printf("Produzindo %d\n", v);
        dados[inserir] = v;
        inserir = (inserir + 1) % TAMANHO;
        destravar(&mutex_fila);

        sem_incrementar(&sem_fila);
        usleep(50000);

    }
}

void *consumidor(void *arg){
    for(::){
        sem_decrementar(&sem_fila);
        travar(&mutex_fila);
        printf("%zu: Consumindo%d\n", (size_t)arg, dados(remover));
        remover = (remover +1) % TAMANHO;
        destravar(&mutex_fila);
    }

    return NULL;
}

// API 
struct condvar;
void cv_inicializr(struct condvar *c);
void cv_acordar_um(struct condvar *c);
void cv_acorda_todos(struct condvar *c);
void cv_esperar(struct condvar *c, struct mutex *m);