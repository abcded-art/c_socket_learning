#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int active_threads = 0;  // 현재 가동 중인 스레드의 수를 추적하는 변수
pthread_mutex_t lock;

void* alarm_thread(void* arg) {
    int seconds = *((int*)arg);
    free(arg);  // 동적으로 할당된 메모리 해제

    // 타이머 시작
    sleep(seconds);
    
    // 타이머가 끝나면 알람 트리거
    pthread_mutex_lock(&lock);
    printf("\n********** Alarm triggered after %d seconds! **********\n", seconds);
    fflush(stdout);
    active_threads--;  // 스레드 종료 시 active_threads 감소
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

void set_alarm(int seconds) {
    pthread_t tid;
    int* arg = malloc(sizeof(int));  // 동적으로 인자를 할당
    *arg = seconds;

    pthread_mutex_lock(&lock);
    active_threads++;  // 새로운 스레드 생성 시 active_threads 증가
    pthread_mutex_unlock(&lock);

    // 새로운 스레드를 생성하여 타이머를 시작
    pthread_create(&tid, NULL, alarm_thread, arg); // 쓰레드를 생성한다.
    pthread_detach(tid); // 실행 중인 쓰레드를 분리 상태로 만든다.

    printf("Alarm set for %d seconds.\n", seconds);
    printf("Currently active threads: %d\n", active_threads);
    fflush(stdout);
}

int main() {
    pthread_mutex_init(&lock, NULL); // 
    int seconds;

    while (1) {
        printf("Enter the number of seconds (0 or negative to exit): ");
        fflush(stdout);
        
        if (scanf("%d", &seconds) != 1 || seconds <= 0) {
            break;
        }

        set_alarm(seconds);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
