#ifndef SERVER_H
#define SERVER_H

#include "../Helper/helper.h"
#include <WinSock2.h>
#include <winsock.h>


struct Connection {
	Connection(long id, std::string userName)
		:id(id), userName(userName)
	{}
	long id;  
	std::string userName; 
	std::string currentConversation = ""; 
};

struct User {
	User(std::string userName, std::string  password, std::string firstName, std::string lastName)
		:userName(userName), password(password), firstName(firstName), lastName(lastName)
	{}
	std::string userName;
	std::string password;
	std::string firstName;
	std::string lastName;
	std::vector<std::string> stackMess;
};

class Server {
public:
	Server();
	virtual ~Server();

	void startServer();
	void closeServer();

private:
	void handle();
	char interpret(std::string data, long connection);
	char sendMessage(std::vector<std::string> data);
	char initConnection(std::vector<std::string> data);
	char login(std::vector<std::string> data, long connection);
	char createAccount(std::vector<std::string> data);
	char checkUserExists(std::vector<std::string> data);
	char endConversation(std::vector<std::string> data);

	int sock;  
	int numCurrentConnections;  
	DWORD WINAPI WorkWithClient(long client_socket);
	std::vector<Connection> connections;
	std::vector<User> users;
	SOCKET mysocket;
	WSAData wData;
	int nClients;
};


#endif