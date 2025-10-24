#ifndef Utils_HPP
#define Utils_HPP

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"


#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include "config.hpp"





class Utils
{
       
        public:
        static void validateBraceTokens(const std::string& line, char brace);
        static void validateDirective(const std::string& line);
        static bool isServerBlockDeclaration(const std::string& line);
        static std::string removeComment(const std::string& line);
        static std::string trim(const std::string& line);
        static std::string removeSemicolon(const std::string& str);
        static bool isNumber(std::string& str);
        static bool check_host(std::string& str);
        static bool check_IP(std::string& str);
        static bool check_hostname(std::string& str);
        static bool check_path(std::string& str);
        static bool check_index(std::string& file);
        static bool check_code(std::string& file);
        static bool check_methods(std::string& file);
        static bool check_cgi(std::string& file);
        
        
};  

#endif