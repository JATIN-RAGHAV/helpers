typedef struct{
	char *request;
	int length;
}data_t;
data_t* server(char* response);
data_t* client(char* request, char* server_domain);
