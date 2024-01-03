#include "Console.h"
#include "SharedConfigs.h"
#include <vector>

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

	Console::Console() {
		_prompt = " : ";
	}
	Console::Console(std::string username) {
		_prompt = username + ": ";
	}

	Console& Console::operator=(const Console& other)
	{
		if (this != &other) { // Avoid self-assignment
			// Perform a deep copy of the data
			std::lock(_mtx, other._mtx);  // Lock both mutexes to avoid deadlock
			std::lock_guard<std::mutex> lockThis(_mtx, std::adopt_lock);
			std::lock_guard<std::mutex> lockOther(other._mtx, std::adopt_lock);

			_input = other._input;
			_prompt = other._prompt;
		}
		return *this;
	}

	std::string Console::read() {
		{
			std::lock_guard<std::mutex> lock(_mtx);
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
				_input.clear();
//				_prompt.clear();
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
	void Console::writeWsource(const std::string message) {
		std::vector<std::string> splitted = shared::split(message.c_str(), message.size(), '|');
		if (splitted.size() < 2) {
			std::cerr << "Failed printing: message doesn't have source\n";
			return;
		}
		std::string source = splitted[0];
		std::string text = splitted[1];

		size_t len = text.size();
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

		std::cout << source << ": " << text;

		if (!eol)
			std::cout << std::endl;

		std::cout << _prompt << _input << std::flush;
	}

	void Console::writeWsource(const char* text) {
		writeWsource(text, strlen(text));
	}

	void Console::writeWsource(const char* text, size_t len) {
		writeWsource(std::string(text, len));
	}
}
