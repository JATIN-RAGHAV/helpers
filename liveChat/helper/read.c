#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>

char* readFile(char* name){

    FILE* file = fopen(name, "r");
    if(file == NULL){
        printf("%s didn't open.\n",name);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);

    char* buffer = (char*) malloc(length+1);

    if(buffer == NULL){
        printf("Couln't read %s, not enough space in heap.\n",name);
        return NULL;
    }
    fread(buffer, 1, length-1, file);
    buffer[length-1] = 0;
    fclose(file);
    return buffer;
}
