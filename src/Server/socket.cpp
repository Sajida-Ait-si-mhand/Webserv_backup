#include "../../Includes/request/http_parsing.hpp"
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return 1;
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Set socket options failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    // 2. Bind
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    // 3. Listen
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "Waiting for connections on port " << PORT << "..." << std::endl;

    // 4. Accept clients in a loop
    while (true) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, &addr_len);
        if (client_fd < 0) {
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;  // Try to accept the next connection
        }

        std::cout << "Client connected" << std::endl;
        
        // 5. Read data from client socket
        memset(buffer, 0, BUFFER_SIZE);  // Clear buffer before each read
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';  // null-terminate
            std::string requestStr(buffer);
            
            std::cout << "Received " << bytes_read << " bytes" << std::endl;
            
            // 6. Call parse function which will handle the response
            ft_parse_request(requestStr, client_fd);
            
            // NOTE: Don't send another response here!
            // ft_parse_request already handles sending responses
        }
        else if (bytes_read < 0) {
            std::cerr << "Read error: " << strerror(errno) << std::endl;
        }
        else {
            std::cout << "Client closed connection" << std::endl;
        }
        
        // Close this client's connection and accept a new one
        close(client_fd);
    }

    // We'll never reach here with the infinite loop, but good practice
    close(server_fd);
    return 0;
}