#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "GlobalLib.hpp"
#include "User.hpp"
#include "Server.hpp"
#include <chrono>
#include <time.h
// #include "Channel.hpp"

class User;
class Server;
class Channel;

class Command{
    private:

    int     _fd;
    int     _iter;
    string  _msg;
    string  _usrname;
    string  _nick;
    string  _cmd;

    vector<User>    _users;
    vector<string>  _arguments;

    public:
    Command();
    Command(string msg, int fd, string nick, vector<User> &users);
    ~Command();

    //
    vector<User>    getVectorOfUsers();

    int     commandStart(Server &server, struct pollfd fds[]);
    void       checkCommand(Server &server, struct pollfd fds[]);

    void QuitCmd(Server &server, struct pollfd fds[]);
    void NoticeCmd(Server server);
    void NickCmd(Server &server);
    void PrivmsgCmd(Server &server);

    //channel cmds in bot
    void createNewChnl(Server &server);
    void JoinCmd(Server &server);
    void PartCmd(Server &server);
    void KickCmd(Server &server);

    void HelpCmd();
    void InfoCmd();

    void ShowUserCmd(Server &server);


};
#endif