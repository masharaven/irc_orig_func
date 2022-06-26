#ifndef USER_HPP
# define USER_HPP

#include "GlobalLib.hpp"
#include "Server.hpp"

class Server;

class User {
private:
	int		_fd;
	string	_nick;
	string	_username;

	int		_flags[3];

public:
	User();
	User(int fd);
	~User();

	void	setNick(string nick);
	void	setUsername(string username);
	void	setFd(int fd);
	void	setFlags(string input);

	int		getFd();
	int		getFlags(string input);
	string	getUsername();
	string	getNick();

	int		settingParams(Server &server, string msg, int i, struct pollfd fds[]);
	int		cmdPars(Server &server, string msg, int i);
	void	validatePassword(Server &server, string msg, int i);
	// int		signUp(Server &server, string msg, int i);
	// vector<string> getParams(string msg);
	void	sendError(string err);
};

#endif