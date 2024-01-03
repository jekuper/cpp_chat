#include "Networking.h"
#include <vector>
#include <string>
#include "SharedConfigs.h"
#include <iostream>
#include <map>
#include <ws2tcpip.h>
#include <algorithm>

p2p_socket_data::p2p_socket_data() {
	socket = INVALID_SOCKET;
	target_socket = INVALID_SOCKET;
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
bool p2p_socket_data::Is_target_listening() {
	return target_socket != INVALID_SOCKET;
}
std::string p2p_socket_data::reference() {
	return get_ip() + "-" + username;
}



SocketsList::SocketsList() {
	graph = std::map<SOCKET, std::vector<p2p_socket_data*>>();
	connections = std::map<SOCKET, p2p_socket_data>();
}
SocketsList::SocketsList(const SocketsList& other) {
	// Perform a deep copy of the data
	std::lock_guard<std::mutex> lock(other.data_mtx);
	graph = other.graph;
	connections = other.connections;
}

// Copy assignment operator
SocketsList& SocketsList::operator=(const SocketsList& other) {
	if (this != &other) { // Avoid self-assignment
		// Perform a deep copy of the data
		std::lock(data_mtx, other.data_mtx);  // Lock both mutexes to avoid deadlock
		std::lock_guard<std::mutex> lockThis(data_mtx, std::adopt_lock);
		std::lock_guard<std::mutex> lockOther(other.data_mtx, std::adopt_lock);

		graph = other.graph;
		connections = other.connections;
	}
	return *this;
}

// Destructor
SocketsList::~SocketsList() {
	// Destructor automatically releases the mutex
}
const std::string SocketsList::ADDING_ERRORS[] = {"ADDING OK", "--Target is not listening"};

int SocketsList::Add_client(p2p_socket_data data) {
	std::lock_guard<std::mutex> lock(data_mtx);

	connections[data.socket] = data;
	for (const auto& pair : connections) {
		if (pair.second.username == data.target_username &&
			pair.second.target_username == data.username && 
			pair.second.target_socket == INVALID_SOCKET) {
			graph[pair.second.socket].push_back(&data);
			graph[data.socket].push_back(&connections[pair.first]);

			connections[data.socket].target_socket = pair.second.socket;
			connections[pair.first].target_socket = data.socket;
			
			return 0;
		}
	}

	return 1;
}
void SocketsList::Remove_client(SOCKET socket) {
	std::lock_guard<std::mutex> lock(data_mtx);

	for (auto e : graph[socket]) {

		e->target_socket = INVALID_SOCKET;
		for (auto it = graph[e->socket].begin(); it != graph[e->socket].end(); ) {
			p2p_socket_data* e2 = *it;

			if (e2->socket == socket) {
				it = graph[e->socket].erase(it);
			}
			else {
				++it;
			}
		}
	}

	graph.erase(socket);
	connections.erase(socket);
}
p2p_socket_data* SocketsList::Get(SOCKET socket) {
	return &connections[socket];
}

const std::string Handshake_errors[] = {"OK handshake", "Empty handshake", "Socket error during handshake", "Wrong handshake format", "Version mismatch during handshake"};
int Handshake(SOCKET ClientSocket, p2p_socket_data& result, sockaddr_in addr, int addr_len) {
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult = 0;

	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::vector<std::string> splitted = shared::split(recvbuf, iResult, '|');

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


int send (SOCKET s, const std::string message, int flags, p2p_socket_data* data) {
	std::string source = Get_source(data);

	std::string result;
	result = source + "|" + message;

	return send(s, result.c_str(), result.size(), flags);
}

int send_and_handle(SOCKET s, const std::string message, int flags, p2p_socket_data* data) {
	int iSendResult = send(s, message, flags, data);

	if (iSendResult == SOCKET_ERROR) {
		std::cout << "Send failed: " << WSAGetLastError() << "\n";
		closesocket(s);
	}
	return iSendResult;
}

std::string Get_source(p2p_socket_data* data) {
	if (data->socket == INVALID_SOCKET)
		return "SERVER";
	return data->username;
}