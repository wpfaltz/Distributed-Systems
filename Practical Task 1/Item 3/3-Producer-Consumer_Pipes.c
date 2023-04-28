#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define PIPE_READ 0
#define PIPE_WRITE 1
#define BUFFER_SIZE 20

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void producer(int pipefd[], int num_elements) {
    close(pipefd[PIPE_READ]);

    int current_number = 1;
    printf("%d enviado pelo produtor ao pipe\n", current_number);

    for (int i = 0; i < num_elements; i++) {
        int delta = (rand() % 100) + 1;
        printf("%d enviado pelo produtor ao pipe\n", current_number);

        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%d", current_number);
        write(pipefd[PIPE_WRITE], buffer, BUFFER_SIZE);
        current_number += delta;
    }

    char buffer[BUFFER_SIZE] = "0";
    write(pipefd[PIPE_WRITE], buffer, BUFFER_SIZE);

    close(pipefd[PIPE_WRITE]);
}

void consumer(int pipefd[]) {
    close(pipefd[PIPE_WRITE]);

    char buffer[BUFFER_SIZE];

    while (1) {
        read(pipefd[PIPE_READ], buffer, BUFFER_SIZE);
        int number = atoi(buffer);

        if (number == 0) {
            printf("Zero lido, interrompendo execução\n");
            break;
        }

        if (is_prime(number)) {
            printf("Número primo recebido pelo consumidor: %d\n", number);
        }
        else {
            printf("O número %d lido pelo consumidor através do pipe não é primo\n", number);
        }
    }

    close(pipefd[PIPE_READ]);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <número de elementos>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_elements = atoi(argv[1]);

    srand(time(NULL));
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        consumer(pipefd);
    } else {
        producer(pipefd, num_elements);
        wait(NULL);
    }

    return 0;
}
