#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(char *msg);

char name[NAME_SIZE] = "DEFAULT";
char msg[BUF_SIZE];

int main(int argc, char *argv[]){
    int sock;
    struct sockaddr_in serv_addr;

    // Two threads: For Send Thread and Receive Thread
    // Should send my thread, and receive opponent's thread
    pthread_t snd_thread, rcv_thread;
    // It uses at pthread_join
    void *thread_return;
    // IP PORT USER_NAME
    if (argc != 4){
        printf("Usage: %s <IP> <port> <name> \n", argv[0]);
        exit(1);
    }

    sprintf(name, "%s", argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    write(sock, argv[3], strlen(argv[3]));
    
    // Generate two threads. Each main are send_msg, recv_msg
    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

    // Close threads
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    // Close client connection
    close(sock);
    return 0;
}

// main thread of the 'snd_thread'
void *send_msg(void *arg){
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    printf("You[%s]: ", name);
    while(1){
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")){
            close(sock);
            exit(0);
        }
        // Expand 20 for additional annotations
        snprintf(name_msg, sizeof(name_msg) + 4, "[%s]: %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}


// main thread of the 'rcv_thread'
void *recv_msg(void *arg){
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE * BUF_SIZE];
    int str_len;
    while(1){
        str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        if(str_len == -1){
            return (void *) -1;
        }
        name_msg[str_len] = '\0';

        // 커서를 한 줄 위로 이동하고, 그 줄을 지움
        printf("\033[A\033[K");  // 현재 라인을 지우고 커서를 위로 이동
        
        fputs(name_msg, stdout);  // 수신한 메시지 출력
        printf("You[%s]: ", name);  // 다시 프롬프트 출력
        fflush(stdout);  // 강제로 출력 버퍼를 비움

        // str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        // if(str_len == -1){
        //     return (void *) -1;
        // }
        // name_msg[str_len] = 0;
        // printf("\n\n");
        // // fflush(stdout);
        // fputs(name_msg, stdout);
        // printf("\nYou[%s]: ", name);
        // fflush(stdout);
    }
    return NULL;
}

void error_handling(char *msg){
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

/*

xml update?
at command?
acme?
air command?
nec

seman - hybrid.
sejong - hybrid

*/