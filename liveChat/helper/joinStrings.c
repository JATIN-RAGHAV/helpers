#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* join_two (char *x, char*y);

char *join(char*strs[],int len){
    if(len == 0)
        return 0;
    if(len == 1)
        return strs[0];

    char* joined = join_two(strs[0],strs[1]);
    for(int i = 2;i<len; i++){
        char* new_joined = join_two(joined, strs[i]);
        free(joined);
        joined = new_joined;
    }
    return joined;
}

char* join_two (char *x, char*y){
    int lenX = strlen(x);
    int len = lenX + strlen(y) -1;

    char* joined = (char*) malloc(len+1);
    if(joined == NULL){
        printf("Couln't get space to joing strings, in the join function.\n");
    }
    strcpy(joined, x);
    strcpy(&joined[lenX], y);
    return joined;
}
