#include "helper.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>

using namespace std;


string convertToString(vector<string> data) {
	string str;

	for (int i = 0; i < data.size() - 1; i++) {
		str += data.at(i) + "|";
	}

	str += data.at(data.size() - 1);
	return str;
}


vector<string> parseString(string data) {
	istringstream stream(data);
	string token;
	vector<string> vec;

	while (getline(stream, token, '|')) {
		vec.push_back(token);
	}

	return vec;
}


string client_createAccount(string userName, string password, string firstName, string lastName) {
	vector<string> data;
	data.push_back("create");
	data.push_back(userName);
	data.push_back(password);
	data.push_back(firstName);
	data.push_back(lastName);

	return convertToString(data);
}


string client_login(string userName, string password) {
	vector<string> data;
	data.push_back("login");
	data.push_back(userName);
	data.push_back(password);

	return convertToString(data);
}


string client_sendMessage(string from, string message) {
	vector<string> data;
	data.push_back("send");
	data.push_back(from);
	data.push_back(message);

	return convertToString(data);
}


string client_confirmUser(string confirm, string from) {
	vector<string> data;
	data.push_back("confirm");
	data.push_back(confirm);
	data.push_back(from);

	return convertToString(data);
}


string client_quitConversation(string from) {
	vector<string> data;
	data.push_back("quit");
	data.push_back(from);

	return convertToString(data);
}

std::string client_connect(std::string from, std::string to)
{
	vector<string> data;
	data.push_back("connect");
	data.push_back(from);
	data.push_back(to);

	return convertToString(data);
}