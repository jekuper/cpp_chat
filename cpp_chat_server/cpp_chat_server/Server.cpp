
#include <iostream>
#include <map>
#include "Networking.h"
#include "SharedConfigs.h"
#ifdef _WIN32
#include <Winsock2.h>
#include <thread>
#include <ws2tcpip.h>

#endif

#pragma comment(lib, "Ws2_32.lib")

SocketsList sockets_list = SocketsList();


void Messaging(SOCKET ClientSocket, sockaddr_in addr, int addr_len) {
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult = 0;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult = 0;
	p2p_socket_data new_client_data = p2p_socket_data();

	iResult = Handshake(ClientSocket, new_client_data, addr, addr_len);

	if (iResult) {
		std::cout << Handshake_errors[iResult] << "\n  WSA error:" << WSAGetLastError() << "\n";
		closesocket(ClientSocket);
		return;
	}

	std::cout << "Starting messaging with: " << new_client_data.reference() << "\n";

	iResult = sockets_list.Add_client(new_client_data);
	if (iResult == 1) {
		send(ClientSocket, SocketsList::ADDING_ERRORS[iResult], 0, nullptr);
	}
	else if (iResult != 0) {
		std::cout << "Unable to add "<< new_client_data.reference() << " to SocketsList, with error " << iResult << " - " << SocketsList::ADDING_ERRORS[iResult] << "\n";
		send(ClientSocket, SocketsList::ADDING_ERRORS[iResult], 0, nullptr);
		closesocket(ClientSocket);
		return;
	}

	p2p_socket_data* client_data = sockets_list.Get(ClientSocket);

	if (client_data->Is_target_listening()) {
		p2p_socket_data* target_data = sockets_list.Get(client_data->target_socket);

		// swap public keys so the two can encrypt for each other
		send_msg(client_data->socket, "KEY", target_data->pubkey);
		send_msg(target_data->socket, "KEY", client_data->pubkey);

		send(client_data->socket, "--Target connected.", 0, nullptr);
		send(target_data->socket, "--Target connected.", 0, nullptr);
	}

	std::string pending;
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			pending.append(recvbuf, iResult);

			// messages are newline separated, tcp can glue or split them
			size_t pos;
			while ((pos = pending.find('\n')) != std::string::npos) {
				std::string line = pending.substr(0, pos);
				pending.erase(0, pos + 1);
				if (line.empty())
					continue;

				if (client_data->Is_target_listening()) {
					iSendResult = send_and_handle(client_data->target_socket, line, 0, client_data);
				}
				else {
					iSendResult = send_and_handle(ClientSocket, "--Target is not listening.", 0, nullptr);
				}
				if (iSendResult == SOCKET_ERROR)
					break;
			}
			if (iSendResult == SOCKET_ERROR)
				break;
		}
		else if (iResult == 0)
			std::cout << "Connection closing with " << client_data->reference() << "\n";
		else
			std::cout << "Message from " << client_data->reference() << " failed with code " << WSAGetLastError() << "\n";

	} while (iResult > 0);

	if (client_data->Is_target_listening())
		send(client_data->target_socket, "--Target disconnected.", 0, nullptr);

	sockets_list.Remove_client(client_data->socket);
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
		system("pause");
		return 1;
	}

	addrinfo* result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "Getaddrinfo failed with error: " << iResult << "\n";
		system("pause");
		WSACleanup();
		return 1;
	}
	for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		char ipstr[INET_ADDRSTRLEN];
		struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;

		// Convert the IP address to a human-readable format
		if (inet_ntop(sockaddr_ipv4->sin_family, &(sockaddr_ipv4->sin_addr), ipstr, sizeof(ipstr)) != nullptr) {
			std::cout << "Listening on " << ipstr << ":" << DEFAULT_PORT << "\n";
		}
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error at socket(): " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		WSACleanup();
		system("pause");
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		system("pause");
		return 1;
	}

	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;

	while (true) {
		sockaddr_in addr = { 0 };
		int addrlen = (int)sizeof(sockaddr_in);

		ClientSocket = accept(ListenSocket, (sockaddr *) &addr, &addrlen);
		if (ClientSocket == INVALID_SOCKET) {
			std::cout << "accept failed: " << WSAGetLastError() << "\n";
			continue;
		}

		std::cout << "Established connection to " << Get_IP(&addr) << ". Waiting for handshake...\n";

		// detached, cleans up after itself
		std::thread(Messaging, ClientSocket, addr, addrlen).detach();
	}

	std::cout << "Closing server...\n";

	closesocket(ListenSocket);
	WSACleanup();

	return 0;
}
