#pragma once

#include <vector>
#include <string>


///<summary>Splits c-style string by delimiter and trims each new string</summary>
///<param name="s">string to split</param>
///<param name="len">string length</param>
///<param name="delimiter">delimeter character</param>
///<returns>array of strings after the split</returns>
std::vector<std::string> split(const char* s, int len, char delimiter);

///<summary>Trims string by \r\n\t characters from both ends</summary>
///<param name="s">string to trim</param>
///<returns>new trimmed string</returns>
void trim_str(std::string& str);