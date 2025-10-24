#include "../../Includes/request/http_parsing.hpp"

#define MAX_BODY_SIZE 10485760 // 10MB

bool handlePostRequest(const std::map<std::string, std::string>& headers, 
                     std::istringstream& stream,
                     std::string& requestBody)
{
    std::cout << "Processing POST request body..." << std::endl;
    
    // Check for Transfer-Encoding: chunked
    if (headers.find("Transfer-Encoding") != headers.end() && 
        headers.at("Transfer-Encoding") == "chunked")
    {
        std::cout << "Processing chunked encoded body" << std::endl;
        return parseChunkedBody(stream, requestBody);
    }
    // Check for Content-Length
    else if (headers.find("Content-Length") != headers.end())
    {
        try {
            int contentLen = std::stoi(headers.at("Content-Length"));
            
            if (contentLen < 0) {
                std::cerr << "Error: Negative Content-Length value" << std::endl;
                return false;
            }
            
			if (contentLen > MAX_BODY_SIZE) {
				std::cerr << "Error: Content-Length exceeds maximum allowed size" << std::endl;
                return false;
            }
            
            std::cout << "Reading " << contentLen << " bytes from body" << std::endl;
            
            requestBody.resize(contentLen);
            stream.read(&requestBody[0], contentLen);
            
            // Check if we read the expected number of bytes
            if (stream.gcount() != contentLen) {
                std::cerr << "Warning: Read only " << stream.gcount() 
                          << " of " << contentLen << " bytes" << std::endl;
                requestBody.resize(stream.gcount());  // Resize to actual bytes read
            }
            
            std::cout << "Successfully read " << requestBody.size() << " bytes" << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing Content-Length: " << e.what() << std::endl;
            return false;
        }
    }
    // No body information found
    else {
        std::cout << "No Content-Length or Transfer-Encoding header found" << std::endl;
        requestBody = "";  // Empty body
        return true;  // Still successful, just with empty body
    }
}