#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SharedConfigs.h"
#include "Crypto.h"
#include <thread>
#include "Console.h"

#pragma comment(lib, "Ws2_32.lib")

///<summary>Wrapper over all client authentication data</summary>
class auth_data
{
public:
	///<summary>Client nickname</summary>
	std::string name;

	///<summary>Username of the peer to connect to</summary>
	std::string target_username;

	///<summary>Own RSA public key, base64</summary>
	std::string pubkey;

	auth_data(std::string _name, std::string _target_username, std::string _pubkey) {
		name = _name;
		target_username = _target_username;
		pubkey = _pubkey;
	}
};

const std::string Generate_handshake(auth_data data) {
	return VERSION + "|" + data.name + "|" + data.target_username + "|" + data.pubkey;
}


///<summary>Connects to socket and performs handshake</summary>
int Connect_IP (SOCKET &Server_socket, const char* ip, const addrinfo hints, auth_data data) {
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;

	int iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		std::cout << "Getaddrinfo failed with error: " << iResult << "\n";
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		Server_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (Server_socket == INVALID_SOCKET) {
			std::cout << "Error at socket(): " << WSAGetLastError() << "\n";
			freeaddrinfo(result);
			return 1;
		}

		iResult = connect(Server_socket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR) {
			closesocket(Server_socket);
			Server_socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (Server_socket == INVALID_SOCKET) {
		std::cout << "Failed connecting to server on IP: " << ip << "\n";
		return 1;
	}

	const std::string handshake = Generate_handshake(data);
	std::cout << "Sending handshake...\n";
	iResult = send(Server_socket, handshake.c_str(), (int)handshake.size(), 0);

	if (iResult == SOCKET_ERROR) {
		std::cout << "failed sending message: " << WSAGetLastError() << "\n";
		closesocket(Server_socket);
		return 1;
	}

	std::cout << "Connected to " << ip << "\n";
	return 0;
}

void Handle_line(const std::string& line, CustomConsole::Flags& shared, crypto::RsaKeys& keys) {
	size_t sep = line.find('|');
	if (sep == std::string::npos) {
		shared.console.write("SERVER", line);
		return;
	}
	std::string source = line.substr(0, sep);
	std::string payload = line.substr(sep + 1);

	if (source == "KEY") {
		if (keys.set_peer_key_b64(payload))
			shared.console.write("CLIENT", "Messages are now RSA encrypted.");
		else
			shared.console.write("CLIENT", "Got a broken key from the server.");
		return;
	}
	if (source == "SERVER") {
		shared.console.write(source, payload);
		return;
	}

	// anything else is a peer message, so it is encrypted
	std::string text = keys.decrypt(payload);
	if (text.empty())
		text = "<could not decrypt message>";
	shared.console.write(source, text);
}

int Display (SOCKET Server_socket, CustomConsole::Flags& shared, crypto::RsaKeys& keys) {
	int iResult;

	char recvbuf[DEFAULT_BUFLEN];
	std::string pending;

	shared.console.write("CLIENT", "Listening to messages.");

	do {
		if (shared.stop)
			return 0;

		iResult = recv(Server_socket, recvbuf, DEFAULT_BUFLEN, 0);

		if (shared.stop)
			return 0;

		if (iResult > 0) {
			pending.append(recvbuf, iResult);

			// messages are newline separated, tcp can glue or split them
			size_t pos;
			while ((pos = pending.find('\n')) != std::string::npos) {
				std::string line = pending.substr(0, pos);
				pending.erase(0, pos + 1);
				if (!line.empty())
					Handle_line(line, shared, keys);
			}
		}
		else if (iResult == 0) {
			shared.stop = true;
			std::cout << "Connection closed\n";
		}
		else {
			shared.stop = true;
			std::cout << "Recv failed with error: " << WSAGetLastError() << "\n";
		}

	} while (iResult > 0);
	return 0;
}

int Input(SOCKET Server_socket, CustomConsole::Flags& shared, crypto::RsaKeys& keys) {
	int iResult = 0;
	while (true) {
		if (shared.stop) {
			break;
		}
		std::string input = shared.console.read();
		if (shared.stop) {
			break;
		}
		if (input.empty())
			continue;

		if (!keys.has_peer_key()) {
			shared.console.write("CLIENT", "Target is not connected yet, message not sent.");
			continue;
		}

		// rsa 2048 fits ~245 bytes per block, keep some margin
		if (input.size() > 200) {
			shared.console.write("CLIENT", "Message too long, cut to 200 chars.");
			input = input.substr(0, 200);
		}

		std::string enc = keys.encrypt_for_peer(input);
		if (enc.empty()) {
			shared.console.write("CLIENT", "Encryption failed, message not sent.");
			continue;
		}
		enc += "\n";

		iResult = send(Server_socket, enc.c_str(), (int)enc.size(), 0);

		if (iResult == SOCKET_ERROR) {
			shared.stop = true;

			std::cout << "send failed: " << WSAGetLastError() << "\n";
			return iResult;
		}
	}
	return 0;
}

void Separate_console(SOCKET Server_socket, std::map<std::string, std::string>& argk, crypto::RsaKeys& keys) {
	system("cls");

	CustomConsole::Flags shared(argk["name"]);

	std::thread displayThread(Display, Server_socket, std::ref(shared), std::ref(keys));
	Input(Server_socket, shared, keys);

	shared.stop = true;
	// unblocks recv so the display thread can exit
	shutdown(Server_socket, SD_BOTH);
	displayThread.join();
}

int main(int argc, char* argv[]) {
	std::map<std::string, std::string> argk = shared::Get_keyword_arguments(argc, argv);
	shared::validate_arguments(argk);

	crypto::RsaKeys keys;
	std::cout << "Generating RSA keys...\n";
	if (!keys.generate()) {
		std::cout << "RSA key generation failed\n";
		system("pause");
		return 1;
	}

	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed with error " << iResult << "\n";
		system("pause");
		return 1;
	}


	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	SOCKET Server_socket = INVALID_SOCKET;
	iResult = Connect_IP (Server_socket, argk["server"].c_str(), hints, auth_data(argk["name"], argk["target"], keys.public_key_b64()));
	if (iResult != 0) {
		WSACleanup();
		return iResult;
	}

	Separate_console(Server_socket, argk, keys);

	std::cout << "Stopping client...\n";

	// cleanup
	closesocket(Server_socket);
	WSACleanup();

	return 0;
}
