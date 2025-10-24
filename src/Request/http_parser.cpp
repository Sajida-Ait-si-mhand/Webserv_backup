
// #include <vector>
#include "../../Includes/request/http_parsing.hpp"

// 	//istringstram hiya wahed l class likat khlik treadi mn cin (character input) (from the keyboard)
// 	// Hnaya create L input bax nhet fiha line dyal request dyali:

std::string Request_parsing::Method = "";
std::string Request_parsing::rawRequest = "";
std::string Request_parsing::Version = "";

void ft_parse_request(const std::string& StrREQ, int clientSocket)
{
	std::istringstream stream(StrREQ);
	std::string line;

	if(std::getline(stream, line))
	{
		std::istringstream request_line(line);
		request_line >>  Request_parsing::Method >>  Request_parsing::rawRequest >>  Request_parsing::Version; 

		 
        std::cout << "Method:>> " << Request_parsing::Method << std::endl;
        std::cout << "Path:>> " << Request_parsing::rawRequest << std::endl;
        std::cout << "Version:>> " << Request_parsing::Version << std::endl;
	}
	if (  Request_parsing::Method != "GET" &&  Request_parsing::Method != "POST" && Request_parsing::Method != "DELETE"){
		std::cout << "METHOD NOT SUPPORTED: 405 :" << Request_parsing::Method << std::endl;
		return ;
	}
	// Create a map to hold the headers galha liya chat hh
	std::cout << "\n=== Header PARSING ===" << std::endl;
	std::map<std::string, std::string> headers;
	while(std::getline(stream, line))
	{

		if (!line.empty() && line.back() == '\r') {
        	line.pop_back();
    	}
		if (line.empty())
		{
			std::cout << "DEBUG: End of headers reached" << std::endl;
			break;
		}

		// Hado bash nsipariw the header w l value 
		size_t pos = line.find(":");
        if (pos != std::string::npos && pos > 0)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
			while (!key.empty() && key.back() == ' ') {
            	key.pop_back();
        	}
            if (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            if (!key.empty()) {
				headers[key] = value;  
				std::cout << "Header: '" << key << "' = '" << value << "'" << std::endl;
			}  
			else {
    			std::cout << "WARNING: EMPTY HEADER" << std::endl;  // Fixed typo and format
			}
        }
		else{
			std::cout << "WARNING: Invalid header format (no colon or colon at start)" << std::endl;
		}
	}
	std::string requestBody;
	bool success = false;
	std::string response;

	if (Request_parsing::Method == "POST") {
		success = handlePostRequest(headers, stream, requestBody);
        std::cout << "Handling POST request..." << std::endl;
        if(success) {
            std::cout << "POST body successfully parsed: " << requestBody << std::endl;
            
            // Send a JSON response
            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: application/json\r\n";
            response += "Access-Control-Allow-Origin: *\r\n";
            response += "Content-Length: 30\r\n";
            response += "\r\n"; 
            response += "{\"status\":\"success\",\"message\":\"Data received\"}";
            
            send(clientSocket, response.c_str(), response.length(), 0);
            std::cout << "Sent JSON response to client" << std::endl;
        } else {
			std::cout << "Failed to parse POST body" << std::endl;
			// Send a 400 Bad Request response
			response = "HTTP/1.1 400 Bad Request\r\n";
			response += "Content-Length: 0\r\n";
			response += "\r\n";
			send(clientSocket, response.c_str(), response.length(), 0);
			std::cout << "Sent 400 Bad Request response to client" << std::endl;
		}
	}
	else if (Request_parsing::Method == "GET") {
    std::cout << "Handling GET request..." << std::endl;
    
    // Check if requesting root or index.html
    if (Request_parsing::rawRequest == "/" || 
        Request_parsing::rawRequest == "/index.html") {
        
        // Read the HTML file
        std::ifstream htmlFile("./index.html");
        std::string htmlContent;
        
        if (htmlFile) {
            // Read file into string
            std::stringstream buffer;
            buffer << htmlFile.rdbuf();
            htmlContent = buffer.str();
            
            // Create success response with the HTML content
            response = "HTTP/1.1 200 OK\r\n";
            response += "Content-Type: text/html\r\n";
            response += "Content-Length: " + std::to_string(htmlContent.length()) + "\r\n";
            response += "\r\n";
            response += htmlContent;
            
            // Send the HTML file
            send(clientSocket, response.c_str(), response.length(), 0);
            std::cout << "Sent HTML page to client" << std::endl;
        } else {
            // File not found
            response = "HTTP/1.1 404 Not Found\r\n";
            response += "Content-Type: text/plain\r\n";
            response += "Content-Length: 13\r\n";
            response += "\r\n";
            response += "File not found";
            
            send(clientSocket, response.c_str(), response.length(), 0);
            std::cout << "Sent 404 Not Found response to client" << std::endl;
        }
    }
}
	else if (Request_parsing::Method == "DELETE") {
		// DELETE-specific handling
		std::cout << "Handling DELETE request..." << std::endl;
	}
	
	// std::cout << "\n=== BODY PARSING ===" << std::endl;
	// std::string parseBody;
	// if (headers.find("Transfer-Encoding") != headers.end() && headers["Transfer-Encoding"] == "chunked")
	// {
	// 	std::cout << "Parsing CHUNKED body..." << std::endl;
	// 	parseChunkedBody(stream, parseBody);
	// }
	// else if (headers.find("Content-Length") != headers.end())
	// {
	// 	std::string contentLenStr = headers["Content-Length"];
	// 	int contentLen = std::stoi(contentLenStr);
	// 	std::cout << "Parsing String body..." <<  "Body size:" << contentLen << "Bytes"<< std::endl;
	// 	std::string body;
	// 	body.resize(contentLen);
	// 	stream.read(&body[0], contentLen);
	// 	std::cout << "Body content: " << body << std::endl;
	// }
	// else 
	// {
	// 	std::cout << "No Content-Length - no body to parse" << std::endl;
	// }
	std::cout << "\n--------------Parsing complete!--------------------\n" << std::endl;

}