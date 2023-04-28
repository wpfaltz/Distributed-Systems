#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define MAX_MSG_LEN 20
#define PORT 8080

int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i <= num/2; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char const *argv[]) {
    srand(time(NULL)); // semente para números aleatórios

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <number of values>\n", argv[0]);
        exit(1);
    }

    int num_values = atoi(argv[1]);

    int sock = 0;
    struct sockaddr_in serv_addr;
    char msg[MAX_MSG_LEN] = {0};
    int current_number, delta;

    // criação do socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Erro na criação do socket\n");
        return -1;
    }

    // configuração do endereço do servidor
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Endereço inválido\n");
        return -1;
    }

    // conexão com o servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Erro na conexão\n");
        return -1;
    }

    // envio de números aleatórios crescentes ao servidor
    current_number = 1;
    for (int i = 0; i < num_values; i++) {
        delta = rand() % 100 + 1;

        // conversão do número para uma string de tamanho fixo
        snprintf(msg, MAX_MSG_LEN, "%d", current_number);

        // envio do número para o servidor
        send(sock, msg, MAX_MSG_LEN, 0);

        // recebimento da resposta do servidor
        memset(msg, 0, MAX_MSG_LEN);
        if (recv(sock, msg, MAX_MSG_LEN, 0) <= 0) {
            printf("Erro na leitura do socket\n");
            break;
        }

        // impressão do resultado
        if (strcmp(msg, "1") == 0) {
            printf("%d é primo\n", current_number);
        } else {
            printf("%d não é primo\n", current_number);
        }

        current_number += delta;
    }

    // envio de mensagem de término para o servidor
    snprintf(msg, MAX_MSG_LEN, "0");
    send(sock, msg, MAX_MSG_LEN, 0);

    // fechamento do socket
    close(sock);

    return 0;
}
