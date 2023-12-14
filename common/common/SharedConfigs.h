#include <map>
#include <string>
#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

const std::string VERSION = "0.0.1";

namespace shared {
	std::map<std::string, std::string> Get_keyword_arguments(int argc, char* argv[]);
	bool validate_arguments (std::map<std::string, std::string> argk);
}