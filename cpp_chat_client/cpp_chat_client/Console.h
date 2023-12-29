#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <mutex>
#include <string>
#include <conio.h>
#include <windows.h>
#include <iomanip>

namespace CustomConsole {

	int getChar();

	class Console {
	private:
		std::mutex _mtx;
		std::string _input;
		std::string _prompt;

	public:
		Console();

		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;

		std::string read();
		void write(const char* text, size_t size);
		void write(const char* text);
		void write(const std::string& text);
	};

	struct Flags {
		bool stop;
		Console console;

		Flags() : stop(false) {}
	};
}

#endif // CONSOLE_H
