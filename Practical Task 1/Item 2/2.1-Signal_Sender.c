#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Erro: informe o PID do processo destino e o número do sinal a ser enviado.\n");
        exit(1);
    }
    
    int pid = atoi(argv[1]);
    int signalNumber = atoi(argv[2]);
    
    if (kill(pid, 0) != 0) {
        printf("Erro: processo destino não encontrado.\n");
        exit(1);
    }
    
    if (kill(pid, signalNumber) != 0) {
        printf("Erro: não foi possível enviar o sinal.\n");
        exit(1);
    }
    
    printf("Sinal enviado com sucesso para o processo de PID %d.\n", pid);
    return 0;
}
