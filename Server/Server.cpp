#include "Server.h"

#include <iostream>
#include <fstream> 
#include <sys/types.h>
#include <thread>
#include <sys/stat.h>
#include <WS2tcpip.h>

Server::Server() {
	nClients = 0;
	std::ifstream fin;
	fin.open("C:\\Users\\home\\Documents\\\Visual Studio 2017\\Projects\\Client_Server\\users.txt");
	if (!fin.is_open()) 
		std::cout << "Файл не может быть открыт!\n";
	else
	{
		while (!fin.eof())
		{
			User user("","","","");
			fin >> user.userName;
			fin >> user.password;
			fin >> user.firstName;
			fin >> user.lastName;
			users.push_back(user);
		}
		fin.close();
	}
}

Server::~Server() {
}


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
		DWORD thID;
		if (nClients < SOMAXCONN)
		{
			std::thread thr(&Server::WorkWithClient, this, client_socket);
			thr.detach();
		}
		else
		{
			std::cout << "The max amount of connections have been established, new incoming connection failed." << std::endl;
		}
	}
	return;
}

DWORD WINAPI Server::WorkWithClient( long client_socket)
{
	char buff[1024] ;

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

		if (responseCode == CONNECT_SUCCESS)
		{
			std::string mess="Successful connect!\0";
			send(client_socket, mess.c_str(), mess.length(), 0);
		}
		else if (responseCode == ERROR_DOES_NOT_CONNECT)
		{
			std::string mess = "User doesn't exist!\0";
			send(client_socket, mess.c_str(), mess.length(), 0);
		}
		else if (responseCode == ERROR_SEND_MESSAGE)
		{
			std::string mess = "User for conversation isn't selected\0";
			send(client_socket, mess.c_str(), mess.length(), 0);
		}
		
		if (responseCode != 0) {
			send(client_socket, &responseCode, 1, 0);
		}
		Sleep(650);
	}
	std::cout << "-disconnect" << std::endl;

		closesocket(client_socket);
	return 0;
}

char Server::interpret(std::string str, long connection) {
	std::vector<std::string> data = parseString(str);
	std::string function = data.at(0);

	if (function == "create") {
		return createAccount(data);
	}
	else if (function == "connect") {
		return initConnection(data);
	}
	else if (function == "login") {
		return login(data, connection);
	}
	else if (function == "send") {
		return sendMessage(data);
	}
	else if (function == "confirm") {
		return checkUserExists(data);
	}
	else if (function == "quit") {
		return endConversation(data);
	}

	return 0;
}


char Server::endConversation(std::vector<std::string> data) {
	std::string closer = data.at(1);
	std::string other = data.at(2);

	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).userName == other) {
			connections.at(i).currentConversation.clear();
			char resp = CONVERSATION_ENDED;
			send(connections.at(i).id, &resp, 1, 0);
			Sleep(650);
		}

		if (connections.at(i).userName == closer) {
			connections.at(i).currentConversation.clear();
		}
	}

	return CONVERSATION_ENDED;
}


char Server::createAccount(std::vector<std::string> data) {
	std::string userName = data.at(1);
	std::string password = data.at(2);
	std::string firstName = data.at(3);
	std::string lastName = data.at(4);

	users.push_back(User(userName, password, firstName, lastName));

	std::ofstream fout;
	fout.open("C:\\Users\\home\\Documents\\\Visual Studio 2017\\Projects\\Client_Server\\users.txt", std::ios_base::app);
	if (!fout.is_open())
		std::cout << "Файл не может быть открыт!\n";
	else
	{
		fout << userName;
		fout << "\t";
		fout << password;
		fout << "\t";
		fout << firstName;
		fout << "\t";
		fout << lastName;
		fout << "\n";
		fout.close();
	}

	return CREATE_ACCOUNT_SUCCESS;
}
    

char Server::login(std::vector<std::string> data, long connection) {
	
	for (int i=0; i<users.size();i++)
	{
		if (users.at(i).userName == data.at(1))
		{
			if (users.at(i).password == data.at(2))
			{
				connections.push_back(Connection(connection, data.at(1)));
				for (auto msg : users.at(i).stackMess)
				{
					send(connection, msg.c_str(), msg.length(), 0);
					Sleep(650);
				}
				send(connection, "end",sizeof("end"), 0);
				Sleep(650);
				users.at(i).stackMess.clear();
				return PASSWORD_CORRECT;
			}
			else {
				return ERROR_INCORRECT_PASSWORD;
			}
		}
	}

	return ERROR_ACCOUNT_DOES_NOT_EXIST;
}


char Server::checkUserExists(std::vector<std::string> data) {
	std::string user = data.at(1);
	std::string from = data.at(2);

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


char Server::sendMessage(std::vector<std::string> data) {
	std::string sentFrom = data.at(1);
	std::string msg = data.at(1)+": "+ data.at(2);
	bool success = false;

	const char *message = msg.c_str();

	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).userName == sentFrom)
		{
			for (int j = 0; j < connections.size(); j++)
			{
				if (connections.at(j).userName == connections.at(i).currentConversation) {
					size_t bytesSent;
					int len = strlen(message);
					bytesSent = send(connections.at(j).id, message, len, 0);
					Sleep(650);
					return SEND_MESSAGE_SUCCESS;
				}
			}
			for (int j = 0; j < users.size(); j++)
			{
				if (users.at(j).userName == connections.at(i).currentConversation)
				{
					users.at(j).stackMess.push_back(msg);
					return SEND_MESSAGE_SUCCESS;
				}
			}
		}
	}
	return ERROR_SEND_MESSAGE;
}



char Server::initConnection(std::vector<std::string> data) {
	std::string connectFrom = data.at(1);
	std::string connectTo = data.at(2);
	for (int i = 0; i < connections.size(); i++) {
		if (connections.at(i).userName == connectFrom) {
			connections.at(i).currentConversation.clear();
			for (int j = 0; j < users.size(); j++)
			{
				if (users[j].userName == connectTo)
				{
					connections.at(i).currentConversation = users[j].userName;
					return CONNECT_SUCCESS;
				}

			}
		}
	}

	return ERROR_DOES_NOT_CONNECT;
}




int main() {
	Server server;
	server.startServer();
	return 0;
}