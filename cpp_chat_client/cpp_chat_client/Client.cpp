#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SharedConfigs.h"

#pragma comment(lib, "Ws2_32.lib")

///<summary>Wrapper over all client authentication data</summary>
class auth_data
{
public:
	///<summary>Client nickname</summary>
	std::string name;

	///<summary>IP of client to which you want to connect</summary>
	std::string target_ip;

	auth_data(std::string _name, std::string _target_ip) {
		name = _name;
		target_ip = _target_ip;
	}
};

///<summary>Generates handshake string</summary>
///<param name="data">client authentication data</param>
///<returns>string handshake</returns>
const std::string Generate_handshake(auth_data data) {
	const std::string res = (VERSION + "|" + data.name + "|" + data.target_ip);
	return res;
}


///<summary>Connects to socket and performs handshake</summary>
///<param name="Server_socket">variable where new socket will be placed</param>
///<param name="ip">ip to connect to</param>
///<param name="hints">connection configuration</param>
///<param name="data">client authentication data</param>
///<returns>Error code or 0 if no error</returns>
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
	std::cout << "sending handshake: \n  " << handshake << "\n";
	iResult = send(Server_socket, handshake.c_str(), (int)strlen(handshake.c_str()), 0);

	if (iResult == SOCKET_ERROR) {
		std::cout << "failed sending message: " << WSAGetLastError() << "\n";
		closesocket(Server_socket);
		return 1;
	}

	std::cout << "Connected to " << ip << "\n";
	return 0;
}

int main(int argc, char* argv[]) {
	std::map<std::string, std::string> argk = shared::Get_keyword_arguments(argc, argv);
	if (!shared::validate_arguments(argk)) {
		return 1;
	}

	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}


	addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	SOCKET Server_socket = INVALID_SOCKET;
	iResult = Connect_IP (Server_socket, argk["server"].c_str(), hints, auth_data(argk["name"], argk["target"]));
	if (iResult != 0) {
		WSACleanup();
		return iResult;
	}


	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	do {

		iResult = recv(Server_socket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::cout << "Bytes received: " << iResult << "\n";
			std::cout << "Message: " << std::string(recvbuf, iResult) << "\n";
		}
		else if (iResult == 0)
			std::cout << "Connection closed\n";
		else
			std::cout << "Recv failed with error: " << WSAGetLastError() << "\n";

	} while (iResult > 0);

	std::cout << "finishing...\n";

	// cleanup
	closesocket(Server_socket);
	WSACleanup();

	return 0;
}