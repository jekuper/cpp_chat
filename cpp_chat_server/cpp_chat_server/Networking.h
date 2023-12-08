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

/// <summary> holds associated string description for each handshake error code</summary>
extern const string Handshake_errors[];

/// <summary>used to check version compatablilty and receive nicknames</summary>
/// <param name="ClientSocket">the socket associated with client</param>
/// <param name="result">variable to put result in</param>
/// <returns>error code</returns>
int Handshake(SOCKET ClientSocket, p2p_socket_data& result);

int send(SOCKET s, const string message, int flags);
int send_and_handle(SOCKET s, const string message, int flags);
int send_and_handle(SOCKET s, const char* message, int len, int flags);