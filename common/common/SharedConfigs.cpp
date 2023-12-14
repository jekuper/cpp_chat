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
			std::cout << "Invalid name argument.";
			return false;
		}
		if (argk["target"].size() == 0) {
			std::cout << "Invalid target IP.";
			return false;
		}
		if (argk["server"].size() == 0) {
			std::cout << "Invalid server IP.";
			return false;
		}
		return true;
	}
}