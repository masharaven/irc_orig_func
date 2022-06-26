#include "../../inc/GlobalLib.hpp"
#include "../../inc/User.hpp"

User::User() { this->_flags[0] = 0; this->_flags[1] = 0; this->_flags[2] = 0; }

User::User(int fd) { this->_fd = fd; this->_flags[0] = 0; this->_flags[1] = 0; this->_flags[2] = 0; }

void	User::setFd(int fd) { this->_fd = fd; }
void	User::setNick(string nick) { _nick = nick; };
void	User::setUsername(string username) { _username = username; };
void	User::setFlags(string input) {
	if (input == "PASS"){ _flags[0] = 1; }
	if (input == "NICK"){ _flags[1] = 1; }
	if (input == "USER"){ _flags[2] = 1; }
}

string	User::getNick() { return (_nick); }
string	User::getUsername() { return (_username); }
int		User::getFd() { return(this->_fd); }
int		User::getFlags(string input) {
	if (input == "PASS"){ return (_flags[0]); }
	if (input == "NICK"){ return (_flags[1]); }
	if (input == "USER"){ return (_flags[2]); }
	if (input == "ALL"){ return getFlags("PASS") && getFlags("NICK") && getFlags("USER") ? 2 : 3; } // 2 - true / 3 - false
	return 0;
}

int 	User::settingParams(Server &server, string msg, int i, struct pollfd fds[]) {
	int checkFlags = server.getUser(i).getFlags("ALL");

	if (checkFlags == 3)	// if flags are 0
		return server.getUser(i).cmdPars(server, msg, i);

	return (0);
}

int 	User::cmdPars(Server &server, string msg, int i) {
	string firstParam;
	vector<string> params = getParams(msg, firstParam);
	static int flag = 0;
	
	if (!flag && params.size() > 0){
		if (params[0] == "LSPING"){
			string pong = "PONG " + params[1] + "\r\n";
			send(_fd, pong.c_str(), pong.length() + 1, 0);
		}
		++flag;
	}

	if (server.getUser(i).getFlags("PASS") == 0 && firstParam == "PASS") {
		server.getUser(i).validatePassword(server, msg, i);
		return (1);
	}

	return 0;
}

void	User::validatePassword(Server &server, string msg, int i) {
	string			noUseParam;
	vector<string>	params = getParams(msg, noUseParam);

	if (params.size() == 0) {
		sendError(ERR_NEEDMOREPARAMS(string("PASS")));
		return ;
	}
	if (params[0] == server.getPassword())
		server.setFlags(i, "PASS");
	else
		sendError(ERR_PASSWDMISMATCH);
}

void	User::sendError(string err) { send(_fd, err.c_str(), err.length() + 1, 0); }

User::~User() {  }