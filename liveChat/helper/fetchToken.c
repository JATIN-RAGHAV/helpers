#include "stdio.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/joinStrings.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/read.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/https.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/helper.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/cJSON.h"
#include "stdlib.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>


char* get_request_first(char* code);
char** parse_response(data_t* response);

char* TOKEN_ENDPOINT = "https://oauth2.googleapis.com";

char** fetchToken(char* code){
    char* request = get_request_first(code);
    if(request == NULL){
        return NULL;
    }

    data_t* response = client(request, TOKEN_ENDPOINT);
    free(request);
    if(response == NULL){
        perror("Couldn't get token in token function\n");
        return NULL;
    }

    char** tokens = parse_response(response);
    if(tokens == NULL){
        printf("Couln't parse tokens\n");
        return NULL;
    }
    return tokens;

}

char** parse_response(data_t* response){
    char** ans = (char**)malloc(sizeof(char*));
    ans[0] = ans[1] = 0;
    cJSON* obj;
    for(int i = 0;i<response->length;i++){
        if(response->request[i] == '{'){
            obj = cJSON_ParseWithLength(&response->request[i],response->length-i+1);
        }
    }

    cJSON* item;
    cJSON_ArrayForEach(item, obj){
        if(strcmp(item->string, "access_token") == 0){
            ans[0] = item->valuestring;
        }
        else if(strcmp(item->string, "refresh_token") == 0){
            ans[1] = item->valuestring;
        }
    }
    if(ans[1] != 0 && ans[0] != 0){
        return ans;
    }
    return NULL;

}

char* get_request_first(char* code){
    char* header = "POST /token HTTP/1.1\r\n"
        "Host: oauth2.googleapis.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: ";


    char* client_id = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientId");
    char* client_secret = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientSecret");
    char* body_arr[] = {
        "grant_type=authorization_code&",
        "code=",code,"&",
        "client_id=",client_id,"&",
        "client_secret=",client_secret,"&",
        "prompt=consent&redirect_uri=https://jatinraghav.duckdns.org"};
    char* body = join(body_arr,11);
    int body_length = strlen(body);
    free(client_id);
    free(client_secret);
    char* response_arr[] = {header,
        itoa(body_length),
        "\r\n\r\n",
        body};
    char* request = join(response_arr,4);
    free(body);
    return request;
}

