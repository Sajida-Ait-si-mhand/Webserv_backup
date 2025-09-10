
#include "http_parsing.hpp"
std::string Request_parsing::rawRequest = "GET /candy HTTP/1.1\nName: Timmy\nFavorite: [chocolate](url)\n";
std::string Request_parsing::Method;
std::string Request_parsing::Version;
int main()
{

	//istringstram hiya wahed l class likat khlik treadi mn cin (character input) (from the keyboard)
	// Hnaya create L input bax nhet fiha line dyal request dyali:
	std::istringstream stream(Request_parsing::rawRequest);
	std::string line; 
	// Bash nji nparsi I need to read it so I need getliinee
	std::getline(stream, line);
	std::istringstream requestLine(line);
	requestLine >>  Request_parsing::Method >>  Request_parsing::rawRequest  >> Request_parsing::Version;
	std::cout << "Action: " << Request_parsing::Method << std::endl;
	std::cout << "Request: " << Request_parsing::rawRequest << std::endl;
	std::cout << "Version: " << Request_parsing::Version << std::endl;

	// Hnaya fash radi nparsi dakchii w dakchii (header && body)hh 
	// Create a map to hold the headers galha liya chat hh
	std::map<std::string, std::string> headers;
	while(std::getline(stream, line) && !line.empty())
	{
		// Hado bash nsipariw the header w l value 
		size_t pos = line.find(":");
		if (!pos)
		{
			std::string key = line.substr(0, pos); // 0 bash nbdaw mn index 0
			std::string value = line.substr(pos+1); // " Value"
			headers[key] = value;                    
		}
	}
	// 🤖 Hadi darha liya AI hh rir to make sure I have no idea ki radi ndir liha saraha :( but if you want to test it please use this Flag : -std=c++11
	// for (const auto& header : headers) {
	// 	std::cout << header.first << ": " << header.second << std::endl;
	// }
	std::string body;
	while(std::getline(stream, body))
	{
		body+= line + "\n";
	}
	// afficher le body.
    std::cout << "\nBody:\n" << body << std::endl;
	// stp ila kayna chi haja goliha liliii stpppp 
}