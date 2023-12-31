#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>
#include <map>

///<summary>holds all data about peer connection</summary>
class p2p_socket_data {
public:
	///<summary>client's socket</summary>
	SOCKET socket;

	///<summary>socket address structure</summary>
	sockaddr_in addr;
	int addr_len;

	///<summary>username of the target username</summary>
	std::string target_username;
	///<summary>client's username</summary>
	std::string username;

	p2p_socket_data();
	void load(SOCKET _socket, std::vector<std::string> handshake, sockaddr_in _addr, int _addr_len);
	std::string get_ip();

	///<summary>get a reference string for this client</summary>
	///<returns>Returns a concatenation of client's IP & username</returns>
	std::string reference();
private:
};

///<summary>holds all connections to server and their data</summary>
class SocketsList {
public:
	///<summary>the graph of users. Element in vector means that this client listens to key client</summary>
	std::map<std::string, std::vector<p2p_socket_data>> graph;
	///<summary>hosts all connected users</summary>
	std::vector<p2p_socket_data> connections;

	///<summary>List of string representation of Adding Errors</summary>
	static const std::string ADDING_ERRORS[];

	SocketsList();
	~SocketsList();
	///<summary>Adds a client to a list and graph</summary>
	///<param name="data">the client to add</param>
	///<returns>Returns error code if adding failed</returns>
	int Add_client(p2p_socket_data data);

	///<summary>Removes a client from a list and graph</summary>
	///<param name="username">the client's username</param>
	void Remove_client(std::string username);
	///<summary>Checks if target of client listens to client</summary>
	///<returns>True if listens, False if not</returns>
	bool Target_listens(p2p_socket_data data);
	///<summary>Gets target of client</summary>
	///<returns>Target client</returns>
	p2p_socket_data Get_target(p2p_socket_data data);
private:
};

/// <summary> holds associated string description for each handshake error code</summary>
extern const std::string Handshake_errors[];

/// <summary>Handshake used to check version compatablilty and receive nicknames</summary>
/// <param name="ClientSocket">the socket associated with client</param>
/// <param name="result">variable to put result in</param>
/// <returns>error code</returns>
int Handshake(SOCKET ClientSocket, p2p_socket_data& result, sockaddr_in addr, int addr_len);

/// <summary>Converts sockaddr to readable IP</summary>
/// <param name="addr">the socket address structure</param>
/// <returns>converted IP. Ex. 127.0.0.1</returns>
std::string Get_IP(sockaddr_in* addr);

/// <summary>Wraper function for sending string over socket</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">string message</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send(SOCKET s, const std::string message, int flags);


/// <summary>Wraper function for sending string over socket</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">string message</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send_and_handle(SOCKET s, const std::string message, int flags);


/// <summary>Wraper function for sending string over socket</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">c-style string message</param>
/// <param name="len">string length</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send_and_handle(SOCKET s, const char* message, int len, int flags);