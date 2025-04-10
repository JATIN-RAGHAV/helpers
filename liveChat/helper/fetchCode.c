#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/https.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/read.h"
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/joinStrings.h"

#define TRUE 1
#define FALSE 0

char* parseCode(data_t rs);
void* fetchCode(void* codePtr){
	data_t* request = server(NULL);
	if(request == NULL){
		printf("Some error happened with the server.\n");
		return NULL;
	}
	void** code = codePtr;
	*code = parseCode(*request);
	return NULL;
}


char* parseCode(data_t rs){
	int isCode = FALSE;
	int index = 0;
	char *code = (char*)malloc(300);
	for(int i = 0;i<rs.length && rs.request[i] != '\n';i++){
		if(rs.request[i] == '=')
			isCode = TRUE;
		else if(isCode == TRUE){
			for(;rs.request[i] != '&' ;i++)
				code[index++] = rs.request[i];
			code[index] = 0;
			code[1] = '/';
			return code;
		}
	}
	return NULL;
}

char* getUrl(){
	char* before = "'https://accounts.google.com/o/oauth2/v2/auth?client_id=";
	char* after = "&redirect_uri=https://jatinraghav.duckdns.org&response_type=code&scope=https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fyoutube%20https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fyoutube.channel-memberships.creator%20https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fyoutube.force-ssl%20https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fyoutube.readonly%20https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fyoutube.upload%20https%3A%2F%2Fwww.googleapis.com%2Fauth%2Fyoutubepartner&access_type=offline'";
	char* client_id = readFile("/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/.clientId");
	if(client_id == NULL){
		printf("Couln't create url, envFile not read.\n");
		return NULL;
	}
	char* strs[] = {before, client_id, after};
	char* url = join(strs,3);
	free(client_id);
	return url;
}
