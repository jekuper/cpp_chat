#include "Networking.h"
#include <vector>
#include <string>
#include "utils.h"
#include "SharedConfigs.h"
#include <iostream>

using namespace std;

p2p_socket_data::p2p_socket_data() {
	socket = INVALID_SOCKET;
	is_host = true;
	target_ip = "";
	username = "?";
}
void p2p_socket_data::load_handshake(SOCKET _socket, vector<string> handshake) {
	socket = _socket;
	username = handshake[1];
	target_ip = handshake[2];
	is_host = (handshake[2] == "");
}

const string Handshake_errors[] = {"OK", "Empty handshake", "Unknown error", "Wrong handshake format", "Different version"};
int Handshake(SOCKET ClientSocket, p2p_socket_data& result) {
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult = 0;

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		vector<string> splitted = split(recvbuf, iResult, '|');

		if (splitted.size() != 3) {
			return 3;
		}
		if (splitted[0] != VERSION) {
			return 4;
		}

		result.load_handshake(ClientSocket, splitted);

		return 0;
	}
	else if (iResult == 0)
		return 1;
	else {
		return 2;
	}
}

int send (SOCKET s, const string message, int flags) {
	return send(s, message.c_str(), message.size(), flags);
}
int send_and_handle(SOCKET s, const string message, int flags) {
	return send_and_handle(s, message.c_str(), message.size(), flags);
}

int send_and_handle(SOCKET s, const char* message, int len, int flags) {
	int iSendResult = send(s, message, len, flags);
	if (iSendResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(s);
	}
	return iSendResult;
}