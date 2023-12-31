#include "SharedConfigs.h"
#include <map>
#include <string>
#include <iostream>
#include <vector>

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
	
	void validate_arguments(std::map<std::string, std::string>& argk) {
		while (argk["name"].size() == 0) {
			std::cout << "Username is not provided\nPlease type your username: ";
			std::cin >> argk["name"];
			trim_str(argk["name"]);
			system("cls");
		}
		while (argk["target"].size() == 0) {
			std::cout << "Target Username is not provided\nPlease type target username:";
			std::cin >> argk["target"];
			trim_str(argk["target"]);
			system("cls");
		}
		while (argk["server"].size() == 0) {
			std::cout << "Server IP is not provided\nPlease type Server IP:";
			std::cin >> argk["server"];
			trim_str(argk["server"]);
			system("cls");
		}
	}

	void trim_str(std::string& str) {
		size_t start = str.find_first_not_of(" \t\n\r");

		if (start == std::string::npos) {
			return;
		}

		size_t end = str.find_last_not_of(" \t\n\r");

		str = str.substr(start, end - start + 1);

		return;
	}

	std::vector<std::string> split(const char* s, int len, char delimiter)
	{
		std::vector<std::string> result = std::vector<std::string>();

		std::string buf = "";
		for (int i = 0; i < len; i++) {
			if (s[i] == delimiter) {
				shared::trim_str(buf);
				result.push_back(buf);
				buf = "";
			}
			else {
				buf += s[i];
			}
		}
		shared::trim_str(buf);
		result.push_back(buf);

		return result;
	}
}

