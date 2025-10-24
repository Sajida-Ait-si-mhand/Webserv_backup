
#include "utils.hpp"

std::string Utils::trim(const std::string& line)
{

    size_t first = line.find_first_not_of(" \t\n\r");
    if(first == std::string::npos)
        return("");
    size_t last = line.find_last_not_of(" \t\n\r");
        return(line.substr(first, (last - first + 1)));

}

std::string Utils::removeComment(const std::string& line)
{
    
    size_t pos = line.find('#');
    if(pos != std::string::npos)
        return(line.substr(0, pos));
    return(line);

}

bool Utils::isServerBlockDeclaration(const std::string& line)
{
    if (line.find("server") != 0)
        return false;
    
    
    if (line == "server")
        return true;
        

    if (line.length() > 6)  
    {
        size_t brace_pos = line.find("{");
        if (brace_pos != std::string::npos)
        {
            std::string between = line.substr(6, brace_pos - 6);
            between = trim(between);
            if(between.empty())
                return true;
            else
                return false;
        }
    }
    
    return false;
}

void Utils::validateDirective(const std::string& line)
{
    
    if (isServerBlockDeclaration(line) || line.find("location") == 0 || 
        line == "{" || line == "}" || line.find("{") != std::string::npos)
        return;
    
    
    if (!line.empty() && line[line.length() - 1] != ';')
        throw std::runtime_error("Missing semicolon at end of directive: " + line);
    
   
    size_t semicolon_pos = line.find(';');
    if (semicolon_pos != std::string::npos && semicolon_pos < line.length() - 1)
    {
        std::string after_semicolon = line.substr(semicolon_pos + 1);
        after_semicolon = trim(after_semicolon);
        if (!after_semicolon.empty())
            throw std::runtime_error("Extra tokens after semicolon: " + after_semicolon);
    }
}


void Utils::validateBraceTokens(const std::string& line, char brace)
{
    size_t brace_pos = line.find(brace);
    if (brace_pos != std::string::npos && brace_pos < line.length() - 1)
    {
        std::string after_brace = line.substr(brace_pos + 1);
        after_brace = trim(after_brace);
        if (!after_brace.empty())
        {
            throw std::runtime_error("Extra tokens after brace :" + line );
        }
    }
}

std::string Utils::removeSemicolon(const std::string& str)
 {
    size_t pos = str.find(';');
    if (pos != std::string::npos)
        return str.substr(0, pos);
    return str;
}
 
bool Utils::isNumber(std::string& str)
{
    if(str.empty())
        return false;
    for(size_t i = 0; i < str.size() ; i++)
    {
        if(!std::isdigit(str[i]))
            return false;
    }
    return true;
}

bool Utils::check_IP(std::string& str)
{
    if(str.empty())
        return false;
    if(str[0] == '.' || str[str.size() - 1] == '.')
        return false;

    if(str.find("..") != std::string::npos)
        return false;

    std::stringstream  iss(str);
    std::string token;
    std::vector<std::string> parts;

    while(getline(iss, token, '.'))
        parts.push_back(token);

    if(parts.size() != 4)
        return false;

    for(size_t i = 0; i < parts.size(); i++)
    {
        if(!Utils::isNumber(parts[i]))
            return false;
        if(parts[i].length() > 1 && parts[i][0] == '0')
            return false;
        if(std::atoi(parts[i].c_str()) < 0 || std::atoi(parts[i].c_str()) > 255)
            return false;
    }
    return true;
    
}

bool Utils::check_hostname(std::string& str)
{
    if(str.empty() || str.length() > 253)
        return false;
    if(str == "localhost" || str == "*")
        return true;

    if(str[0] == '.' || str[str.size() - 1] == '.')
        return false;

    if(str.find("..") != std::string::npos)
        return false;
     if(str.find(".") == std::string::npos)
        return false;

    if(check_IP(str) == true)
        return false;

    std::vector<std::string> parts;
    std::stringstream iss(str);

    std::string token;

    while(getline(iss, token, '.'))
        parts.push_back(token);

    for(size_t i = 0; i < parts.size() ; i++)
    {
        if(parts[i].length() > 63)
            return false;
        if(!std::isalnum(parts[i][0]) || !std::isalnum(parts[i][parts[i].length() - 1]))
            return false;
        for(size_t j = 0; j < parts[i].length(); j++)
        {
            if(!std::isalnum(parts[i][j]) && parts[i][j] != '-')
            return false;
        }
        if(parts[i].find("--") != std::string::npos)
            return false;
    }
    return true;

}

bool Utils::check_host(std::string& str)
{
    if(str.empty())
        return false;
    return (check_IP(str) || check_hostname(str));
}

bool Utils::check_path( std::string& str)
{
    if (str.empty() )
        return false;
    if(str[0] != '/')
        return false;
    if (str[str.size() - 1] == '.')
        return false;
    if (str.find("..") != std::string::npos || str.find("//") != std::string::npos)
        return false;

    for (size_t i = 0; i < str.size(); i++)
    {
        char c = str[i];
        if (!(std::isalnum(c) || c == '-' || c == '_' || c == '/' || c == '.'))
        {
            return false; 
        }
    }

    return true;
}

bool Utils::check_index(std::string& file)
{
    if (file.empty() || file.length() > 255)
        return false;

    
    if (file[0] == '-')
        return false;

    if (file.find("..") != std::string::npos || file.find("//") != std::string::npos)
        return false;
    
    for (size_t i = 0; i < file.size(); i++) {
        char c = file[i];
        if (!(std::isalnum(c) || c == '_' || c == '-' || c == '.')) {
            return false;
        }
    }

    return true;
}

bool Utils::check_code(std::string& code)
{
    // HTTP defines client errors (400–499) and server errors (500–599).
    if(code.empty())
        return false;
    if(!Utils::isNumber(code))
        return false;
    if(std::atoi(code.c_str()) < 400 || std::atoi(code.c_str()) > 599)
        return false;
    return true;
}

bool Utils::check_methods(std::string& method)
{
    if(method.empty())
        return false;
    if(method != "GET" && method != "POST" && method != "DELETE")
        return false;
    return(true);
}

bool Utils::check_cgi(std::string& code)
{
    
    if(code.empty())
        return false;
    if(code[0] != '.')
        return false;
    if(!(code == ".py" || code == ".php"))
        return false;
    
    return true;
}
