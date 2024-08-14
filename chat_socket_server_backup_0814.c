/**
 * Define headers
 * This code refers to this web-site "https://velog.io/@dev-hoon/%EB%A9%80%ED%8B%B0-%EC%93%B0%EB%A0%88%EB%93%9C-%EA%B8%B0%EB%B0%98-%EB%8B%A4%EC%A4%91-%EC%B1%84%ED%8C%85"
 */

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>


/**
 * Define static sizes
 * We'll service to 256 clients
 */

#define BUF_SIZE 100
#define MAX_CLNT 256

/**
 * Define functions and global variables
 */

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);
void show_menu();
void show_connecting_hosts();
void show_recent_messages();
int getIntValue();
int shopOptions();

/**
 * A data area.
 * Here will be shared with other threads.
 * There are multi clients connect to the server, client socket should be a list.
 */

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    int option;
    socklen_t optlen;

    // For log file store
    // FILE &fp;

    if(argc != 2){ // <File Name> <Port #>
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Generate Mutex
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    // Eliminate Time-wait
    // SO_REUSEADDR 0 -> 1
    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);

    // Allocate IPv4, IP, Port
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    // Allocate an address
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    

    if (listen(serv_sock, 5) == -1 ){
        error_handling("listening() error");
    }

    while(1){
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
        printf("Connecting Hosts Numbers: %d\n", clnt_cnt);
    }

    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg) {
    // void -> int
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    // Client가 Server와 접속하고 있는 동안, 해당 while문을 벗어나지 않음
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    // while문이 끊어졌다 = socket connection ended
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++){
        if (clnt_sock == clnt_socks[i]){
            while(i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}

void send_msg(char *msg, int len){
    // lock mutex: clnt_cnt, clnt_socks[]
    pthread_mutex_lock(&mutx);
    for(int i = 0; i < clnt_cnt; i++){
        // send to every socket clients
        write(clnt_socks[i], msg, len);
    }

    // unlock mutex
    pthread_mutex_unlock(&mutx);
}

void error_handling(char *msg){
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

int getIntValue(){
        int res = 0;
    while(1){
        if(scanf("%d",&res) != 1){
            printf("Invalid input. Please enter an integer value.\n");
            while(getchar() != '\n');// Clear Input Buffer
            continue;
        }
        return res;
    }
    return res;
}

void show_menu(){
    int userInput = 0;
    while(1){
        printf("========== Server Options ==========\n");
        printf("[1] Connecting Hosts\n");
        printf("[2] Recent Messages\n");
        printf("[3] Announce All Clients\n");
        printf("[4] Schedule Notifications\n");
        printf("[5] Quit Server\n");
        printf("Select an option:");

        userInput = getIntValue();

        if(userInput <= 0){
            return -1;
        }
        return userInput;
    }
}

void show_connecting_hosts(){
    pthread_mutex_lock(&mutx);
    printf("Connecting hosts: %d \n", clnt_cnt);
    printf("Host lists: \n");
    for(int i =0; i < clnt_cnt; i++){
        struct sockaddr_in clnt_adr;
        socklen_t clnt_adr_sz = sizeof(clnt_adr);
        getpeername(clnt_socks[i], (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        
    }
}

