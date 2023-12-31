#include "Networking.h"
#include <vector>
#include <string>
#include "utils.h"
#include "SharedConfigs.h"
#include <iostream>
#include <map>
#include <ws2tcpip.h>
#include <algorithm>

p2p_socket_data::p2p_socket_data() {
	socket = INVALID_SOCKET;
	addr = { 0 };
	addr_len = (int)sizeof(sockaddr_in);

	target_username = "";
	username = "?";
}
void p2p_socket_data::load(SOCKET _socket, std::vector<std::string> handshake, sockaddr_in _addr, int _addr_len) {
	socket = _socket;
	addr = _addr;
	addr_len = _addr_len;

	username = handshake[1];
	target_username = handshake[2];
}
std::string p2p_socket_data::get_ip() {
	char ip[INET_ADDRSTRLEN];
	if (inet_ntop(addr.sin_family, &addr.sin_addr, ip, INET_ADDRSTRLEN) != NULL) {
		return ip;
	}
	return "";
}
std::string p2p_socket_data::reference() {
	return get_ip() + "-" + username;
}



SocketsList::SocketsList() {
	graph = std::map<std::string, std::vector<p2p_socket_data>>();
	connections = std::vector<p2p_socket_data>();
}
SocketsList::~SocketsList() { }
const std::string SocketsList::ADDING_ERRORS[] = {"ADDING OK", "Username is already taken"};

int SocketsList::Add_client(p2p_socket_data data) {
	for (int i = 0; i < connections.size(); i++) {
		if (connections[i].username == data.username) {
			return 1;
		}
	}

	graph[data.target_username].push_back(data);
	connections.push_back(data);
	return 0;
}
void SocketsList::Remove_client(std::string username) {
	for (auto e : graph[username]) {

		for (auto it = graph[e.username].begin(); it != graph[e.username].end(); ) {
			if (it->username == username) {
				it = graph[e.username].erase(it);
			}
			else {
				++it;
			}
		}
	}

	graph.erase(username);

	for (auto it = connections.begin(); it != connections.end(); ) {
		if (it->username == username) {
			it = connections.erase(it);
		}
		else {
			++it;
		}
	}
}
bool SocketsList::Target_listens(p2p_socket_data data) {
	
	for (auto it = graph[data.username].begin(); it != graph[data.username].end(); ++it) {
		if (it->username == data.target_username) {
			return true;
		}
	}
	return false;
}
p2p_socket_data SocketsList::Get_target(p2p_socket_data	data) {

	for (auto it = graph[data.username].begin(); it != graph[data.username].end(); ++it) {
		if (it->username == data.target_username) {
			return *it;
		}
	}
	throw std::invalid_argument("Exeption in Get_target:: Target is not listening!");
}

const std::string Handshake_errors[] = {"OK handshake", "Empty handshake", "Socket error during handshake", "Wrong handshake format", "Version mismatch during handshake"};
int Handshake(SOCKET ClientSocket, p2p_socket_data& result, sockaddr_in addr, int addr_len) {
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult = 0;

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::vector<std::string> splitted = split(recvbuf, iResult, '|');

		if (splitted.size() != 3) {
			return 3;
		}
		if (splitted[0] != VERSION) {
			return 4;
		}

		result.load(ClientSocket, splitted, addr, addr_len);

		return 0;
	}
	else if (iResult == 0)
		return 1;
	else if (iResult == SOCKET_ERROR) {
		return 2;
	}
}
std::string Get_IP (sockaddr_in* addr) {
	char ip[INET_ADDRSTRLEN];
	if (inet_ntop(addr->sin_family, &addr->sin_addr, ip, INET_ADDRSTRLEN) == NULL) {
		std::cerr << "Failed retrieving IP from addr\n";
		return "";
	}

	return ip;
}


int send (SOCKET s, const std::string message, int flags) {
	return send(s, message.c_str(), message.size(), flags);
}

int send_and_handle(SOCKET s, const std::string message, int flags) {
	return send_and_handle(s, message.c_str(), message.size(), flags);
}

int send_and_handle(SOCKET s, const char* message, int len, int flags) {
	int iSendResult = send(s, message, len, flags);
	if (iSendResult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << "\n";
		closesocket(s);
	}
	return iSendResult;
}