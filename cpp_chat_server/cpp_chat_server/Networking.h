#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

///<summary>holds all data about peer connection</summary>
class p2p_socket_data {
public:
	SOCKET socket;

	sockaddr_in addr;
	int addr_len;

	string target_ip;
	string username;

	p2p_socket_data();
	void load(SOCKET _socket, vector<string> handshake, sockaddr_in _addr, int _addr_len);
	string get_ip();
private:
};

///<summary>holds all connections to server and their data</summary>
class SocketsList {
public:
	map<string, vector<p2p_socket_data>> connections;

	SocketsList();
	~SocketsList();
	void Add_client(p2p_socket_data data);
	void Remove_client(string ip);
	bool Target_listens(p2p_socket_data data);
	p2p_socket_data Get_target(p2p_socket_data data);
private:
};

/// <summary> holds associated string description for each handshake error code</summary>
extern const string Handshake_errors[];

/// <summary>Handshake used to check version compatablilty and receive nicknames</summary>
/// <param name="ClientSocket">the socket associated with client</param>
/// <param name="result">variable to put result in</param>
/// <returns>error code</returns>
int Handshake(SOCKET ClientSocket, p2p_socket_data& result, sockaddr_in addr, int addr_len);

/// <summary>Wraper function for sending string over socket</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">string message</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send(SOCKET s, const string message, int flags);


/// <summary>Wraper function for sending string over socket</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">string message</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send_and_handle(SOCKET s, const string message, int flags);


/// <summary>Wraper function for sending string over socket</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">c-style string message</param>
/// <param name="len">string length</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send_and_handle(SOCKET s, const char* message, int len, int flags);