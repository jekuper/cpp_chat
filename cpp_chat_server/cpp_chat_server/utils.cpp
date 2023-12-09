#include "utils.h"

using namespace std;

std::vector<std::string> split(const char* s, int len, char delimiter)
{
	vector<string> result = vector<string>();

	string buf = "";
	for (int i = 0; i < len; i++) {
		if (s[i] == delimiter) {
			trim_str(buf);
			result.push_back(buf);
			buf = "";
		}
		else {
			buf += s[i];
		}
	}
	trim_str(buf);
	result.push_back(buf);

	return result;
}

void trim_str(string& str) {
	size_t start = str.find_first_not_of(" \t\n\r");

	if (start == std::string::npos) {
		return;
	}

	size_t end = str.find_last_not_of(" \t\n\r");

	str = str.substr(start, end - start + 1);

	return;
}