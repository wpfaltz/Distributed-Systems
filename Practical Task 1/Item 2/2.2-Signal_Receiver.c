#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

volatile int keepRunning = 1;

void handleSignal(int signalNumber) {
    switch(signalNumber) {
        case SIGUSR1:
            printf("Recebi o sinal SIGUSR1.\n");
            break;
        case SIGUSR2:
            printf("Recebi o sinal SIGUSR2.\n");
            break;
        case SIGTERM:
            printf("Recebi o sinal SIGTERM. Terminando...\n");
            keepRunning = 0;
            break;
        default:
            printf("Recebi um sinal desconhecido: %d\n", signalNumber);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Erro: informe o modo de espera (busy ou blocking).\n");
        exit(1);
    }
    
    char *waitMode = argv[1];
    
    struct sigaction action;
    action.sa_handler = handleSignal;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    
    if (strcmp(waitMode, "busy") == 0) {
      printf("Aguardando por sinais (busy wait)...\n");
      while(keepRunning) {}
    }
    else if (strcmp(waitMode, "blocking") == 0) {
      printf("Aguardando por sinais (blocking wait)...\n");
      while(keepRunning) {
        pause();
      }
    }
    else {
      printf("Erro: modo de espera inválido.\n");
      exit(1);
    }
    
    printf("Programa terminado.\n");
    return 0;
}