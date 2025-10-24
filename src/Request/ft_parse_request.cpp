#include "../../Includes/request/http_parsing.hpp"

bool parseChunkedBody(std::istringstream& stream, std::string& body )
{
    std::string completeBody; 
    std::string line;
    
    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::cout << "Raw chunk size line: '->" << line << "<-" << std::endl;
        
        // Add error handling for invalid hex
        int chunkSize = 0;
        try {
            chunkSize = std::stoi(line, nullptr, 16);
        } catch (const std::exception& e) {
            std::cout << "Error parsing chunk size: " << e.what() << std::endl;
            return false;
        }
        
        std::cout << "Chunk size: " << chunkSize << " bytes" << std::endl;
        
        if (chunkSize == 0)
            break;;
        
        std::string readChunk;
        readChunk.resize(chunkSize);
        stream.read(&readChunk[0], chunkSize);
        
//   if (stream.gcount() != chunkSize) {
//             std::cout << "Error: Expected " << chunkSize << " bytes, got " << stream.gcount() << std::endl;
//             return false;
//         } 3lash  drti hafiii
        
        completeBody += readChunk;  
        std::cout << "Chunk content: '" << readChunk << "'" << "Actually read: " << stream.gcount() << " bytes" << std::endl;
        
        // Skip the \r\n after chunk data
        std::getline(stream, line);
    }
    
    body = completeBody;
    std::cout << "Complete body ✅ "<< std::endl; 
    return true;
}