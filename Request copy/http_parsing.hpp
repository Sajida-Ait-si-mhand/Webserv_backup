#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <map>

struct Request_parsing{
	static std::string Method;
	static std::string rawRequest;
	static std::string Version;
};