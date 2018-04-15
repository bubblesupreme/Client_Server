#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include "helper.h"
#include <iostream>
#include <WinSock2.h>
#include <winsock.h>
#define MY_PORT    666
#define SERVERADDR "127.0.0.1"

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