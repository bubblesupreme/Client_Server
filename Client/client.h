
#ifndef CLIENT_H
#define CLIENT_H

#include "../Helper/helper.h"
#include <WinSock2.h>
#include <winsock.h>

class Client
{
public:
	Client();
	~Client();
	void handle();
	void process();
	void listen();
	//int makeSock();

	//void makeConnection();

	//void conversation();

	//void listenForMessages();

	void start();

	void createAccount();

	/*void login();
	void mainMenu();*/
private:
	SOCKET my_sock;
	SOCKADDR_IN dest_addr;
	char buff[1024];
	struct sockaddr_in serverinfo;
	//int sock;
	int connection;
	char buffer[1024];
	long sendAttempt;
	long receiveAttempt;
	std::string username;
	std::string password;
	std::string firstName;
	std::string lastName;
	std::string recipient;
	bool talking;
};

#endif