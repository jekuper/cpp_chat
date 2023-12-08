
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

using namespace std;

class SocketsList
{
public:
	SocketsList() {
		connections = map<string, p2p_socket_data>();
	}
	
	~SocketsList() {

	}

	void Add (string ip, p2p_socket_data data) {
		connections[ip] = data;
	}
	void Remove (string ip) {
		connections.erase(ip);
	}
	p2p_socket_data Get (string ip) {
		return connections[ip];
	}
	bool Exists(string ip) {
		return connections.find(ip) != connections.end();
	}

	map<string, p2p_socket_data> connections;

private:

};
SocketsList sockets_list = SocketsList();


void Messaging(SOCKET ClientSocket, string client_ip) {
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult = 0;
	p2p_socket_data new_data = p2p_socket_data();

	iResult = Handshake(ClientSocket, new_data);

	if (iResult) {
		cout << Handshake_errors[iResult] << endl;
		closesocket(ClientSocket);
		return;
	}

	sockets_list.Add(client_ip, new_data);

	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			if (sockets_list.Exists(new_data.target_ip)) {
				send_and_handle(sockets_list.Get(new_data.target_ip).socket, recvbuf, iResult, 0);
			}
			else {
				iSendResult = send_and_handle(ClientSocket, "--Target is offline.\n", 0);
			}
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			return;
		}

	} while (iResult > 0);

	sockets_list.Remove(client_ip);
	closesocket(ClientSocket);
	return;
}

int main()
{
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

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed %d\n", iResult);
		WSACleanup();
		return 1;
	}

	SOCKET ListenSocket = INVALID_SOCKET;

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;


	cout << "Starting accepting connections:\n";
	//TODO: implement deletion from threads array
	vector<thread> threads;
	while (true) {
		sockaddr_in addr = { 0 };
		int addrlen = (int)sizeof(sockaddr_in);
		ClientSocket = accept(ListenSocket, (sockaddr *) &addr, &addrlen);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed: %d\n", WSAGetLastError());
			continue;
		}

		char ip[INET_ADDRSTRLEN];
		if (inet_ntop(addr.sin_family, &addr.sin_addr, ip, INET_ADDRSTRLEN) != NULL) {
			cout << ip << endl;
		}

		threads.push_back(thread(Messaging, ClientSocket, (string)ip));
	}

	cout << "Finishing program...\n";

	closesocket(ListenSocket);
	WSACleanup();

	return 0;
}