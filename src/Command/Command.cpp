#include "Command.hpp"

Command::Command(){}

Command::Command(string msg, int fd, string nick, vector<User> &users): _msg(msg), _fd(fd), _nick(nick), _users(users) {
    if(!msg.empty()){
        std::istringstream stringToSplit(msg.c_str());
        string stringSplitted;

        while (getline(stringToSplit, stringSplitted, ' ' ) && stringSplitted != " ")
			_arguments.push_back(stringSplitted);
		_cmd = _arguments[0];
		_cmd.erase(std::remove(_cmd.begin(), _cmd.end(), '\r'), _cmd.end());
		_cmd.erase(std::remove(_cmd.begin(), _cmd.end(), '\n'), _cmd.end());
		_arguments.erase(_arguments.begin());

		for (vector<string>::iterator it = _arguments.begin(); it != _arguments.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\r'), (*it).end());

		for (vector<string>::iterator it = _arguments.begin(); it != _arguments.end(); it++)
			(*it).erase(std::remove((*it).begin(), (*it).end(), '\n'), (*it).end());
    }
}

//
vector<User>	Command::getVectorOfUsers(){ return(_users);}

int Command::commandStart(Server &server, struct pollfd fds[]){
	string cmds[] = {"NICK", "QUIT", "PRIVMSG", "NOTICE", "HELP", "JOIN", "PART", "KICK", "BOT"};
	if (std::find(std::begin(cmds), std::end(cmds), _cmd) != std::end(cmds)){
		checkCommand(server, fds);
		return(1);
	}
}

void Command::checkCommand(Server &server, struct pollfd fds[]){
	if (_cmd == "QUIT") QuitCmd(server, fds);
	else if (_cmd == "NICK") NickCmd(server);
	else if (_cmd == "PRIVMSG") PrivmsgCmd(server);
	else if (_cmd == "NOTICE") NoticeCmd(server);
	else if (_cmd == "JOIN") JoinCmd(server);//в боте должно быть 
	else if (_cmd == "PART") PartCmd(server);
	else if (_cmd == "KICK") KickCmd(server);

	// BOT
	if (_cmd == "BOT" && _arguments[0] == "HELP") HelpCmd();
	if (_cmd == "BOT" && _arguments[0] == "INFO") InfoCmd();
	// if (_cmd == "BOT" && _arguments[0] == "SHOWUSER") ShowUserCmd();
	// if (_cmd == "BOT" && _arguments[0] == "SHOWTIME") ShowTimeCmd();
	// if (_cmd == "BOT" && _arguments[0] == "RANDNUMBER") RandNumCmd();
}

void	Command::QuitCmd(Server &server, struct pollfd fds[]){
	std::cout << RED << fds[server.getID() + 1].fd << BLUE << " disconnected" << COLOR_END << std::endl;
	fds[server.getID() + 1].fd = -1;

	vector<User>tmpVector = server.getVectorUsers();
	tmpVector.erase(tmpVector.begin() + server.getID());
	server.setVectorUsers(tmpVector);
	server.setCntConnects(-1);

	close(_fd);
}

void	Command::NoticeCmd(Server server){
	if(_arguments[0] == "*")
		return ;
	PrivmsgCmd(server);
}

void Command::NickCmd(Server &server){
	string newName = _arguments[0];
	newName.erase(std::remove(newName.begin(), newName.end(), '\n'), newName.end());
	vector<User>tmpVector = server.getVectorUsers();

	for(vector<User>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
		if ((*it).getNick() == newName){
			string InUse = ERR_ERRONEUSNICKNAME(newName);
			send(_fd, InUse.c_str(), InUse.length() + 1, 0);
			return ;
		}
	}
	server.setNick(newName, server.getID());
}

void Command::PrivmsgCmd(Server &server){
	if (_arguments.size() < 2){
		send(_fd, ERR_NEEDMOREPARAMS(string("PRIVMSG")).c_str(), ERR_NEEDMOREPARAMS(string("PRIVMSG")).length() + 1, 0);
		return ;
	}

	//
	bool usrExist = false;
	int fdToParam;

	for (vector<User>::iterator it = _users.begin(); it != _users.end(); it++){
		if ((*it).getNick() == _arguments[0]){
			usrExist = true;
			fdToParam = (*it).getFd();
			break;
		}
	}

	bool chnlExist = false;
	Channel tmpChnl;
	vector<Channel>tmpVector = server.getVectorChannels();
	for(vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
		if ((*it).ChnlName() == _arguments[0]){ //в chhannel
			chnlExist = true;
			tmpChnl = (*it);
			break;
		}
	}

	if (usrExist)
		SendMessageIrcSyntax(fdToParam, _nick, _usrname, _msg);//дописан в utils
	else if (chnlExist) 
		tmpChnl.doChannelPrivmsg(_fd, _msg, _nick, _usrname); //дописать Channel
	else {
		send(_fd, ERR_NORECIPIENT(string("PRIVMSG")).c_str(), ERR_NORECIPIENT(string("PRIVMSG")).length() + 1, 0);
		return ;
	}
}
Command::~Command(){ }
