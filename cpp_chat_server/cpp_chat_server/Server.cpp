
#include <iostream>
#include <map>
#include "utils.h"
#include "Networking.h"
#include "SharedConfigs.h"
#ifdef _WIN32
#include <Winsock2.h>
#include <thread>
#include <vector>
#include <ws2tcpip.h>

#endif

#pragma comment(lib, "Ws2_32.lib")

SocketsList sockets_list = SocketsList();


void Messaging(SOCKET ClientSocket, sockaddr_in addr, int addr_len) {
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult = 0;
	p2p_socket_data client_data = p2p_socket_data();

	iResult = Handshake(ClientSocket, client_data, addr, addr_len);

	if (iResult) {
		std::cout << Handshake_errors[iResult] << "\n  WSA error:" << WSAGetLastError() << "\n";
		closesocket(ClientSocket);
		return;
	}

	std::cout << "Starting messaging with: " << client_data.get_ip() << "\n";

	sockets_list.Add_client(client_data);

	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			if (sockets_list.Target_listens(client_data)) {
				send_and_handle(sockets_list.Get_target(client_data).socket, recvbuf, iResult, 0);
			}
			else {
				iSendResult = send_and_handle(ClientSocket, "--Target is offline.\n", 0);
			}
		}
		else if (iResult == 0)
			std::cout << "Connection closing...\n";
		else {
			std::cout << "recv failed: " << WSAGetLastError() << "\n";
			closesocket(ClientSocket);
			return;
		}

	} while (iResult > 0);

	sockets_list.Remove_client(client_data.get_ip());
	closesocket(ClientSocket);
	return;
}

int main()
{
	WSADATA wsaData;

	int iResult;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << "\n";
		return 1;
	}

	addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "Getaddrinfo failed with error: " << iResult;
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;


	std::cout << "Starting accepting connections:\n";
	//TODO: implement deletion from threads array
	std::vector<std::thread> threads;
	while (true) {
		sockaddr_in addr = { 0 };
		int addrlen = (int)sizeof(sockaddr_in);

		ClientSocket = accept(ListenSocket, (sockaddr *) &addr, &addrlen);
		if (ClientSocket == INVALID_SOCKET) {
			std::cout << "accept failed: " << WSAGetLastError() << "\n";
			continue;
		}

		threads.push_back(std::thread(Messaging, ClientSocket, addr, addrlen));
	}

	std::cout << "Finishing program...\n";

	closesocket(ListenSocket);
	WSACleanup();

	return 0;
}