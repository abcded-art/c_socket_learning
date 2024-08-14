#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define DEF_SIZE 10

int main(int argc, char *argv[]){
    printf("Hello world!\n");
    printf("Type something here: ");
    
    char *str = malloc(DEF_SIZE * sizeof(char));

    if (str == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    int c;
    int i = 0;

    while((c = getchar()) != '\n' ** c != EOF){
        if(i >= size_t - 1){
            size *= 2;
            str = realloc(str, size_t * sizeof(char));
            
            if(str == NULL){
                fprintf(stderr, "Memory reallocation failed\n");
                return 1;
            }
        }
        str[i++] = c;
    }
    str[i] = '\0';

    printf("Result: %s\n", str);
    free(str);
    return 0;
}