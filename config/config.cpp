#include "config.hpp"

locationConfig::locationConfig() 
    : autoindex(false), 
      redirect(0, ""), 
      max_client_body_size(1048576){} 

serverConfig::serverConfig() 
: Port(8080), 
    Host("127.0.0.1"), 
    max_client_body_size(1048576) {}

config::config() {}

config::~config() {}




void config::parseConfig(const std::string &filename)
{
   
    std::ifstream readfile(filename.c_str());
    if (!readfile.is_open())
        throw std::runtime_error("Failed to open config file");

    std::string line;
    int count = 0;
    bool inServer = false;
    bool inLocation = false;
    bool blockJustOpened = false; 
    bool foundContent = false;

    serverConfig* currentServer = NULL;
    locationConfig* currentLocation = NULL;

    while (std::getline(readfile, line))
    {
        line =  Utils::trim(Utils::removeComment(line));
        if (line.empty()) continue;
        
        foundContent = true;


        if (line.find("{{") != std::string::npos || line.find("}}") != std::string::npos)
            throw std::runtime_error("Consecutive '{' detected.");
    
        if (!inServer && !Utils::isServerBlockDeclaration(line))
            throw std::runtime_error("Error: Config Error:: unexpected token outside server block: " + line);
        
        if ( Utils::isServerBlockDeclaration(line))  
        {
            if (inServer)
                throw std::runtime_error("Nested server blocks not allowed " + line);
            servers.push_back(serverConfig());
            currentServer = &servers.back();
            
            size_t brace_pos = line.find("{");
            if(brace_pos != std::string::npos)
            {
                Utils::validateBraceTokens(line, '{');
                
                std::string between = line.substr(6, brace_pos - 6);
                between = Utils::trim(between);
                if(!between.empty())
                    throw std::runtime_error("Unexpected token between 'server' and '{' = " + between);
                inServer = true;
                count++;
                blockJustOpened = true;
            }
            else if(line == "server")
            {
                std::string nextLine;
                if(std::getline(readfile, nextLine))
                {
                    nextLine = Utils::trim(Utils::removeComment(nextLine));
                    std::cout << "next :: "<< nextLine << std::endl;
                    if (nextLine != "{")
                        throw std::runtime_error("Extra token after '{' : " + nextLine);
                    
                    Utils::validateBraceTokens(nextLine, '{');
                    
                    inServer = true;
                    count++;
                    blockJustOpened = true;

                }
                else
                    throw std::runtime_error("unexpected end of file after 'server'");
            }
            else
            {
                throw std::runtime_error("Invalid server declaration: " + line);
            }
        }
        else if (line.find("location") == 0)
        {
            if (!inServer)
                throw std::runtime_error("Location block must be inside server " + line);
            if (inLocation)
                throw std::runtime_error("Nested location blocks not allowed " + line);

            currentServer->locations.push_back(locationConfig());
            currentLocation = &currentServer->locations.back();
            
            size_t brace_pos = line.find('{');
            if(brace_pos != std::string::npos)
            {
                
                Utils::validateBraceTokens(line, '{');
                
                std::string between = line.substr(8, brace_pos - 8);
                between = Utils::trim(between);
                std::cout << "between location :: " << between << std::endl;
                if(between.empty() || between[0] != '/')
                    throw std::runtime_error("invalid Location path " + between);
                if(Utils::check_path(between))
                    currentLocation->path = between;
                else
                    throw std::runtime_error("invalid Location between " + between);
                inLocation = true;
                count++; 
                blockJustOpened = true;
            }
            else
            {
                std::string path = line.substr(8);
                path = Utils::trim(path);
                std::cout << "between location :: " << path << std::endl;

                if(path.empty() || path[0] != '/')
                    throw std::runtime_error("invalid Location path " + path);
                if(Utils::check_path(path))
                    currentLocation->path = path;
                else
                    throw std::runtime_error("invalid Location path " + path);
                
                std::string nextLine;
                if(std::getline(readfile, nextLine))
                {
                    nextLine = Utils::trim(Utils::removeComment(nextLine));
                    if (nextLine != "{")
                        throw std::runtime_error("Extra token after '{' location : " + nextLine);
                    
                  
                    Utils::validateBraceTokens(nextLine, '{');
                    
                    inLocation = true;
                    count++;
                    blockJustOpened = true;
                }
                else
                    throw std::runtime_error("unexpected end of file after location");
            }
        }
        else if (line == "}") 
        {
            if (blockJustOpened)
                throw std::runtime_error("Empty block '{}' detected " + line);
            
            if (count == 0)
                throw std::runtime_error("Unexpected '}' - no matching '{'");
                
            count--;
            
            if (inLocation)
            {
                inLocation = false;
                currentLocation = NULL;
            }
            else if (inServer)
            {
                inServer = false;
                currentServer = NULL;
            }
            
            blockJustOpened = false;
        }
        else if (line.find('}') != std::string::npos)
        {
           
            size_t brace_pos = line.find('}');
            std::string before_brace = line.substr(0, brace_pos);
            before_brace = Utils::trim(before_brace);
            if (!before_brace.empty())
                throw std::runtime_error("Tokens before closing brace: " + before_brace);
            
           
            Utils::validateBraceTokens(line, '}');
            
            if (blockJustOpened)
                throw std::runtime_error("Empty block '{}' detected " + line);
            
            if (count == 0)
                throw std::runtime_error("Unexpected '}' - no matching '{'");
                
            count--;
            
            if (inLocation)
            {
                inLocation = false;
                currentLocation = NULL;
            }
            else if (inServer)
            {
                inServer = false;
                currentServer = NULL;
            }
            
            blockJustOpened = false;
        }
        else if (line.find("{") == std::string::npos && line.find("}") == std::string::npos)
        {
            if (!inServer)
                throw std::runtime_error("Directive outside server block: " + line);
            
            Utils::validateDirective(line);

            parseDirective(line, currentServer, currentLocation, inLocation);
                
            // std::cout << " ---------------------->  " << line << std::endl;
            blockJustOpened = false;
        }
        else
            throw std::runtime_error("Unexpected content: " + line);
    }

    readfile.close();

     if (!foundContent)
        throw std::runtime_error("Error :: Empty file");

    if (count != 0)
    {
        std::cout << " ---------------------->  " << count << std::endl;
        throw std::runtime_error("Mismatched braces in config file");
    }
}

void config::parseServer(const std::string& key, std::istringstream& iss, serverConfig* currentServer)
{
    if (key == "listen") {
            std::string port;
            iss >> port;
            if(!Utils::isNumber(port))
                throw std::runtime_error("Error :: Port must be in digits " + port);
            else
                currentServer->Port = std::atoi(port.c_str());
            // std::cout << " listen == " << currentServer->Port <<std::endl;
        }
        else if (key == "host") {
            std::string host;
            iss >> host;
            // std::cout << " host == " << host <<std::endl;
            if( Utils::check_host(host))
                currentServer->Host = host;
            else
                throw std::runtime_error("Error :: Invalid host format: IP adress or hostname :: " + host);
            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after host: " + extra);
        }
        else if (key == "server_name") {
            if(!(iss >> currentServer->ServerName))
                throw std::runtime_error("Missing value for server_name");
        }
        else if (key == "root") {
            std::string root;
            if(!(iss >> root))
                throw std::runtime_error("Error :: Missing value for root");
            
            if(Utils::check_path(root))
                currentServer->root = root;
            else
                throw std::runtime_error("Error :: Invalid root format :: " + root);
            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after root: " + extra);
        }
        else if (key == "index") {
            std::string idx;
            currentServer->IndexPage.clear();
            while (iss >> idx) {
                if(Utils::check_index(idx))
                    currentServer->IndexPage.push_back(idx);
                else
                    throw std::runtime_error("Error :: Invalid index format :: " + idx);
            }
            if(currentServer->IndexPage.size() < 1 )
                throw std::runtime_error("Error :: Invalid index format :: " );
        }
        else if (key == "error_page") {
            std::vector<int> codes;
            std::string token;
            std::string page;
            bool foundPath = false;
            
            while(iss >> token)
            {
                if(token[0] == '/' )
                {
                    if (foundPath)
                        throw std::runtime_error("Error :: Multiple paths in error_page directive: " + token);
                    if(Utils::check_path(token) )
                    {
                        page = token;
                        foundPath = true;
                    }
                    else
                        throw std::runtime_error("Error :: Invalid path in error_page directive: " + token);
                    
                }
                else
                {
                    if(Utils::check_code(token))
                        codes.push_back(std::atoi(token.c_str()));
                    else
                        throw std::runtime_error("Error :: Invalid error page code format :: " + token);
                }
            }
            if (codes.empty())
                throw std::runtime_error("Error :: No error code specified for error_page directive");
             if (page.empty())
                throw std::runtime_error("Error :: No path specified for error_page directive");
            for(size_t i = 0; i < codes.size(); i++)
            {
                std::cout << "page :: " << page << std::endl;
                currentServer->error_page[codes[i]] = page;
            }
            

        }
        else if (key == "max_client_body_size") {
            std::string token;
            if(! (iss >> token))
                throw std::runtime_error("Missing value for max_client_body_size");
            if(Utils::isNumber(token))
                currentServer->max_client_body_size = std::atoi(token.c_str());
            else
                    throw std::runtime_error("Error :: Invalid max_client_body_size format :: " + token);
            
            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after max_client_body_size: " + extra);
        }
        else
        throw std::runtime_error("Error :: Invalid Server Directive :: " + key);
}

void config::parseLocation(const std::string& key, std::istringstream& iss, locationConfig* currentLocation)
{
     if (key == "root") {

            std::string root;
            if(!(iss >> root))
                throw std::runtime_error("Error :: Missing value for root");
            
            if(Utils::check_path(root))
                currentLocation->location_root = root;
            else
                throw std::runtime_error("Error :: Invalid root format :: " + root);

            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after root: " + extra);

        }
        else if (key == "autoindex") {
            std::string value;
            if(!(iss >> value))
                throw std::runtime_error("Error :: Missing value for autoindex ");
            if (value == "on") 
            {
                currentLocation->autoindex = true;
            } else if (value == "off") 
                currentLocation->autoindex = false;
            else
                throw std::runtime_error("Error :: autoindex must be 'on' or 'off' :: " + value);
            
            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token for autoindex : " + extra);

        }
        else if (key == "method") {
            std::string method;
            currentLocation->methods.clear();
            while (iss >> method) {
                if(Utils::check_methods(method))
                    currentLocation->methods.push_back(method);
                else
                    throw std::runtime_error("Error :: method must be 'GET' or 'POST' or 'DELETE' :: " + method);
            }
            if(currentLocation->methods.empty())
                throw std::runtime_error("Error :: method must be 'GET' or 'POST' or 'DELETE' " );
        }
        else if (key == "index") {
             std::string idx;
            currentLocation->lct_IndexPage.clear();
            while (iss >> idx) {
                if(Utils::check_index(idx))
                    currentLocation->lct_IndexPage.push_back(idx);
                else
                    throw std::runtime_error("Error :: Invalid index format :: " + idx);
            }
            if(currentLocation->lct_IndexPage.size() < 1 )
                throw std::runtime_error("Error :: Invalid index format :: ");

        }
        else if (key == "max_client_body_size") {
            std::string token;
            if(! (iss >> token))
                throw std::runtime_error("Missing value for max_client_body_size");
            if(Utils::isNumber(token))
                currentLocation->max_client_body_size = std::atoi(token.c_str());
            else
                    throw std::runtime_error("Error :: Invalid max_client_body_size format :: " + token);
            
            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after max_client_body_size: " + extra);
        }
        else if (key == "upload_path") 
        {
            std::string token;

            if(! (iss >> token))
                throw std::runtime_error("Missing value for upload_path");
            if(Utils::check_path(token))
                currentLocation->upload_path = token;
            else
                    throw std::runtime_error("Error :: Invalid upload_path format :: " + token);
            if (currentLocation->upload_path.empty())
                throw std::runtime_error("upload_path directive requires a path");
             std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after upload_path: " + extra);
        }
        else if (key == "return") {
            std::string code_str;
            if(! (iss >> code_str))
                throw std::runtime_error("Missing value for return");
            if(Utils::isNumber(code_str))
            {
                int code = std::atoi(code_str.c_str());
                if(code < 300 || code > 399)
                    throw std::runtime_error("return directive requires a 3xx status code");
                currentLocation->redirect.first = code;
            }
            else
                throw std::runtime_error("Invalid status code for return: " + code_str);
            std::string path_str;
            if(! (iss >> path_str))
                throw std::runtime_error("Missing path for return");
            if(Utils::check_path(path_str))
                currentLocation->redirect.second = path_str;
            else
                throw std::runtime_error("Invalid return path: " + path_str);
            
             std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after return: " + extra);
        }
        else if (key == "cgi") {
            std::string ext, handler;
            if(!(iss >> ext))
                throw std::runtime_error("ERROR :: cgi directive requires extension and handler");
            if(!Utils::check_cgi(ext))
                throw std::runtime_error("ERROR :: Invalid extension for CGI");

            if(!(iss >> handler))
                throw std::runtime_error("ERROR :: cgi directive requires a handler");
            if(Utils::check_path(handler))
                currentLocation->cgi[ext] = handler;
            else
                throw std::runtime_error("ERROR :: Invalid handler for CGI " + handler);
            
            std::string extra;
            if (iss >> extra) 
                throw std::runtime_error("Extra token after CGI handler : " + extra);
        }
        else
            throw std::runtime_error("Error :: Invalid Location Directive :: " + key);
    }


void config::parseDirective(const std::string& line, serverConfig* currentServer,
                           locationConfig* currentLocation, bool inLocation)
{
    std::string clean = Utils::removeSemicolon(line);
    
    std::istringstream iss(clean);
    std::string key;
    iss >> key;
    
    if (!inLocation && currentServer)
        parseServer(key,iss, currentServer);
    else if (inLocation && currentLocation)
        parseLocation(key,iss, currentLocation);
}
