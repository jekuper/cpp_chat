#include "SharedConfigs.h"
#include <map>
#include <string>

namespace shared {
	std::map<std::string, std::string> Get_keyword_arguments(const int argc, const char* argv[]) {
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
}