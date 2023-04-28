#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void sig_handler(int sig) {
  switch (sig) {
    case SIGUSR1:
      printf("Sinal SIGUSR1 recebido, finalizando execução...\n");
      exit(0);
      break;
    case SIGUSR2:
      printf("Sinal SIGUSR2 recebido, finalizando execução...\n");
      exit(2);
      break;
    case SIGINT:
      printf("Sinal SIGINT recebido, finalizando execução...\n");
      exit(0);
      break;
    default:
      printf("Sinal recebido desconhecido\n");
      break;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Uso: %s [busy|blocking]\n", argv[0]);
    exit(1);
  }

  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = &sig_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGUSR2, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);

  if (strcmp(argv[1], "busy") == 0) {
    printf("Aguardando por sinais (busy wait)...\n");
    while (1)
      ;
  } else if (strcmp(argv[1], "blocking") == 0) {
    printf("Aguardando por sinais (blocking wait)...\n");
    while (1)
      sleep(1);
  } else {
    printf("Opção inválida: %s\n", argv[1]);
    exit(1);
  }

  return 0;
}