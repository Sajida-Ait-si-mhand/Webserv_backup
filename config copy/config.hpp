#ifndef CONFIG_HPP
#define CONFIG_HPP

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"


#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include "utils.hpp"
#include <sstream>

struct locationConfig
{
        std::string path;
        std::string location_root;
        bool autoindex;
        std::vector<std::string> methods;
        std::vector<std::string> lct_IndexPage;
        std::pair<int, std::string> redirect;
        size_t  max_client_body_size;
        std::string upload_path;
        std::map<std::string, std::string> cgi;

        locationConfig();

};

struct serverConfig
{
        unsigned int Port;
        std::string ServerName;
        std::string Host;
        std::string root;
        std::vector<std::string> IndexPage; // because if there is more than one "index.html" it overrides it , and the last wins!!  
        std::map<int, std::string> error_page;
        size_t  max_client_body_size;

        std::vector<locationConfig> locations;

        serverConfig();
};


class config
{
        private:
                std::vector<serverConfig> servers;
        public:
                config();
                ~config();
                void parseConfig(const std::string& filename);
                void parseServer(std::ifstream& file);

                void parseDirective(const std::string& line, serverConfig* currentServer, 
                                locationConfig* currentLocation, bool inLocation);
                void parseServer(const std::string& key, std::istringstream& iss, serverConfig* currentServer);
                void parseLocation(const std::string& key, std::istringstream& iss, locationConfig* currentLocation);

};

#endif