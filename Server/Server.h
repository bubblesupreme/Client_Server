#ifndef SERVER_H
#define SERVER_H

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <iostream>
#include <WinSock2.h>
#include <winsock.h>
#include <vector>
#define MY_PORT    666
#define SERVERADDR "127.0.0.1"
//
//class Server
//{
//public:
//	Server();
//	~Server();
//	void startServer();
//	void closeServer();
//	void handle();
//	unsigned short port;
//private:
//	SOCKET mysocket;
//	WSAData wData;
//	int nClients;
//	//std::vector<SOCKET>clients;
//};


#include <iostream>
#include <thread>
#include <vector>

#define MAX_CONNECTIONS 10 // The max amount of connections that can be made

struct Connection {
	long id;  // Connection ID of this connection
	std::string userName; // The user name of the user on this connection.
	std::string currentConversation = ""; // The user name of the person this client is talking to.
};

struct User {
	User(std::string userName, std::string  password, std::string firstName, std::string lastName)
		:userName(userName), password(password), firstName(firstName), lastName(lastName)
	{}
	std::string userName;
	std::string password;
	std::string firstName;
	std::string lastName;

};

class Server {
public:
	Server();
	virtual ~Server();

	void startServer();
	void closeServer();
	void handle();

	void mainLoop(long connection);

	void initConnection(long connection);

	char interpret(std::string data, long connection);

	void sendMessage(std::vector<std::string> data);

	char login(std::vector<std::string> data, long connection);

	char createAccount(std::vector<std::string> data);

	char checkUserExists(std::vector<std::string> data);

	char endConversation(std::vector<std::string> data);

private:
	const char* PORT = "8000";   // The port the server will run on
	struct addrinfo serverInfo;  // Server socket info
	struct addrinfo* serverInfoList;
	int sock;  // ID of the socket used for sending and receiving data on the socket
	int numCurrentConnections;  // Number of clients currently connected.
	DWORD WINAPI WorkWithClient(long client_socket);
	std::vector<Connection> connections; // Vector of connections made to the server
	std::vector<User> users;
	SOCKET mysocket;
	WSAData wData;
	int nClients;
};


#endif