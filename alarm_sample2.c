#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>

bool alarm_set = false;
int remaining_time = 0;

// 알람 시간이 되면 실행하는 함수이다.
void alarm_handler(int signum){ 
    printf("\n**********Alarm triggered!**********\n");
    fflush(stdout);
    printf("Enter: ");
    fflush(stdout);
    alarm_set = false;
}

// 알람을 몇 초 뒤에 수행할 것인지 설정하는 함수이다.
void set_alarm(int seconds){
    // 알람 설정을 한다.
    // Alarm이 트리거되면 SIGALRM 신호를 받을 때까지 대기한다.
    // SIGALRM은 signal of alarm의 줄임말로써, C 표준 라이브러리에서 정의된 정수형 상수이다.
    // SIGALRM을 받으면, alarm_handler 함수를 호출한다.
    signal(SIGALRM, alarm_handler); 

    // 알람과 타이머를 동시에 설정한다.
    alarm(seconds);

    // 옵션들의 설정을 위해 남은 시간을 저장한다.
    remaining_time = seconds;

    // 알람이 설정됐음을 표시하기 위한 변수이다.
    alarm_set = true;
    printf("\nAlarm set for %d seconds.\n\n", seconds);
}

// 남은 알람 시간을 알려준다.
// 이 함수는 오차가 발생한다.
void check_remaining_time(){
    if(!alarm_set){
        printf("No alarm is currently set.\n");
        return;
    }
    int remain = alarm(0); // 남은 시간을 가져온다. 그리고 알람을 종료한다.
    printf("Approximately %d seconds are remaining.\n", remain); // printf 함수에 의해 ms의 오차가 발생할 수 있다.
    alarm(remain); // 알람을 다시 설정한다.
}

// alarm으로만 구현하는 알람을 종료하는 함수이다.
void cancel_alarm(){
    if (alarm_set){
        int remain = alarm(0); // 남은 시간을 저장하고, 알람을 종료한다.
        alarm_set = false; // 알람이 취소됐음을 동네방네 알린다.
        printf("Alarm has been cancelled.\n");
    } else{
        printf("No alarm is currently set.\n");
    }
}

// Int형 value만 받기 위해 설정한 함수이다.
// 자꾸 이상한 값을 넣으면서 시험할까봐 넣은 함수이다.
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
                    printf("An alarm is set and will go off in approximately %d seconds. \n", remaining_time);
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