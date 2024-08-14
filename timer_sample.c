#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>

bool alarm_set = false;

// 알람 시간이 되면 실행하는 함수이다.
void alarm_handler(int signum){ 
    printf("\n\n\n**********Alarm triggered!**********\n\n\n");
    alarm_set = false;
}

// 타이머를 설정하고 알람을 동시에 설정하는 함수이다.
void set_alarm(int seconds){
    /*
    timerval it_interval : 타이머가 처음 발동될 때까지의 시간을 지정한다.
      - tv_sec : 타이머가 발동될 때까지의 시간(초 단위)
      - tv_usec : 타이머가 발동될 때까지의 시간(마이크로초 단위)
    timerval it_interval : 타이머가 반복적으로 발동될 간격을 지정한다.
      - tv_sec : 타이머가 반복적으로 반복될 간격(초 단위)
      - tv_usec : 타이머가 반복적으로 발동될 간격(마이크로초 단위)
    */
    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    // SIGALRM 시그널을 받을 때 호출될 핸들러를 설정합니다.
    signal(SIGALRM, alarm_handler); 

    // 타이머를 설정하여 알람을 설정합니다.
    if(setitimer(ITIMER_REAL, &timer, NULL) == -1){
        perror("Failed to set timer");
        return;
    }

    alarm_set = true;
    printf("\nAlarm set for %d seconds.\n\n", seconds);
}

// 남은 알람 시간을 알려준다.
void check_remaining_time(){
    if(!alarm_set){
        printf("No alarm is currently set.\n");
        return;
    }

    struct itimerval timer;
    if (getitimer(ITIMER_REAL, &timer) == -1){
        perror("Failed to get timer");
        return;
    }

    int remain_sec = timer.it_value.tv_sec;
    int remain_usec = timer.it_value.tv_usec;

    double remain = remain_sec + remain_usec / 1000000.0;
    
    printf("Approximately %.1f seconds are remaining.\n", remain);
}

// 알람과 타이머를 동시에 종료하는 함수이다.
void cancel_alarm(){
    if(alarm_set){
        struct itimerval timer = {0};

        if (setitimer(ITIMER_REAL, &timer, NULL) == -1){
            perror("Failed to cancel timer");
        }

        alarm_set = false;
        printf("Alarm and timer have been cancelled.\n");
    } else{
        printf("No alarm is currently set.\n");
    }
}

// Int형 value만 받기 위해 설정한 함수이다.
int getIntValue(){
    int res = 0;
    while(1){
        if(scanf("%d",&res) != 1){
            printf("Invalid input. Please enter an integer value.\n");
            while(getchar() != '\n');// 입력 버퍼 지우기
            continue;
        }
        return res;
    }
    return res;
}

// 옵션을 보여주는 함수이다.
int showOptions(){
    int userInput = 0;
    while(1) {
        printf("========== [ Options ] ==========\n");
        printf("[1] Set Alarm\n");
        printf("[2] Check alarm\n");
        printf("[3] Check Remaining\n");
        printf("[4] Close Alarm\n");
        printf("[5] Exit\n");
        printf("Select an option: ");
        
        userInput = getIntValue();

        if(userInput <= 0){
            return -1;
        }
        return userInput;
    }
}

int main(){
    int seconds;

    while(1){
        int userInput = showOptions();
        switch(userInput){
            case 1:
                if (alarm_set){
                    printf("An alarm is already set.\n");
                } else {
                    printf("Enter the number of seconds: ");
                    seconds = getIntValue();
                    set_alarm(seconds);
                }
                break;
            case 2:
                if(alarm_set){
                    check_remaining_time();
                } else{
                    printf("No alarm is currently set.\n");
                }
                break;
            case 3:
                check_remaining_time();
                break;
            case 4:
                cancel_alarm();
                break;
            default:
                printf("Bye Bye~\n");
                return 0;
        }
    }

    return 0;
}
