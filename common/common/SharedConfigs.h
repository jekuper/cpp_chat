#include <map>
#include <string>
#pragma once

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

const std::string VERSION = "0.0.2";


///<summary>Namespace contains funtions for both client and server projects</summary>
namespace shared {

	///<summary>Converts argv to key value map of keyword arguments</summary>
	///<param name="argc">argv array size</param>
	///<param name="argv">arguments array</param>
	///<returns>converted keyword arguments</returns>
	std::map<std::string, std::string> Get_keyword_arguments(int argc, char* argv[]);

	///<summary>Checks if essential keyword arguments are valid</summary>
	///<param name="argk">keyword arguments map</param>
	///<returns>True if all arguments are valid. False otherwise</returns>
	bool validate_arguments (std::map<std::string, std::string> argk);
	///<summary>Trims string by \r\n\t characters from both ends</summary>
	///<param name="s">string to trim</param>
	///<returns>new trimmed string</returns>
	void trim_str(std::string& str);
}