#include "Server.h"

Server::Server() {
	nClients = 0;
}

Server::~Server() {
}

std::vector<SOCKET> clients;

DWORD WINAPI CheckCommonBuffer(LPVOID client_socket);
void hh();
int bytes_recv;
char* cbuff = (char*)"  ";

void Server::startServer() {
	if (WSAStartup(MAKEWORD(2, 2), &wData) == 0)
	{
		std::cout<<"WSA Startup succes"<<std::endl;
	}
	else
	{
		std::cout << WSAGetLastError() << std::endl;
		return;
	}

	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		std::cout << "Error socket\n"<< WSAGetLastError() << std::endl;
		WSACleanup();
		return;
	}


	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	local_addr.sin_addr.s_addr = 0;



	if (bind(mysocket, (sockaddr *)&local_addr,sizeof(local_addr)))
	{
		std::cout << "Error bind \n"<<WSAGetLastError() << std::endl;
		closesocket(mysocket);  
		WSACleanup();
		return ;
	}
	
	if (listen(mysocket, SOMAXCONN)== SOCKET_ERROR)
	{
		std::cout << "Error listen \n"<< WSAGetLastError() << std::endl;
		closesocket(mysocket);
		WSACleanup();
		return ;
	}

	std::cout << "Waiting for connection" << std::endl;

	handle();
}

void Server::closeServer() {
	closesocket(mysocket);
	WSACleanup();
	std::cout << "Server was stoped. You can close app" << std::endl;
}


void Server::handle() {
	long client_socket;
	sockaddr_in client_addr;
	int client_addr_size = sizeof(client_addr);
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nClients++;     
		HOSTENT *hst;
		hst = gethostbyaddr((char *)
			&client_addr.sin_addr.s_addr, 4, AF_INET);

		std::cout << "+"<< "new connect!\n"<<inet_ntoa(client_addr.sin_addr) << std::endl;
		if (nClients)
		{
			std::cout << nClients << " users on-line"<< std::endl;
		}
		else
		{
			std::cout << "No User on line" << std::endl;
		}

		//for (auto client : clients)
		//{
		//	send(client, "New member in our chat!))\r\n", sizeof("New member in our chat!))\r\n"), 0);
		//}

		DWORD thID;
		//CreateThread(NULL, NULL, &Server::WorkWithClient,&client_socket, NULL, &thID);
		if (nClients < SOMAXCONN)
		{
			std::thread thr(&Server::WorkWithClient, this, client_socket);
			thr.detach();
		}
		else
		{
			std::cout << "The max amount of connections have been established, new incoming connection failed." << std::endl;
		}
		//CreateThread(NULL, NULL, CheckCommonBuffer, &client_socket, NULL, &thID);
		//clients.push_back(client_socket);
	}
	return;
}

DWORD WINAPI Server::WorkWithClient( long client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];
	char buff[1024] ;

	//send(my_sock, "Hello, new member! This's our chat!))\r\n", sizeof("Hello, new member! This's our chat!))\r\n"), 0);

	while (true) {
		size_t bytesReceived;
		char buffer[1024];
		bytesReceived = recv(client_socket, buffer, 1024, 0);

		if (bytesReceived < 0) {
			std::cout << "Error receiving data from client!" << std::endl;
			break;
		}
		else if (bytesReceived == 0) {
			break;
		}

		buffer[bytesReceived] = '\0';

		char responseCode = interpret(buffer, client_socket);

		if (responseCode != 0) {
			send(client_socket, &responseCode, 1, 0);
		}
	}


	
	while (bytes_recv = recv(client_socket, buff, sizeof(buff), 0))
	{
		cbuff = buff;
		//send(my_sock, &buff[0], bytes_recv, 0);


		std::cout << buff << std::endl;
	}
	
	std::cout << "-disconnect" << std::endl;

		closesocket(client_socket);
	return 0;
}


DWORD WINAPI CheckCommonBuffer(LPVOID client_socket)
{

	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];

	while (true)
	{
		if (cbuff[0] != 9 && bytes_recv>1)

		{
			//send(my_sock, buf, sizeof(buf), 0);
			hh();
			cbuff[0] = 9;
			//Sleep(2000);
		}
	}
}

void hh()
{
	for (auto cl : clients)
	{
		send(cl, cbuff, 1024, 0);
	}
}



/**
* Server.cpp
*
* By Ryan Wise
* March 11, 2015
*
* Implementation of a server that handles a simple chat client by sending messages
* between two clients, storing and retrieving user accounts, and handling authentication
* of user accounts.
*/

#include "helper.h"
#include <iostream>
#include <fstream>
#include <sys/types.h>

#include <sys/stat.h>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>


using namespace std;





char Server::interpret(string str, long connection) {
	vector<string> data = parseString(str);
	string function = data.at(0);

	if (function == "create") {
		return createAccount(data);
	}
	else if (function == "login") {
		return login(data, connection);
	}
	else if (function == "send") {
		sendMessage(data);
	}
	else if (function == "confirm") {
		return checkUserExists(data);
	}
	else if (function == "quit") {
		return endConversation(data);
	}

	return 0;
}


char Server::endConversation(vector<string> data) {
	string closer = data.at(1);
	string other = data.at(2);

	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).userName == other) {
			connections.at(i).currentConversation.clear();
			char resp = CONVERSATION_ENDED;
			send(connections.at(i).id, &resp, 1, 0);
		}

		if (connections.at(i).userName == closer) {
			connections.at(i).currentConversation.clear();
		}
	}

	return CONVERSATION_ENDED;
}


char Server::createAccount(vector<string> data) {
	std::string userName = data.at(1);
	std::string password = data.at(2);
	std::string firstName = data.at(3);
	std::string lastName = data.at(4);

	users.push_back(User(userName, password, firstName, lastName));

	return CREATE_ACCOUNT_SUCCESS;
}
    

char Server::login(vector<string> data, long connection) {
	
	for (auto user : users)
	{
		if (user.userName == data.at(1))
		{
			if (user.password == data.at(2))
			{
				return PASSWORD_CORRECT;
			}
			else {
				return ERROR_INCORRECT_PASSWORD;
			}
		}
	}

	return ERROR_ACCOUNT_DOES_NOT_EXIST;
}


char Server::checkUserExists(vector<string> data) {
	string user = data.at(1);
	string from = data.at(2);

	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).userName == user) {
			for (int k = 0; k < connections.size(); k++) {
				if (connections.at(k).userName == from) {
					connections.at(k).currentConversation = user;
					break;
				}
			}

			connections.at(i).currentConversation = from;

			return USER_EXISTS;
		}
	}

	return ERROR_USER_DOES_NOT_EXIST;
}


void Server::sendMessage(vector<string> data) {
	string sentFrom = data.at(1);
	string sendTo = data.at(2);
	string msg = data.at(3);
	bool success = false;

	const char *message = msg.c_str();

	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).userName == sendTo && connections.at(i).currentConversation.length() != 0) {
			size_t bytesSent;
			int len = strlen(message);
			bytesSent = send(connections.at(i).id, message, len, 0);
		}
	}
}

void Server::mainLoop(long connection) {
	while (true) {
		size_t bytesReceived;
		char buffer[1024];
		bytesReceived = recv(connection, buffer, 1024, 0);

		if (bytesReceived < 0) {
			cout << "Error receiving data from client!" << endl;
			break;
		}
		else if (bytesReceived == 0) {
			break;
		}

		buffer[bytesReceived] = '\0';

		char responseCode = interpret(buffer, connection);

		if (responseCode != 0) {
			send(connection, &responseCode, 1, 0);
		}
	}

	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).id == connection) {
			connections.erase(connections.begin() + i);
			break;
		}
	}

	closesocket(connection);
}


void Server::initConnection(long connection) {
	if (numCurrentConnections == MAX_CONNECTIONS) {
		cout << "The max amount of connections have been established, new incoming connection failed." << endl;
		return;
	}

	thread thr(&Server::mainLoop, this, connection);
	thr.detach();
}

//
//void Server::start() {
//	sock = socket(serverInfoList->ai_family, serverInfoList->ai_socktype, serverInfoList->ai_protocol);
//
//	int yes = 1;
//	char* b="";
//	itoa(yes, b, 10);
//	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, b, sizeof(int));
//	bind(sock, serverInfoList->ai_addr, serverInfoList->ai_addrlen);
//
//	listen(sock, 5);
//	waitForConnection();
//}


//
//void Server::waitForConnection() {
//	long connection;
//	struct sockaddr_storage clientAddr;
//	int clientAddrSize = sizeof(clientAddr);
//	cout << "Chat server initialized." << endl;
//
//	while (true) {
//		connection = accept(sock, (struct sockaddr *) &clientAddr, &clientAddrSize);
//
//		if (connection == -1)
//			cout << "Error establishing connection with a client!" << endl;
//		else
//			initConnection(connection);
//	}
//}



int main() {
	Server server;
	server.startServer();
	return 0;
}