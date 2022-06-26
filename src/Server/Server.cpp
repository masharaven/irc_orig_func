#include "../../inc/Server.hpp"
#include "../../inc/GlobalLib.hpp"
#include <vector>
#include <string>

Server::Server(int port, string password) : _port(port), _channelID(0), _cntConnects(0), _password(password){}

int				Server::getID() { return (_id); }
int				Server::getChannelID() { return (_channelID); }
int				Server::getPort() { return (_port); }
int				Server::getListenning() { return (_listenning); }
int				Server::getCntConnects() { return (_cntConnects); }
vector<Channel>	Server::getVectorChannels() { return (_channels); }	//
vector<Channel>	&Server::getVectorChannelsRef() { return (_channels); }	//
vector<User>	Server::getVectorUsers() { return (_users); };
Channel			Server::getChannel(int i) { return (_channels[i]); }
string			Server::getPassword() { return (_password); }

User			Server::getUser(int i) { return (_users[i]); }

void			Server::setID(int id) { _id = id; }
void			Server::setChannelID(int i) { _channelID += i; }
void			Server::setListenning(int socket) { _listenning = socket; }
void			Server::setCntConnects(int i) { _cntConnects += i; }
void			Server::setFlags(int i, string flag) { _users[i].setFlags(flag); }
void			Server::setUsername(string username, int i) { _users[i].setUsername(username); }
void			Server::setNick(string nick, int i) { _users[i].setNick(nick); }

void			Server::channelPushBack(Channel *channel) { _channels.push_back(*channel); }
void			Server::userPushBack(User *user) { _users.push_back(*user); }

void			Server::setVectorUsers(vector<User> &vect) { _users = vect; }

////
void			Server::channelVectorSetNew(vector<Channel> &tmpVector){ _channels = tmpVector;}

////

void 	Server::createSocket(Server &server){
	server.setListenning(socket(AF_INET, SOCK_STREAM, 0));
	if (server.getListenning() == -1){
		error("Can't create a socket");
	}
}

void 	Server::bindSocket(Server &server) {
	int flag = 1;

	if (setsockopt(server.getListenning(), SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0) {
		error("Setsockpt crash");
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(server.getPort());
	hint.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "127.0.0.1", &hint.sin_addr);

	if (bind(server.getListenning(), (sockaddr *)&hint, sizeof(hint)) == -1) {
		error("Can't bind");
	}
}

void 	Server::listenSocket(Server &server, struct pollfd fds[]) {
	if (listen(server.getListenning(), SOMAXCONN) == -1) {
		error("Can't listen");
	}

	fds[0].fd = server.getListenning();
	fcntl(fds[0].fd, F_SETFL, O_NONBLOCK);
}

void	Server::startServ(Server &server, struct pollfd fds[]) {
	int flag = 0;
	std::cout << "Server start!\n";

	while(1) {
		int fdcnt;
		if (flag > 0) {
			std::cout << "Exit\n";
			exit(EXIT_SUCCESS);
		}
		if ((fdcnt = poll(fds, server.getCntConnects(), -1)) < 0) {
			error("Poll crash");
		}
		for (int i = 0; i < server.getCntConnects(); i++) {
			if (fds[i].fd > 0 && (fds[i].revents & POLLIN) == POLLIN) {
				++flag;
				if (i == 0)
					server.addConnection(flag, fds, i);
				else
					server.allConnection(flag, fds, i);
			}
		}
	}
}

void	Server::addConnection(int &flag, struct pollfd fds[], int &i) {
	User *user = new User(fds[i].fd);
	_users.push_back(*user);
	delete user;

	flag = 0;
	fds[getCntConnects()].fd = accept(fds[i].fd, NULL, NULL);
	std::cout << BLUE << "NEW CONNECT" << COLOR_END << std::endl;
	fds[getCntConnects()].events = POLLIN;
	fds[getCntConnects()].revents = 0;
	setCntConnects(1);
}
void	Server::allConnection(int &flag, struct pollfd fds[], int &i) {
	char buf[BUFFER_SIZE];
	int res;

	flag = 0;
	memset(buf, 0, BUFFER_SIZE);
	res = read(fds[i].fd, buf, BUFFER_SIZE);
	fds[i].revents = 0;
	if (!res) {
		std::cout << RED << fds[i].fd << " disconnected" << COLOR_END << std::endl;
		fds[i].fd = -1;
		_users.erase(_users.begin() + i - 1);
		setCntConnects(-1);
	}
	buf[res] = 0;
	_users[i - 1].setFd(fds[i].fd);
	setID(i - 1);
	
	_users[i].settingParams(*this, std::string(buf), i - 1, fds);
	// std::cout << buf;

	fds[i].revents = 0;
}

Server::~Server(){}