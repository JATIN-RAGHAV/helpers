#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include <stdlib.h>

void redirectUser(char *url){
    char cmd[] = "open ";
    char* command = (char*)malloc(sizeof(cmd) + sizeof(url)+2);
    if(command == NULL){
        printf("Couln't create the redirect command.Not enough space in heap\n");
        exit(1);
    }
    strcpy(command, cmd);
    strcpy(&command[sizeof(cmd)-1], url);
    system(command);
}

char* itoa(int x){
    char* num = (char*)malloc(10);
    num[9] = 0;
    int index = 8;
    while(x>0){
        num[index--] = x%10 + '0';
        x = (int)(x/10);
    }
    return &num[index+1];
}

char save_tokens(char** tokens){
    FILE* file = fopen("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.access_token.txt","w");
    if(file == NULL){
        printf("Couldn't read /Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.access_token.txt\n");
        return 0;
    }
    fprintf(file, "%s",tokens[0]);
    fclose(file);

    file = fopen("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.refresh_token.txt","w");
    if(file == NULL){
        printf("Couldn't read /Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.refresh_token.txt\n");
        return 0;
    }
    fprintf(file, "%s",tokens[1]);
    fclose(file);

    return 1;
}

char* get_refresh_token(){
    FILE* file = fopen("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.refresh_token.txt","r");
    if(file == NULL){
        printf("Coldn't open file /Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.refresh_token.txt\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(len+1);
    if(buffer == NULL){
        printf("Coulnt get enough space to store refresh token\n");
        return NULL;
    }

    fread(buffer,1,len,file);
    buffer[len] = 0;
    fclose(file);
    return buffer;
}

char* get_access_token(){
    FILE* file = fopen("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.access_token.txt","r");
    if(file == NULL){
        printf("Coldn't open file /Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.access_token.txt\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(len+1);
    if(buffer == NULL){
        printf("Coulnt get enough space to store access token\n");
        return NULL;
    }

    fread(buffer,1,len,file);
    buffer[len] = 0;
    fclose(file);
    return buffer;
}
