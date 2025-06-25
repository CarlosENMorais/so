#include "helper.h"
#include <stdint.h>

uint32_t raiz[1024] __attribute__((aligned(4096))) = {
    (1 << 7) | (1 << 1 ) | (1 << 0)
};

int global = 0x10;
void teste(void){
    int *ptr1 = &global;
    int *ptr2 = (int *)((char *)ptr1 + 4 * 1024 * 1024);

    raiz[1] = raiz[0];
    
    puts("Endereçços de ptr1 e ptr2: ");
    put_hex((uint32_t)ptr1);
    puts(" ");
    put_hex((uint32_t)ptr2);
    puts("\n");

    puts("valores em ptr1 e ptr2: ");
    put_hex((uint32_t)*ptr1);
    puts(" ");
    put_hex((uint32_t)*ptr2);
    puts("\n");
    
    puts("valores em ptr1 e ptr2: ");
    *ptr1 = 0x30;
    put_hex((uint32_t)*ptr1);
    puts(" ");
    put_hex((uint32_t)*ptr2);
    puts("\n");
}

void enable_pagination(void){
    set_cr3((uint32_t)&raiz[0]);
    set_cr4(get_cr4() | ( 1 << 4 ));
    set_cr0(get_cr0() | ( 1 << 31 ));

    puts("Paginacao habilitada!\n");
}

int main(){
	puts("\nOlá mundo!\n");

    enable_pagination();

	for(;;);
}
