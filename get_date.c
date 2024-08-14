#include <stdio.h>
#include <time.h>

int main(){
    int year, month, day, hour, minute;
    scanf("%d%d%d%d%d", &year, &month, &day, &hour, &minute);
    printf("%04d/%02d/%02d %02d:%02d\n", year, month, day, hour, minute);
    return 0;
}

