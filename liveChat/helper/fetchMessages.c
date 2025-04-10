#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/read.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/joinStrings.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/https.h"
#include <stdio.h>
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include "unistd.h"

#define POLING_TIME 5

typedef struct {
    char* name;
    char* msg;
}message;

typedef struct {
    message* chats;
    char* nextPageToken;
    int len;
}chats;

static char* SERVER_DOMAIN = "https://youtube.googleapis.com";
chats* parse_get_messages(char* response);
char* getNextMessageRequest(char* token,char* live_chat_id ,char* next_token);
char* getFirstMessageRequest(char* token,char* live_chat_id );

void fetchMessages(char* token, char* live_chat_id){
    char* request = getFirstMessageRequest(token, live_chat_id);
    if(request == NULL){
        printf("Couldn't generate request to get messages\n");
    }

    data_t* response = client(request, SERVER_DOMAIN);
    if(response == NULL){
        printf("Couln't get response from server to fetch messages\n");
    }

    chats* chats = parse_get_messages(response->request);
    char* nextPageToken;

    printf("\033[1;34mLive Chats Below\033[0m\n\n");
    if(chats == NULL){
        printf("\033[1;34mCouln't get chats\033[0m\n");
    }
    else if(chats->len == 0){
        printf("\033[1;34mNo chats yet\033[0m\n");
    }
    for(int i = 0;i<chats->len;i++){
        printf("%s:  %s\n",chats->chats[i].name, chats->chats[i].msg);
    }
    nextPageToken = chats->nextPageToken;
    free(chats);

    while(1){
        request = getNextMessageRequest(token, live_chat_id, nextPageToken);
        if(request == NULL){
            printf("\033[1;34mCouln't get new request\033[0m");
            break;
        }

        response = client(request, SERVER_DOMAIN);
        if(response == NULL){
            printf("\033[1;34mCouln't get response from server to fetch messages\033[0m\n");
        }

        chats = parse_get_messages(response->request);
        if(chats == NULL){
            printf("\033[1;34mCouln't get chats\033[0m\n");
        }
        else if(chats->len != 0){
            for(int i = 0;i<chats->len;i++){
                printf("\033[1;31m%s\033[0m:  \033[1;32m%s\033[0m\n",chats->chats[i].name, chats->chats[i].msg);
            }
        }
        nextPageToken = chats->nextPageToken;
        free(chats);
        sleep(POLING_TIME);
    }

    printf("Next Page Token: %s\n",chats->nextPageToken);

}

chats* parse_get_messages(char* response){
    cJSON* obj;
    int i = 0;
    while(response[i] != '{')
        i++;

    obj = cJSON_Parse(&response[i]);
    if(obj == NULL){
        printf("Coulnt' parse response\n");
        return NULL;
    }

    cJSON* items;
    int msgIndex = 0;
    message* messages;
    chats* ans = (chats*)malloc(sizeof(chats));
    cJSON_ArrayForEach(items, obj){
        if(strcmp(items->string, "nextPageToken") == 0){
            ans->nextPageToken = items->valuestring;
        }
        else if(strcmp(items->string, "items") == 0){
            int messages_count = cJSON_GetArraySize(items);
            if(messages_count == 0){
                ans->len = 0;
                ans->chats = NULL;
                return ans;
            }
            else{
                messages = (message*)malloc(sizeof(message)*messages_count);
                memset(messages, 0, sizeof(message)*messages_count);
                ans->chats = messages;
                ans->len = messages_count;
            }
            cJSON* item;
            cJSON_ArrayForEach(item, items){
                cJSON* msg;
                cJSON_ArrayForEach(msg, item){
                    if(strcmp(msg->string,"snippet") == 0){
                        cJSON* textMsg;
                        cJSON_ArrayForEach(textMsg, msg){
                            if(strcmp(textMsg->string, "displayMessage") == 0){
                                    messages[msgIndex].msg = textMsg->valuestring;
                            }
                        }
                    }
                    else if(strcmp(msg->string,"authorDetails") == 0){
                        cJSON* displayName;
                        cJSON_ArrayForEach(displayName, msg){
                            if(strcmp(displayName->string, "displayName") == 0){
                                messages[msgIndex].name = displayName->valuestring;
                                msgIndex++;
                            }
                        }
                    }
                }
            }
        }
    }
    return ans;
}

char* getFirstMessageRequest(char* token,char* live_chat_id ){
    char* clientId = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientId");
    if(clientId == NULL){
        printf("Couln't read client id");
        return NULL;
    }
    char* request_line_array[] = {"GET /youtube/v3/liveChat/messages?part=snippet,authorDetails&fields=nextPageToken,items(snippet(displayMessage),authorDetails(displayName))&liveChatId=",live_chat_id,"&key=",clientId," HTTP/1.1\r\n"};
    char* request_line = join(request_line_array,5);

    char* header_auth_less = "Host: youtube.googleapis.com\r\n"
                    "Accept: application/json\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Authorization: Bearer ";
    char* header_list[] = {request_line,header_auth_less,token,"\r\n\r\n"};
    char* header_auth_full = join(header_list, 4);
    if(header_auth_full == NULL){
        printf("Couln't create live id getter header\n");
        return NULL;
    }

    return header_auth_full;
}

char* getNextMessageRequest(char* token,char* live_chat_id ,char* next_token){
    char* clientId = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientId");
    if(clientId == NULL){
        printf("Couln't read client id");
        return NULL;
    }
    char* request_line_array[] = {"GET /youtube/v3/liveChat/messages?part=snippet,authorDetails&fields=nextPageToken,items(snippet(displayMessage),authorDetails(displayName))&pageToken=",next_token,"&liveChatId=",live_chat_id,"&key=",clientId," HTTP/1.1\r\n"};
    char* request_line = join(request_line_array,7);

    char* header_auth_less = "Host: youtube.googleapis.com\r\n"
                    "Accept: application/json\r\n"
                    "Content-Type: application/x-www-form-urlencoded\r\n"
                    "Authorization: Bearer ";
    char* header_list[] = {request_line,header_auth_less,token,"\r\n\r\n"};
    char* header_auth_full = join(header_list, 4);
    if(header_auth_full == NULL){
        printf("Couln't create live id getter header\n");
        return NULL;
    }

    return header_auth_full;
}
