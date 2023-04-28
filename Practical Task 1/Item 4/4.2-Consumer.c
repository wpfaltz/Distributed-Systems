#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

bool is_prime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Criando o arquivo descritor do socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Atrelando socket à porta 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Esperando conexão do Produtor...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Conexão estabelecida!\n");

    char buffer[20] = {0};
    int n;

    while (true) {
        valread = read(new_socket, buffer, 20);
        if (valread == 0) {
            printf("Conexão encerrada pelo Produtor.\n");
            break;
        }
        n = atoi(buffer);
        // printf("Consumidor recebeu o número %d.\n", n);
        if (n == 0) {
            printf("Consumidor recebeu o sinal de término. Encerrando conexão...\n");
            break;
        }
        bool is_n_prime = is_prime(n);
        char response[20] = {0};
        sprintf(response, "%d", is_n_prime);
        send(new_socket, response, 20, 0);
        // printf("Consumidor enviou a resposta %d ao Produtor.\n", is_n_prime);
    }

    close(new_socket);
    close(server_fd);

    return 0;
}