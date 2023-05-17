#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define M 100000 // número de elementos a serem processados pelo consumidor

int N, np, nc, filled_positions; // tamanho do buffer
int *buffer;
int in = 0, out = 0, item_produzido = 1, item_consumido = 1;
sem_t empty, full; // empty representa a quantidade de posições vazias e o full a quantidade de posições ocupadas
pthread_mutex_t mutex; // controla o acesso ao buffer
clock_t start;
int *filled_positions_history;
int history_index = 0;

void push(int value) {
    filled_positions_history[history_index] = value;
    history_index = (history_index + 1) % ((2*M)+N);
}

void finish_execution() {
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tempo de execução do programa para tamanho do buffer N = %d, Np = %d e Nc = %d: %.8lf segundos\n", N, np, nc, cpu_time_used);

    char filename[100];  // Defina um tamanho suficiente para a string do nome do arquivo
    sprintf(filename, "Histories/buffer_use_history_N_%d_Np_%d_Nc_%d.txt", N, np, nc);
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1); // Encerrar o programa em caso de erro ao abrir o arquivo
    }

    for (int i = 0; i < ((2*M)+N); i++) {
        if (i < ((2*M)+N)-1) {
            fprintf(file, "%d,", filled_positions_history[i]);
        } else {
            fprintf(file, "%d", filled_positions_history[i]);
        }
        
    }
    fclose(file);
            
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    free(buffer);
    free(filled_positions_history);
    exit(0);
}

int is_prime(int num) {
    if (num < 2) {
        return 0;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

void *producer(void *arg) {
    int *tid = (int *) arg;
    while (1) {
        int item = rand() % 10000000 + 1;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (item_consumido-1 == M) {
            finish_execution();
        }
        else {
            buffer[in] = item;
            //printf("Thread %d (Produtor %d) produziu item %d: número %d na posição %d\n", *tid, *tid, item_produzido, item, in);
            in = (in + 1) % N;
            filled_positions++;
            push(filled_positions);
            item_produzido++;
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
        }
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int *tid = (int *) arg;
    int item, isPrime;
    while (1) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        if (item_consumido-1 == M) {
            finish_execution();
        }
        else {
            item = buffer[out];
            printf("Thread %d (Consumidor %d) consumiu item %d de %d: consumiu número %d na posição %d\n", *tid, *tid - np, item_consumido, M, item, out);
            out = (out + 1) % N;
            filled_positions--;
            push(filled_positions);
            item_consumido++;
            pthread_mutex_unlock(&mutex);     
            sem_post(&empty);
            isPrime = is_prime(item);
            if (isPrime) {
                printf("Thread %d (Consumidor %d): número %d é primo\n",*tid, *tid - np, item);
            } else {
                printf("Thread %d (Consumidor %d): número %d não é primo\n",*tid, *tid -np, item);
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf ("Informe, respectivamente, o tamanho do buffer, o número de threads de produtores e o número de threads de consumidores como argumentos");
        return -1;
    }

    srand(time(NULL));
    N = atoi(argv[1]);
    np = atoi(argv[2]);
    nc = atoi(argv[3]);
    buffer = malloc(N * sizeof(int));
    filled_positions = 0;
    filled_positions_history = malloc(((2*M) + N) * sizeof(int));
    pthread_t tid[nc+np];
    int id[nc+np];

    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < np; i++) {
        id[i] = i+1;
        pthread_create(&tid[i], NULL, producer, (void *) &id[i]);
    }

    for (int i = np; i < nc+np; i++) {
        id[i] = i+1;
        pthread_create(&tid[i], NULL, consumer, (void *) &id[i]);
    }

    start = clock();

    for (int i = 0; i < nc+np; i++) {
        pthread_join(tid[i], NULL);
    }

    free(buffer);
    free(filled_positions_history);

    return 0;
}
