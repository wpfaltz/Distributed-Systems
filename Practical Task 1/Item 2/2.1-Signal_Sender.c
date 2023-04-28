#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s pid signal_number\n", argv[0]);
    exit(1);
  }

  int pid = atoi(argv[1]);
  int sig = atoi(argv[2]);

  if (kill(pid, sig) < 0) {
    perror("kill");
    exit(1);
  }

  printf("Sinal %d enviado ao processo %d\n", sig, pid);

  return 0;
}
