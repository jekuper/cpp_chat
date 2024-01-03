#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <mutex>
#include <string>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include "SharedConfigs.h"
#include <vector>

namespace CustomConsole {

	int getChar();

	class Console {
	private:
		mutable std::mutex _mtx;
		std::string _input;
		std::string _prompt;

	public:
		Console();
		Console(std::string username);

		Console(const Console&) = delete;
		Console& operator=(const Console& other);

		std::string read();
		void writeWsource(const std::string text);
		void writeWsource(const char* text, size_t size);
		void writeWsource(const char* text);
	};

	struct Flags {
		bool stop;
		Console console;

		Flags() : stop(false) {}
		Flags(std::string username) : stop(false) {
			console = Console(username);
		}
	};
}

#endif // CONSOLE_H
