#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

void timer_handler(int signum){
    static int count = 0;
    count++;
    printf("Alarm triggered! Count: %d\n", count);
}

int main(int argc, char *argvp[]){
    struct itimerval timer;
    signal(SIGALRM, timer_handler);

    int seconds;
    printf("How long do you wanna wait: ");
    scanf("%d", &seconds);

    // Set timers
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    for (int i = 0; i < seconds; i++){
        sleep(1);
        int remain_sec = timer.it_value.tv_sec - i - 1;
        printf("You set %d seconds, Time passed %d seconds, Remaining %d seconds.\n", seconds, i + 1, remain_sec);
    }
    return 0;
}