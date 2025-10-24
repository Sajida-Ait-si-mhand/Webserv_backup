#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <sys/socket.h> 
#include <fstream>  // for file handling in GET requests
struct Request_parsing{
	static std::string Method;
	static std::string rawRequest;
	static std::string Version;
};


void ft_parse_request(const std::string& StrREQ, int clientSocket);
bool parseChunkedBody(std::istringstream& stream, std::string& body);
bool handlePostRequest(const std::map<std::string, std::string>& headers, 
                     std::istringstream& stream,
                     std::string& requestBody);