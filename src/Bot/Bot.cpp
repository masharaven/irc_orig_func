#include "../inc/Command.hpp"

void Command::HelpCmd(){
    if (_arguments.size() == 1)
        send(_fd, "BOT CMDs: HELP, SHOWTIME, RANDNUMBER, INFO, SHOWUSER\r\n", 59, 0);
    else
        send(_fd, "BOT ERROR Wrong syntax\r\n", 25, 0);
}

void Command::InfoCmd(){
    string showInfo = ": BOT NICKNAME is [" + _nick + "]\r\n:BOT USERNAME IS [" + _usrname + "]\r\n";
    send(_fd, showInfo.c_str(), showInfo.length() + 1, 0);
}

void Command::ShowUserCmd(Server &server){
    if(_arguments.size() != 2){
        send(_fd, ":BOT Syntax is: BOT SHOWUSER <id>\r\n", 36, 0);
        return ;
    }

    try{
        int id = stoi(_arguments[1]);
        vector<User>tmpVectorUsrs =  server.getVectorUsers();

        string infoTOSend;
        int numOfUsrs = tmpVectorUsrs.size();
        string stringNumbOfUsrs = ":BOT SHOWUSERS NUMBER OF USERS [" + std::to_string(numOfUsrs) + "]\r\n";
        if (id > numOfUsrs - 1 || id < 0){
            send(_fd, ":BOT Out of range!\r\n", 21, 0);
            throw std::invalid_argument("Out of range");
        }

        string infoAboutUserID = ":BOT USER ID [" + std::to_string(id) + "]\r\n";
		string infoAboutUserUsername = ":BOT USER USERNAME [" + tmpVectorUsrs[id].getUsername() + "]\r\n";
		string infoAboutUserNickname = ":BOT USER NICKNAME [" + tmpVectorUsrs[id].getNick() + "]\r\n";



    }   
}