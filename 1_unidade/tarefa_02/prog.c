#include <unistd.h>

int main() {
    const char *mensagem = "Olá, mundo!\n";
    write(1, mensagem, 13);
    return 0;
}