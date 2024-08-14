#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int counter = 1;

void sigint_handler(){
    printf("Alarm!\n");
}

int main(int argc, char *argv[]){
    printf("Alarm Setting\n");
    signal(SIGALRM, &sigint_handler);
    int iterator = 0;
    while(1){
        printf("[Alarm iterate %d]\n", iterator);
        printf("How long do you want to wait: ");
        scanf("%d", &counter);
        if(counter <= 0){
            return 0;
        }
        alarm(counter);
        // 몇 초가 남았는지 보여준다.
        // 단, 이 코드는 ms단위까지의 정확성을 보여주지는 못한다.
        // 그 이유는 alarm(0)이 타이머를 멈추고, 그 정보를 저장하는 코드이기 때문이다.
        // 이를 해결하기 위해 settimer()를 활용할 수 있다. 
        printf("Alarm was set for %d seconds.\n", counter);
        for(int i = 0; i < counter; i++){
            int remain = alarm(0);
            printf("Time pass %d seconds, and remain %d seconds.\n", counter - remain, remain);
            alarm(remain);
        }
        // pause();
        iterator++;
    }
}