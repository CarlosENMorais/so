#include <stdlib.h>
#include <pthread.h>

struct esperando{
    struct mutex m;
    struct esperando *prox;
};

struct semaforo {
    struct mutex trava;
    size_t valor;
    struct esperando *cabeca;
    struct esperando *cauda;
};

void sem_inicializar(struct semaforo *s){
    inicializar(&s->trava);
    s->valor = 0;
    s->cabeca = NULL;
    s->cauda = NULL;
}
void sem_incrementar(struct semaforo *s){
    struct esperando *esp;

    travar(&s->trava);
    esp = s->cabeca;
    if (esp != NULL) {
        s->cabeca = esp->prox;
        if(!s->cabeca){
            s->cauda=   NULL;
        }
    }
    s->valor++;
    destrava(&s->trava);

    if(esp!= NULL){
        destravar(&esp->m);
    }
}

void sem_dec(struct sem* s){
    struct espreando exp;

    for(::){
        travar(&s->trava);
        if(s->valor > 0){
            s->valor--;
            destravar(&s->trava);
            return;
        }
        inicializar(&esp.m);
        travar(&esp.m);
        esp.prox =  NULL;
        if(s -> cauda){
            s->cauda->prox = &esp;
        }else{
            s->cabeca = &esp;
        }

        s->cauda = &esp;

        destravar(&s->trava);
        travar(&esp.m);
    }
}

void sem_dec(struct semaforo *s){
    struct esperando *novo_esp;

    travar(&s -> trava);
    if(s -> valor > 0){
        s -> valor--;
    } else {
        novo_esp = (struct esperando *) malloc(sizeof(struct esperando));
        inicializar(&novo_esp->m);
        novo_esp->prox = NULL;

        if(s -> cauda == NULL){
            s -> cabeca = novo_esp;
            s -> cauda = novo_esp;
        } else {
            s -> cauda -> prox = novo_esp;
            s -> cauda = novo_esp;
        }

        destravar(&s -> trava);
        travar(&novo_esp->m);
    }
    destravar(&s -> trava);
}