#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 256
#define NAME_SIZE 32
#define BUF_SIZE 1024

int clnt_socks[MAX_CLIENTS];
char clnt_names[MAX_CLIENTS][NAME_SIZE];
int client_count = 0;
pthread_mutex_t mutex;

void *handle_client(void *arg);
void show_connecting_hosts(void);

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size;
    pthread_t thread_id;

    server_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket() error");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind() error");
        exit(1);
    }

    if (listen(server_sock, 5) == -1) {
        perror("listen() error");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);

    while (1) {
        client_addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);

        if (client_sock == -1) {
            perror("accept() error");
            continue;
        }

        pthread_mutex_lock(&mutex);
        clnt_socks[client_count] = client_sock;
        client_count++;
        pthread_mutex_unlock(&mutex);

        pthread_create(&thread_id, NULL, handle_client, (void *)&client_sock);
        pthread_detach(thread_id);
    }

    close(server_sock);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *handle_client(void *arg) {
    int client_sock = *((int *)arg);
    char message[BUF_SIZE];
    int str_len;

    // 클라이언트로부터 닉네임을 먼저 받음
    if ((str_len = read(client_sock, message, NAME_SIZE - 1)) != 0) {
        message[str_len] = '\0';
        pthread_mutex_lock(&mutex);
        strcpy(clnt_names[client_count - 1], message);  // 해당 클라이언트의 닉네임 저장
        pthread_mutex_unlock(&mutex);
    }

    show_connecting_hosts();

    while ((str_len = read(client_sock, message, sizeof(message) - 1)) != 0) {
        message[str_len] = '\0';
        printf("Received from %s: %s\n", clnt_names[client_count - 1], message);
        write(client_sock, message, str_len);  // 에코
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) {
        if (clnt_socks[i] == client_sock) {
            for (int j = i; j < client_count - 1; j++) {
                clnt_socks[j] = clnt_socks[j + 1];
                strcpy(clnt_names[j], clnt_names[j + 1]);
            }
            break;
        }
    }
    client_count--;
    pthread_mutex_unlock(&mutex);

    close(client_sock);
    return NULL;
}

void show_connecting_hosts() {
    printf("Connecting hosts: %d\n", client_count);

    for (int i = 0; i < client_count; i++) {
        printf("Client socket: %d\n", clnt_socks[i]);
        printf("Client nickname: %s\n", clnt_names[i]);
    }

    if (client_count > 0) {
        printf("*** [ Host lists ] ***\n");
        for (int i = 0; i < client_count; i++) {
            struct sockaddr_in clnt_adr;
            socklen_t clnt_adr_sz = sizeof(clnt_adr);
            getpeername(clnt_socks[i], (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
            printf("IP: %s | PORT #: %d | Name: %s\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port), clnt_names[i]);
        }
    }
    printf("\n");
}
