#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/joinStrings.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/read.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/https.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/helper.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/cJSON.h"
#include <string.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char* get_request();
char* parse_get_access_token(data_t response);
char* SERVER_DOMAIN = "https://oauth2.googleapis.com";

char * fetchAccessToken(char* refresh){
    char* request = get_request();
    if(request == NULL){
        printf("Fuck this ain't workin\n");
        return NULL;
    }
    data_t* response = client(request, SERVER_DOMAIN);
    if(response == NULL){
        printf("Couln't contact the server to get access token with refresh token");
        return NULL;
    }

    char* access_token = parse_get_access_token(*response);
    if(access_token == NULL){
        printf("Couln't parse to get access token\n");
        return NULL;
    }

    return access_token;
}

char* parse_get_access_token(data_t response){

    cJSON* obj;
    for(int i = 0;i<response.length;i++){
        if(response.request[i] == '{'){
            obj = cJSON_ParseWithLength(&response.request[i],response.length-i+1);
        }
    }

    cJSON* item;
    cJSON_ArrayForEach(item, obj){
        if(strcmp(item->string, "access_token") == 0){
            return item->valuestring;
        }
    }
    return NULL;
}

char* get_request(){
    char* header = "POST /token HTTP/1.1\r\n"
        "Host: oauth2.googleapis.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: ";

    char* client_id = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientId");
    if(client_id == NULL){
        printf("Couln't make request to get access token\n");
        return NULL;
    }

    char* client_secret = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientSecret");
    if(client_secret== NULL){
        printf("Couln't make request to get access token\n");
        return NULL;
    }

    char* refresh_token = get_refresh_token();
    if(refresh_token== NULL){
        printf("Couln't make request to get access token\n");
        return NULL;
    }

    char* body_array[] = {"client_id=",client_id,
                        "&client_secret=",client_secret,
                        "&refresh_token=",refresh_token,
                        "&grant_type=refresh_token"};
    char* body = join(body_array,7);
    int body_length = strlen(body);
    if(body == NULL){
        printf("Couln't make request to get access token\n");
        return NULL;
    }

    char* request_arr[] = {header,itoa(body_length),"\r\n\r\n",body};
    char* request = join(request_arr,4);
    if(request == NULL){
        printf("Couln't make request to get access token\n");
        return NULL;
    }

    free(client_id);
    free(client_secret);
    free(refresh_token);
    free(body);
    return request;
}

// POST /token HTTP/1.1
// Host: oauth2.googleapis.com
// Content-Type: application/x-www-form-urlencoded
//
// client_id=your_client_id&
// client_secret=your_client_secret&
// refresh_token=refresh_token&
// grant_type=refresh_token
