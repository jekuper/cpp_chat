#include "utils.h"
#include "SharedConfigs.h"

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