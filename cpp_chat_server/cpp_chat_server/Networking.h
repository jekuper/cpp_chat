#pragma once
#include <WinSock2.h>
#include <string>
#include <vector>
#include <map>
#include <mutex>

///<summary>holds all data about peer connection</summary>
class p2p_socket_data {
public:
	///<summary>client's socket</summary>
	SOCKET socket;
	///<summary>target's socket. Available ONLY after adding to SocketsList</summary>
	SOCKET target_socket;

	///<summary>socket address structure</summary>
	sockaddr_in addr;
	int addr_len;

	///<summary>username of the target</summary>
	std::string target_username;
	///<summary>client's username</summary>
	std::string username;

	p2p_socket_data();
	void load(SOCKET _socket, std::vector<std::string> handshake, sockaddr_in _addr, int _addr_len);
	///<summary>Returns client's IP address</summary>
	///<returns>This client's IP address</returns>
	std::string get_ip();
	///<summary>Checks if target of this client listens</summary>
	///<returns>True if listens, False if not</returns>
	bool Is_target_listening();

	///<summary>Gets a reference string for this client</summary>
	///<returns>Returns a concatenation of client's IP & username</returns>
	std::string reference();
private:
};

///<summary>holds all connections to server and their data</summary>
class SocketsList {
public:

	///<summary>List of string representation of Adding Errors</summary>
	static const std::string ADDING_ERRORS[];

	SocketsList();
	~SocketsList(); 
	SocketsList(const SocketsList& other);
	SocketsList& operator=(const SocketsList& other);

	///<summary>Adds a client to a list and graph. Calculates target_socket for each peer</summary>
	///<param name="data">The client to add</param>
	///<returns>Returns error code if adding failed. Error representation avalilable in ADDING_ERRORS</returns>
	int Add_client(p2p_socket_data data);

	///<summary>Removes a client from a list and graph. Recalculates target_socket for remaining</summary>
	///<param name="socket">the client's socket</param>
	void Remove_client(SOCKET socket);
	///<summary>Gets client's data by SOCKET</summary>
	///<param name="socket">client's socket</param>
	///<returns>Returns pointer to empty p2p_socket_data if socket doesn't exists. Otherwise returns pointer to actual data</returns>
	p2p_socket_data* Get(SOCKET socket);
private:
	///<summary>The graph of users. Element in vector means that this client listens to key client</summary>
	std::map<SOCKET, std::vector<p2p_socket_data*>> graph;
	///<summary>Tracks all connected users</summary>
	std::map<SOCKET, p2p_socket_data> connections;
	///<summary>Locks threads during adding and removing</summary>
	mutable std::mutex data_mtx;
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


/// <summary>Wraper function for sending string over socket. Closes socket on error</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">string message</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send_and_handle(SOCKET s, const std::string message, int flags);


/// <summary>Wraper function for sending string over socket. Closes socket on error</summary>
/// <param name="s">socket for sending</param>
/// <param name="message">c-style string message</param>
/// <param name="len">string length</param>
/// <param name="flags">flags</param>
/// <returns>error code</returns>
int send_and_handle(SOCKET s, const char* message, int len, int flags);