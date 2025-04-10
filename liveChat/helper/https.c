#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netdb.h>
#include "/Users/jatinraghav/Documents/programming/projects/youtubeStuff/liveChat/headerFiles/cJSON.h"

#define PORT 443  // Use 443 for production
#define CERT_FILE "/Users/jatinraghav/.acme.sh/jatinraghav.duckdns.org_ecc/fullchain.cer"
#define KEY_FILE "/Users/jatinraghav/.acme.sh/jatinraghav.duckdns.org_ecc/jatinraghav.duckdns.org.key"
#define BUFFERSIZE 1024
#define TRUE 1
#define FALSE 0

typedef struct{
	char *request;
	int length;
}data_t;

char* RESPONSE = NULL;
char* REQUEST = NULL;

data_t* SSL_dynamic_read_request(SSL *ssl);
data_t* SSL_dynamic_read_response(SSL *ssl);
char is_complete_request(char *request, int length);
char is_complete_response(char *request, int length);
data_t* SSL_handle_server(int client_fd, SSL_CTX* ctx);
data_t* SSL_handle_client(int client_fd, SSL_CTX* ctx);
data_t* get_client_request(SSL *ssl);
data_t* get_server_response(SSL *ssl);
SSL_CTX *create_server_context();
SSL_CTX *create_client_context();
void configure_server_context(SSL_CTX *ctx);
void init_openssl();
char *getResponse();

data_t* server(char* response){
	if(response != NULL)
		RESPONSE = response;
	init_openssl();
	SSL_CTX *ctx = create_server_context();
	configure_server_context(ctx);
	int server_fd, client_fd;

	struct sockaddr_in6 server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);

	server_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if(server_fd == -1){
		perror("Server socket creation failed\n");
		exit(EXIT_FAILURE);
	}

	server_addr.sin6_family= AF_INET6;
	server_addr.sin6_addr = in6addr_any;
	server_addr.sin6_port = htons(PORT);

	if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Bind Failed\n");
		return NULL;
	}
	if(listen(server_fd, 10) <0){
		perror("Listen Failed\n");
		return NULL;
	}

	client_fd = accept(server_fd, NULL, NULL);
	if (client_fd < 0) {
		perror("Accept failed");
		return NULL;
	}
	data_t* request =  SSL_handle_server(client_fd, ctx);
	close(server_fd);
	SSL_CTX_free(ctx);
	EVP_cleanup();
	return request;
}

data_t* client(char* request, char* server_domain){
	init_openssl();
	SSL_CTX *ctx = create_client_context();
	int client_fd;
	REQUEST = request;

	struct addrinfo hints, *res;
	struct sockaddr_in6 server_addr ;

	client_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if(client_fd < 0){
		perror("Server socket creation failed\n");
		return NULL;
	}
	char ipstr[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(server_domain, NULL, &hints, &res) != 0) {
		perror("getaddrinfo\n");
		return NULL;
	}

	server_addr = *(struct sockaddr_in6 *)res->ai_addr;
	server_addr.sin6_family = AF_INET6;
	server_addr.sin6_port = htons(PORT);
	inet_ntop(AF_INET6, &server_addr.sin6_addr, ipstr, sizeof ipstr);
	if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		perror("Connection failed from client side.\n");
		return NULL;
	}
	data_t* response = SSL_handle_client(client_fd, ctx);
	close(client_fd);
	SSL_CTX_free(ctx);
	EVP_cleanup();
	freeaddrinfo(res);
	return response;
}

data_t* get_server_response(SSL *ssl) {
	if(SSL_write(ssl, REQUEST ,strlen(REQUEST)) < 0){
		perror("Couln't sent response\n");
	}
	data_t* response = SSL_dynamic_read_response(ssl);
	printf("\a");
	SSL_shutdown(ssl);
	SSL_free(ssl);
	return response;
}

data_t* SSL_handle_client(int client_fd, SSL_CTX* ctx){
	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, client_fd);
	data_t* response;
	if(SSL_connect(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
	} else {
		response = get_server_response(ssl);
	}
	close(client_fd);
	return response;
}
data_t* SSL_handle_server(int client_fd, SSL_CTX* ctx){
	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, client_fd);
	data_t* request;
	if(SSL_accept(ssl) <= 0) {
		printf("SSL connection failed");
		return NULL;
	} else {
		request= get_client_request(ssl);
	}
	close(client_fd);
	return request;
}


SSL_CTX *create_server_context() {
	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	return ctx;
}

SSL_CTX *create_client_context() {
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	return ctx;
}

void configure_server_context(SSL_CTX *ctx) {
	if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
}

data_t* get_client_request(SSL *ssl) {
	data_t* request = SSL_dynamic_read_request(ssl);
	printf("\a");
	if (RESPONSE == NULL)
		RESPONSE = getResponse();
	if(SSL_write(ssl, RESPONSE ,sizeof(RESPONSE)) < 0){
		perror("Couln't sent response\n");
	}
	SSL_shutdown(ssl);
	SSL_free(ssl);
	return request;
}

char *getResponse(){
	cJSON* body = cJSON_CreateObject();
	cJSON_AddStringToObject(body,"Have you read about Gilgemish??", "No, you Dick I invented that shit.");
	cJSON_AddStringToObject(body,"Have you read about Hari Sheldon?", "Who hasn't?");
	cJSON_AddStringToObject(body,"You must be aware of the principle of least Action, which governs the working of the whole universe.", "Yup, I can find the equations for anything with the appropriate Lagrange.");
	cJSON_AddStringToObject(body,"Have you seen the monkey.", "uhun");
	cJSON_AddStringToObject(body,"Gott ist", "ToT.");
	char *strBody = cJSON_PrintUnformatted(body);
	const char *header= 
		"HTTP/1.1 200 YOOOO\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: ";  // The length of the body ({"})
	int bodyLen = (strlen(strBody));
	int contentLen = (int)(((ceil(log10(bodyLen))))*sizeof(char));
	char contentLength[contentLen];
	sprintf(contentLength, "%d", bodyLen);
	char *buffer = "\r\n\r\n";
	int headerLen = strlen(header);
	int bufferLen = 4;

	char *response = (char*)malloc(headerLen + contentLen + bufferLen + bodyLen);
	int index = 0;
	memcpy(&response[index], header, headerLen);
	index += headerLen;
	memcpy(&response[index], contentLength, contentLen);
	index += contentLen;
	memcpy(&response[index], buffer, bufferLen);
	index += bufferLen;
	memcpy(&response[index], strBody, bodyLen);
	index += bodyLen;
	free(strBody);
	cJSON_Delete(body);
	return response;
}
data_t* SSL_dynamic_read_request(SSL *ssl) {
	char *received= NULL;
	char buffer[BUFFERSIZE];
	size_t totalSize = 0;
	int receivedBytes;

	while ((receivedBytes = SSL_read(ssl, buffer, BUFFERSIZE)) > 0) {
		char *newdata_t = realloc(received, totalSize + receivedBytes + 1);
		if (!newdata_t) {
			free(received);
			perror("Memory allocation failed");
			data_t err = {NULL, 0};
			return NULL;
		}

		received = newdata_t;
		memcpy(received + totalSize, buffer, receivedBytes);
		totalSize += receivedBytes;
		received[totalSize] = 0; 
		int isComplete = is_complete_request(received, totalSize);
		if(isComplete == TRUE)
			break;
	}
	data_t* req = (data_t*)malloc(sizeof(data_t));
	req->request = received;
	req->length = totalSize;
	return req;
}

data_t* SSL_dynamic_read_response(SSL *ssl) {
	char *received= NULL;
	char buffer[BUFFERSIZE];
	size_t totalSize = 0;
	int receivedBytes;

	while ((receivedBytes = SSL_read(ssl, buffer, BUFFERSIZE)) > 0) {
		char *newdata_t = realloc(received, totalSize + receivedBytes + 1);
		if (!newdata_t) {
			free(received);
			perror("Memory allocation failed");
			data_t err = {NULL, 0};
			return NULL;
		}

		received = newdata_t;
		memcpy(received + totalSize, buffer, receivedBytes);
		totalSize += receivedBytes;
		received[totalSize] = 0; 
		int isComplete = is_complete_response(received, totalSize);
		if(isComplete == TRUE)
			break;
	}
	data_t* req = (data_t*)malloc(sizeof(data_t));
	req->request = received;
	req->length = totalSize;
	return req;
}

char is_complete_response(char *request, int length){
	for(int i = 0;i < length; i++){
		if(request[i] == '\r'){
			if(i+3<length && strncmp(&request[i],"\r\n\r\n",4) == 0){
				i += 3;
				char hexLen[10];
				int index = 0;
				while(request[i] != '\r')
					hexLen[index++] = request[i++];
				i += 2;
				hexLen[index] = 0;
				int body_length = (int)strtol(hexLen, NULL, 16);
				if(body_length <= length - i){
					return TRUE;
				}
				else
					return FALSE;
			}
		}
	}
	return FALSE;

}
char is_complete_request(char *request, int length){
	int contentCompareLength = 15;
	for(int i = 0;i<length; i++){
		if(request[i] == '\n'){
			if(i+contentCompareLength < length){
				int isContent = strncmp(&request[i+1],"Content-Length:",contentCompareLength );
				if(isContent == 0){
					i += contentCompareLength + 2;
					char *intBase = &request[i];
					while(request[i] != '\n')
						i++;

					request[i] = 0;
					int contentLength = atoi(intBase);
					if(contentLength == 0)
						return TRUE;
					request[i] = '\n';
					for(;request[i]== '\n' && i<length;i++){
						if (strncmp(&request[i], "\n\r\n", 3) == 0){
							if(i+contentLength+3 == length)
								return TRUE;
							else
								return FALSE;
						}
					}
				}
			}
			if(strncmp(&request[i], "\n\r\n",3) == 0)
				return TRUE;
		}
	}
	return FALSE;

}

void init_openssl() {
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
}
