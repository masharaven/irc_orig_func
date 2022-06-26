#ifndef GLOBAL_LIB_H
# define GLOBAL_LIB_H

#include <iostream>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
// #include <algorithm>
// #include <cstring>

using std::string;
using std::vector;

#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define COLOR_END "\033[0m"

#define BUFFER_SIZE 4096

#define ERR_NOTREGISTERED "451 *  :You have not registered\r\n"
#define ERR_UNKNOWNCOMMAND(command) ("421 *  " + command + " :Unknown command")
#define ERR_NEEDMOREPARAMS(command) ("461 *  " + command + " :Not enough parameters\n")
#define ERR_PASSWDMISMATCH "464 *  :Password incorrect\n"
#define ERR_ALREADYREGISTRED "462 *  :You may not reregister\n"
#define ERR_ERRONEUSNICKNAME(nick) ("432 *  " + nick + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nick) ("433 *  " + nick + " :Nickname is already in use\r\n")
#define ERR_NORECIPIENT(command) ("411 * :No recipient given " + command + "\n")
#define ERR_NOTEXTTOSEND "412 *  :No text to send\n"
#define ERR_CANNOTSENDTOCHAN(channel) ("404 *  " + channel " :Cannot send to channel\n")
#define ERR_NOSUCHNICK(nick) ("401 *  " + nick + " :No such nick/channel\n")
#define ERR_CHANNELISFULL(channel) ("471 *  " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_TOOMANYCHANNELS(channel) ("405 *  " + channel + " :You have joined too many channels\r\n")
#define ERR_BADCHANNELKEY(channel) ("475 * " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOSUCHCHANNEL(channel) ("403 *  " + channel + " :No such channel\n")
#define ERR_NOTONCHANNEL(channel) ("442 *  " + channel + " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(channel) ("482 *  " + channel " :You're not channel operator\n")
#define	ERR_USER_NOT_IN_CHANNEL send(fd, "441 ERR_USERNOTINCHANNEL \r\n", 28, 0)

#define NEW_USER(nickname, username) ("001 * Welcome to the Internet Relay Network " + nickname + "!" + username +"@127.0.0.1\r\n")
#define GET_USER_PASSED server.getUser(i).getFlags("USER")
#define GET_NICK_PASSED server.getUser(i).getFlags("NICK")
#define SEND_ABOUT_NEW_USER std::cout << WHITE << "NEW USER! NICKNAME: " << BLUE << "[" << server.getUser(i).getNickname() << "]" << WHITE << " USERNAME: "<< BLUE << "[" << server.getUser(i).getUsername() << "]" << WHITE << " FD " << BLUE << "[" << server.getUser(i).getFd() << "]" << NORMAL << std::endl;

// class Span
// {
// private:
// public:
// };

void			error(string error);
vector<string>	getParams(string msg, string &firstParam);

#endif