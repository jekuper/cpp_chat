#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>
using namespace std;

class p2p_socket_data {
public:
	SOCKET socket;
	bool is_host;
	string target_ip;
	string username;

	p2p_socket_data();
	void load_handshake(SOCKET _socket, vector<string> handshake);
private:
};

extern const string Handshake_errors[];
int Handshake(SOCKET ClientSocket, p2p_socket_data& result);