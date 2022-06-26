#ifndef SERVER_HPP
# define SERVER_HPP

#include "GlobalLib.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Command.hpp"

class User;

class Server{
private:
	int				_port;
	int				_id;
	int				_channelID;
	int				_listenning;
	int				_cntConnects;
	string			_password;

	vector<User>	_users;
	vector<Channel>	_channels;
public:
	Server();
	Server(int port, string password);
	~Server();

	int				getID();
	int				getChannelID();
	int				getPort();
	int				getListenning();
	int				getCntConnects();
	string			getPassword();

	vector<Channel>	getVectorChannels();
	vector<Channel>	&getVectorChannelsRef();
	vector<User>	getVectorUsers(); 

	User			getUser(int i);

	Channel			getChannel(int i);

	void			setID(int i);
	void			setChannelID(int i);
	void			setCntConnects(int i);
	void			setListenning(int socket);
	void			setUsername(string username, int i);
	void			setNick(string nick, int i);
	void			setFlags(int i, string flag);
	void			setVectorUsers(vector<User> &vect);
	
	void			userPushBack(User *user);
	void			channelPushBack(Channel *channel);

	void			createSocket(Server &server);
	void			bindSocket(Server &server);
	void			listenSocket(Server &server, struct pollfd fds[]);
	void			startServ(Server &server, struct pollfd fds[]);
	void			addConnection(int &flag, struct pollfd fds[], int &i);
	void			allConnection(int &flag, struct pollfd fds[], int &i);

////////
	void SendMsgIrcSynt(int fd, string nickname, string username, string message);
	void			channelVectorSetNew(vector<Channel> &tmpVector);

};

#endif