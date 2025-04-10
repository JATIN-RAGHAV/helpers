#include "string"
#include <curl/curl.h>
#include <sstream>
#include <iostream>
#include <fstream>

std::string get_rfc(int num);

int main(int argc, char** argv){
        if(argc == 2){
                int number;
                try{
                        number = std::stoi(argv[1]);
                }
                catch(...){
                        std::cout << "Are you an orphan?\nEnter a proper number." << std::endl;
                        exit(1);
                }

                std::string rfc;
                try{
                        rfc = get_rfc(number);
                }
                catch(...){
                        std::cout << "Either there is some problem with the website, or you are a dick and not entering a proper rfc number" << std::endl;
                        exit(1);
                }

                std::cout << rfc;

        }else{
                std::cout << "Be a nice dick head.\nEnter exactly one rfc number at a time" << std::endl;
        }
}

size_t write_call_back(void* contents, size_t size, size_t nmemb, std::string* output) {
	size_t total_size = size * nmemb;
	output->append((char*)contents, total_size);
	return total_size;
}

std::string get_rfc(int num){
	std::stringstream url_stream;https://www.rfc-editor.org/rfc/rfc120.txt
	url_stream << "https://www.rfc-editor.org/rfc/rfc" << num << ".txt";

	CURL* curl = curl_easy_init();

	std::string response;
	response.reserve(4 * 1024 * 1024);

	curl_easy_setopt(curl, CURLOPT_URL, url_stream.str().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

	CURLcode res = curl_easy_perform(curl);
	if(res == CURLE_OK){
		return response;
	}
	else{
		std::cout << response;
		std::cerr << "Couln't get input";
		exit(1);
	}
}
