#include "SharedConfigs.h"
#include <map>
#include <string>
#include <iostream>

namespace shared {
	std::map<std::string, std::string> Get_keyword_arguments(int argc, char* argv[]) {
		std::map<std::string, std::string> result;
		for (int i = 0; i < argc; i++) {
			if (strlen(argv[i]) > 0 && argv[i][0] == '-') {
				std::string val = (i + 1 < argc ? argv[i + 1] : "");
				
				size_t pos = 0;
				while (argv[i][pos] == '-') {
					++pos;
				}
				
				result[std::string(argv[i] + pos)] = val;
			}
		}

		return result;
	}
	
	bool validate_arguments(std::map<std::string, std::string> argk) {
		if (argk["name"].size() == 0) {
			std::cout << "Username is not provided\nUse --name \"myUserName\"\n";
			return false;
		}
		if (argk["target"].size() == 0) {
			std::cout << "Target Username is not provided\nUse --target \"127.0.0.1\"\n";
			return false;
		}
		if (argk["server"].size() == 0) {
			std::cout << "Server IP is not provided\nUse --server \"127.0.0.1\"\n";
			return false;
		}
		return true;
	}
}