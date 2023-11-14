#include "utils.h"

using namespace std;

std::vector<std::string> split(const char* s, int len, char delimiter)
{
	vector<string> result = vector<string>();

	string buf = "";
	for (int i = 0; i < len; i++) {
		if (s[i] == delimiter) {
			result.push_back(buf);
			buf = "";
		}
		else {
			buf += s[i];
		}
	}
	result.push_back(buf);

	return result;
}
