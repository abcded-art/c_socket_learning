#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define MAX_LINES 100

void print_logs(char logs[][BUF_SIZE], int start, int end) {
    for (int i = start; i < end; i++){
        printf("%s", logs[i]);
    }
}

int main(){
    char logs[MAX_LINES][BUF_SIZE];
    int line_count = 0;

    FILE *file = fopen("sample.txt", "r");
    if (file == NULL){
        perror("Error open file");
        exit(EXIT_FAILURE);
    }

    while (fgets(logs[line_count], BUF_SIZE, file) != NULL && line_count < MAX_LINES){
        line_count++;
    }

    fclose(file);

    print_logs(logs, 0, line_count);

    return 0;
}