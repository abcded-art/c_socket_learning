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
#define NAME_SIZE 32
#define MAX_LOG_LINES 1000

/**
 * Define functions and global variables
 */

void *handle_clnt(void *arg);
void send_msg(char *msg, int len, int clnt_sock);
void brd_msg(char *msg, int len, int clnt_sock);
void error_handling(char *msg);
void *admin_thread(void *arg);

int show_menu();
void show_connecting_hosts();
void show_recent_messages();
void print_logs(char logs[][128], int start, int end);
void announce_clients();
void setTimerSchedule();
void selectTimerOptions();
void showTimerSchedules();
void deleteTimerSchedule();

int getIntValue();

void clear_input_buffer();
void save_log(const char *message);

/**
 * A data area.
 * Here will be shared with other threads.
 * There are multi clients connect to the server, client socket should be a list.
 */

// To show how many clients connecting to the server
int clnt_cnt = 0;
// To list clients and generate muti-thread.
int clnt_socks[MAX_CLNT];
// Clients' Nickname
char clnt_names[MAX_CLNT][NAME_SIZE];
// Host name
char host_name[80];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
    // Set Variables
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id, admin_t_id;

    int option;
    socklen_t optlen;

    // Get Host Name
    if(gethostname(host_name, sizeof(host_name)) != 0){
        printf("gethostname() Failed");
    }

    // For log file store
    // FILE &fp;

    if(argc != 2){ // <File Name> <Port #>
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Generate Mutex
    pthread_mutex_init(&mutx, NULL);

    // Generate server socket
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
    // Socket binding
    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("bind() error");
    }
    
    // Socket listening
    if (listen(serv_sock, 5) == -1 ){
        error_handling("listening() error");
    }

    // Admin thread
    // This thread can communicate with the server operator to config and monitor this service.
    pthread_create(&admin_t_id, NULL, admin_thread, NULL);
    pthread_detach(admin_t_id);

    // All socket creating is done, so we'll log at here.
    char startLog[2048];
    snprintf(startLog, 2048, "Server Starts Running by host [ %s ]\n", host_name);
    save_log(startLog);

    while(1){
        // Create client socket
        // This socket is for the client listening and communicating
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        // If some client connected with server, it locks the thread until the code logic is done.
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        // Generate client processing thread
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);

        // printf("\nNew host connected, IP: %s \n", inet_ntoa(clnt_adr.sin_addr), clnt_names[clnt_cnt - 1]);
        // printf("Connecting Hosts Numbers: %d\n", clnt_cnt);
    }

    close(serv_sock);
    pthread_mutex_destroy(&mutx);
    return 0;
}

// This function is for the server operator
void *admin_thread(void *arg){
    char input;
    while(1){
        // This option is for the server operator communication.
        int userInput = show_menu();
        switch(userInput){
            // Show connecting hosts.
            case 1:
                show_connecting_hosts();
                break;
            case 2:
                show_recent_messages();
                break;
            case 3:
                announce_clients();
                break;
            case 4:
                selectTimerOptions();
                break;
            default:
                printf("Server shutting down...\n");
                
                char endLog[2048];
                snprintf(endLog, 2048, "Server Shutted Down by host [ %s ]\n", host_name);
                save_log(endLog);
                exit(0);
        }
    }
}

// This function is for the handling client
void *handle_clnt(void *arg) {
    int clnt_sock = *((int *)arg);
    char nickname[NAME_SIZE];
    char msg[BUF_SIZE];
    int str_len = 0;

    // Exctract names from the message
    // The client code should send it's nickname first, and then it will communicate with the message codes.
    if ((str_len = read(clnt_sock, nickname, sizeof(nickname) - 1)) > 0) {
        nickname[str_len] = '\0';
        pthread_mutex_lock(&mutx);
        strcpy(clnt_names[clnt_cnt - 1], nickname);
        pthread_mutex_unlock(&mutx);
    }

    // Print New host
    // Save logs
    printf("\n****** New client connected: %s ******\n", clnt_names[clnt_cnt - 1]);
    char new_connect_log_message[2048];
    snprintf(new_connect_log_message, sizeof(new_connect_log_message), "New client [ %s ] connected\n", clnt_names[clnt_cnt - 1]);
    save_log(new_connect_log_message);

    // Listening client calls
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
        msg[str_len] = '\0';
        // 클라이언트의 채팅이 호스트에도 보인다.
        printf("%s\n", msg);
        // 채팅을 로그 기록으로 남긴다.
        save_log(msg);
        // Send every clients messages. clnt_sock specifies the client who sent a message.
        send_msg(msg, str_len, clnt_sock);
    }

    // End of the while loop
    // Socket connection ended

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++){
        if (clnt_sock == clnt_socks[i]){
            printf("\n****** Client [ %s ] leaved ******\n", clnt_names[i]);
            char connection_end_message[2048];
            snprintf(connection_end_message, sizeof(connection_end_message), "Client [ %s ] leaved\n", clnt_names[i]);
            save_log(connection_end_message);

            for(int j = i; j < clnt_cnt - 1; j++){
                clnt_socks[j] = clnt_socks[j + 1];
                strcpy(clnt_names[j], clnt_names[j + 1]);
            }
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);

    close(clnt_sock);
    return NULL;
}

// This function do sending messages to all clients.
void send_msg(char *msg, int len, int clnt_sock){
    // Each client uses its own thread.
    // So we don't need to use mutex.
    for(int i = 0; i < clnt_cnt; i++){
        // Send to every socket clients
        // Except the client who send the message
        if(clnt_socks[i] != clnt_sock){
            write(clnt_socks[i], msg, len);
        }
    }
}

// This function do sending messages to all clients.
void brd_msg(char *msg, int len, int clnt_sock){
    for(int i = 0; i < clnt_cnt; i++){
        write(clnt_socks[i], msg, len);
    }
}

void error_handling(char *msg){ 
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

/**
 * This function gets an integer value from a user.
 */

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

/**
 * Here is for the empty buffer
 */

void clear_input_buffer(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Here is for the Logs savings
 */

void save_log(const char *message){
    FILE *log_file = fopen("chat_log.txt", "a");
    if (log_file != NULL){
        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

        fprintf(log_file, "[%s] %s", time_str, message);
        fclose(log_file);
    }
}

/**
 * Here is for the Options
 */

int show_menu(){
    int userInput = 0;
    while(1){
        printf("========== [ Server Options ] ==========\n");
        printf("[1] Connecting Hosts\n");
        printf("[2] Show Logs\n");
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

/**
 * [1] Connecting Hosts
 * 
 * This functions shows how many clients connecting to host,
 * and also shows each clients' information.
 */

void show_connecting_hosts() {
    printf("Connecting hosts: %d \n", clnt_cnt);

    // end of the temp
    if(clnt_cnt > 0){
        printf("*** [ Host lists ] *** \n");
        for(int i = 0; i < clnt_cnt; i++){
            struct sockaddr_in clnt_adr;
            socklen_t clnt_adr_sz = sizeof(clnt_adr);
            getpeername(clnt_socks[i], (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
            printf("IP: %s | PORT #: %d | Name: %s \n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port), clnt_names[i]);
        }
    }
    printf("\n");
}

/**
 * [2] Recent Messages
 */

void show_recent_messages() {
    char logs[MAX_LOG_LINES][256];
    int logCount = 0;
    FILE *fp = fopen("chat_log.txt", "r");
    if (fp == NULL){
        printf("Failed to open log file.\n");
        return;
    }

    while(fgets(logs[logCount], sizeof(logs[logCount]), fp)){
        logCount++;
        if(logCount >= MAX_LOG_LINES) break;
    }
    fclose(fp);

    int userInput = 0;
    int start, end;
    
    while(1){
        printf("====== [ [2] Notification Options ] ======\n");
        printf("[1] Show recent 50 messages\n");
        printf("[2] Show all messages\n");
        printf("[3] Exit showing messages\n");
        userInput = getIntValue();
        switch(userInput){
        case 1:
            start = (logCount > 50) ? logcount - 50 : 0;
            end = logCount;
            while(start > 0){
                print_logs(logs, start, end);
                
                logCount -= 50;
                if(logLength <= 0){
                    printf("\nAll log printed\n");
                    break;
                }
                while(logLength > 2 || logLength < 0){
                    printf("Invalid number, try again: ");
                    userInput = getIntValue();
                }
                userInput = getIntValue();
                if(userInput == 1){
                    continue;
                } else if(userInput == 2){
                    break;
                }

            }
            
            break;
        case 2:
            // Logics for show all log datas from chat_log.txt
            break;
        default:
            printf("Exit [2] Notification Options\n");
            return;
        }
    }
}

void print_logs(char logs[][128], int start, int end){
    for(int i = start; i < end; i++){
        printf("%s", logs[i]);
    }
}

/**
 * [3] Announce All Clients
 */

void announce_clients() {
    printf("Type what you want to announce: ");
    char msg[BUF_SIZE];
    clear_input_buffer();
    if(fgets(msg, BUF_SIZE, stdin) != NULL){
        size_t len = strlen(msg);
        if(len > 0 && msg[len - 1] == '\n'){
            msg[len - 1] = '\0';
        }
        char full_msg[BUF_SIZE + 20];
        snprintf(full_msg, sizeof(full_msg), "[manager]: %s\n", msg);
        printf("Announcement: %s \n", msg);
        save_log(full_msg);
        for (int i = 0; i < clnt_cnt; i++){
            brd_msg(full_msg, strlen(full_msg), clnt_socks[i]);
        }
    } else{
        printf("Failded to read input.\n");
    }
}

/**
 * [4] Schedule Notifications
 */

void selectTimerOptions(){
    int userInput = 0;
    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
    while(1){
        printf("====== [ [4] Notification Options ] ======\n");
        printf("Today: %d/%d, %d  |  %d:%d:%d\n", t->tm_mon + 1, t->tm_mday, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
        printf("[1] Set Notification Schedule\n");
        printf("[2] Check Notification Schedule\n");
        printf("[3] Delete Notification Schedule\n");
        printf("[4] Exit Notification Options\n");
        userInput = getIntValue();
        switch(userInput){
            case 1:
                setTimerSchedule();
                break;
            case 2:
                showTimerSchedules();
                break;
            case 3:
                deleteTimerSchedule();
                break;
            default:
                printf("Exit Timer Options\n");
                return;
        }
    }

}

void setTimerSchedule() {
    int m, s;
    printf("We can schedule after dedicated minutes and seconds.\n");
    printf("Type minutes: ");
    scanf("%d", &m);
    printf("Type seconds: ");
    scanf("%d", &s);
    if((m <= 0 && s <= 0) || m < 0 || s < 0){
        printf("Time not scheduled\n");
        return;
    }
    
}

void showTimerSchedules() {
    printf("showTimerSchedules()\n");
}

void deleteTimerSchedule() {
    printf("deleteTimerSchedule()\n");
}