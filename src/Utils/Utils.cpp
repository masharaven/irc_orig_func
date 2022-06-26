#include "../../inc/GlobalLib.hpp"
#include "../../inc/User.hpp"
#include "../../inc/Server.hpp"

vector<string> getParams(string msg, string &firstParam){
	vector<string> params;
	if (!msg.empty()) {
		std::istringstream tempStr(msg.c_str());
		string resStr;

		while (getline(tempStr, resStr, ' ') && resStr != " ")
			params.push_back(resStr);
		firstParam = params[0];
		params.erase(params.begin());

		for(vector<string>::iterator it = params.begin(); it != params.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\r'), (*it).end());
		for(vector<string>::iterator it = params.begin(); it != params.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\n'), (*it).end());

		firstParam.erase(std::remove(firstParam.begin(), firstParam.end(), '\r'), firstParam.end());
		firstParam.erase(std::remove(firstParam.begin(), firstParam.end(), '\n'), firstParam.end());
	}
	return params;
}

void SendMsgIrcSynt(int fd, string nickname, string username, string message){
	string msgToSend = ":" + nickname + "!" + username + "@127.0.0.1 " + message + "\r\n";
	send(fd, msgToSend.c_str(), msgToSend.length() + 1, 0);
}

void	error(string error){
	std::cout << "Error: " << error << std::endl;
	exit(EXIT_FAILURE);
}