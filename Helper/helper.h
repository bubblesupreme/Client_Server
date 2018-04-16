#ifndef HELPER_H
#define HELPER_H

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#include <cstring>
#include <vector>
#include <string>

// Response codes sent back to the client from the server

#define ERROR_ACCOUNT_DOES_NOT_EXIST 1
#define ERROR_INCORRECT_PASSWORD 2
#define ERROR_USER_DOES_NOT_EXIST 3
#define ERROR_DOES_NOT_CONNECT 9
#define ERROR_SEND_MESSAGE 11

#define PASSWORD_CORRECT 4
#define CREATE_ACCOUNT_SUCCESS 5
#define USER_EXISTS 6
#define CONVERSATION_ENDED 7
#define CONNECT_SUCCESS 8
#define SEND_MESSAGE_SUCCESS 10

#define MY_PORT    666
#define SERVERADDR "127.0.0.1"

std::string convertToString(std::vector<std::string> data);

std::vector<std::string> parseString(std::string data);

std::string client_createAccount(std::string userName, std::string password, std::string firstName, std::string lastName);

std::string client_login(std::string userName, std::string password);

std::string client_sendMessage(std::string from, std::string message);

std::string client_confirmUser(std::string confirm, std::string from);

std::string client_quitConversation(std::string from);

std::string client_connect(std::string from, std::string to);

#endif HELPER_H