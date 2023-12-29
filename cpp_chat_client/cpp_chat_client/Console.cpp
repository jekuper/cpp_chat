#include "Console.h"

namespace CustomConsole {

	int getChar() {
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		if (hStdin == INVALID_HANDLE_VALUE) {
			return EOF;
		}

		DWORD mode;
		if (!GetConsoleMode(hStdin, &mode)) {
			return EOF;
		}

		SetConsoleMode(hStdin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));

		int ch = _getch();

		if (ch == '\r')
			ch = '\n';

		SetConsoleMode(hStdin, mode);

		return ch;
	}

	Console::Console() {}

	std::string Console::read() {
		{
			std::lock_guard<std::mutex> lock(_mtx);
			_prompt = "> ";
			_input.clear();
			std::cout << _prompt << std::flush;
		}

		enum {
			Enter = '\n',
			BackSpc = 8
		};

		for (;;) {
			switch (int c = getChar()) {
			case Enter: {
				std::lock_guard<std::mutex> lock(_mtx);
				std::string input = _input;
				_prompt.clear();
				_input.clear();
				std::cout << std::endl;
				return input;
			}
			case BackSpc: {
				std::lock_guard<std::mutex> lock(_mtx);
				if (_input.empty())
					break;
				_input.pop_back();
				std::cout << "\b \b" << std::flush;
			}
						  break;
			default: {
				if (c < ' ' || c >= '\x7f')
					break;
				std::lock_guard<std::mutex> lock(_mtx);
				_input += c;
				std::cout << (char)c << std::flush;
			}
					 break;
			}
		}
	}

	void Console::write(const char* text, size_t len) {
		if (!len)
			return;

		bool eol = text[len - 1] == '\n';
		std::lock_guard<std::mutex> lock(_mtx);

		if (size_t size = _prompt.size() + _input.size())
		{
			std::cout
				<< std::setfill('\b') << std::setw(size) << ""
				<< std::setfill(' ') << std::setw(size) << ""
				<< std::setfill('\b') << std::setw(size) << "";
		}

		std::cout << text;

		if (!eol)
			std::cout << std::endl;

		std::cout << _prompt << _input << std::flush;
	}

	void Console::write(const char* text) {
		write(text, strlen(text));
	}

	void Console::write(const std::string& text) {
		write(text.c_str(), text.size());
	}
}
