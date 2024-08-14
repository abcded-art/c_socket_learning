#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>

bool alarm_set = false;

void* alarm_thread(void* arg) {
    int seconds = *((int*)arg);
    sleep(seconds);
    printf("\n**********Alarm triggered after %d seconds!**********\nEnter the number of seconds: ", seconds);
    fflush(stdout);
    alarm_set = false;
    return NULL;
}

void set_alarm(int seconds) {
    pthread_t tid;
    pthread_create(&tid, NULL, alarm_thread, &seconds);
    pthread_detach(tid);
    alarm_set = true;
    printf("\nAlarm set for %d seconds.\n\n", seconds);
}

int main() {
    int seconds;
    while (1) {
        printf("Enter the number of seconds: ");
        scanf("%d", &seconds);
        if (seconds <= 0) break;
        set_alarm(seconds);
    }
    return 0;
}
