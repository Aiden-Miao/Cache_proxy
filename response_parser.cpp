#include "response_parser.hpp"

void response_parser::parse_response(){
	//parse status code
	size_t status_end = response.find_first_of("\r\n");
	size_t status_begin = response.find_first_of(" ") + 1;
	size_t status_len = status_end - status_begin;
	status = response.substr(status_begin, status_len);

	//parse status number
	status_num = response.substr(status_begin, 3);

	//parse cache_control
	size_t cache_control_begin;
	if((cache_control_begin = response.find("Cache-Control:")) != string::npos){
		cache_control_begin ++;
		size_t cache_control_end = response.find("\r\n", cache_control_begin);
		size_t cache_control_len = cache_control_end - cache_control_begin;
		cache_control = response.substr(cache_control_begin, cache_control_len);
	}

	//parse expire
	size_t expire_begin;
	if((expire_begin = response.find("Expires:")) != string::npos){
		expire_begin = expire_begin + 8 + 1; //skip Expires:
		size_t expire_end = response.find("\r\n", expire_begin);
		size_t expire_len = expire_end - expire_begin;
		expire = response.substr(expire_begin, expire_len);
	}

	//parse date
	size_t date_begin;
	if((date_begin = response.find("Date:")) != string::npos){
		date_begin = date_begin + 5 + 1;
		size_t date_end = response.find("\r\n", date_begin);
		size_t date_len = date_end - date_begin;
		date = response.substr(date_begin, date_len);
	}

	//parse last_modifed
	size_t last_modifed_begin;
	if((date_begin = response.find("Last-Modified:")) != string::npos){
		last_modifed_begin = last_modifed_begin + 14 + 1;
		size_t last_modifed_end = response.find("\r\n", last_modifed_begin);
		size_t last_modifed_len = last_modifed_end - last_modifed_begin;
		last_modifed = response.substr(last_modifed_begin, last_modifed_len);
	}

	//parse e-tag 
	size_t E_tag_begin;
	if((E_tag_begin = response.find("ETag:")) != string::npos){
		E_tag_begin = E_tag_begin + 5 + 1;
		size_t E_tag_end = response.find("\r\n", E_tag_begin);
		size_t E_tag_len = E_tag_begin - E_tag_end;
		E-tag = response.substr(E_tag_begin, E_tag_len);
	}

	//parse content-length
	size_t content_length_begin;
	if((content_length_begin = response.find("Content-Length:")) != string::npos){
		content_length_begin = content_length_begin + 15 + 1;
		content_length_end = response.find("\r\n", content_length_begin);
		content_length_len = content_length_end - content_length_begin;
		content-length = response.substr(content_length_begin, content_length_len);
	}

	
	//see if use chunk to transmit
	if((response.find("chunked") != string::npos)){
		chunk = 1;
	}

	//status valid or invalid
	if(status == "200 OK"){
		status_valid = 1;
	}

	//find age in cache-control
	size_t age_begin = cache_control.find("max-age=");
	age = cache_control.substr(age_begin);




}	



