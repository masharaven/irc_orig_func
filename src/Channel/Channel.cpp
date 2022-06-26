#include "../inc/Channel.hpp"
#include "./inc/Server.hpp"
#include "./inc/Command.hpp"

Channel::Channel() { }

Channel::Channel(string chnlName, int fd) : _chnlName(chnlName), _fdAdm(fd) { _fds.push_back(fd); }
// string				Channel::getChannelName(){ return(_channelName); }

vector<int>			Channel::FdVector(){ return(_fds); }
int					Channel::FdAdm(){ return(_fdAdm); }
string				Channel::ChnlName(){ return(_chnlName); }

//
void				Channel::fdsPushBack(int fd) { _fds.push_back(fd); }
void				Channel::setChnlName(string chnlName) { _chnlName = chnlName; }
void				Channel::setFdVector(vector<int> &fds) { _fds = fds; }
void				Channel::setFdAdmin(int fd) { _fdAdm = fd; }
void				Channel::setNewVector(vector<int> &newVector) { _fds = newVector; }

void                newusrConnect(Server &server, int fd, string nick, int id, string chnlName){
    Channel tmpChannel = server.getChannel(id);
    vector<int> tmpFdVector = tmpChannel.FdVector();
    string JoinedUser = ":127.0.0.1 " + nick + " " + "JOIN " + chnlName + "\r\n";

    for(size_t i = 0; i < tmpFdVector.size(); i++){
        if(tmpFdVector[i] != fd)
            send(tmpFdVector[i], JoinedUser.c_str(), JoinedUser.length() + 1, 0);
    }

    string beginMsg = string(":ircserv 331 " + nick + " " + chnlName + ": No topic is set\r\n");

    send(fd, beginMsg.c_str(), beginMsg.length() + 1, 0);
}

bool checkChnlNameExist(vector<Channel> &tmpVector, string chnlName){
    for(vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
        if((*it).ChnlName() == chnlName)
            return true;
    }
}

bool Channel::checkUserInChnl(int fd){
    for (vector<int>::iterator it = _fds.begin(); it != _fds.end(); it++){
		if ((*it) == fd)
			return true;
	}
	return false;
}

void Channel::doChannelPrivmsg(int fd, string msg, string nick, string username){
    if(checkUserInChnl(fd)){
        for(vector<int>::iterator it = _fds.begin(); it != _fds.end(); it++){
            if((*it) != fd)
                SendMsgIrcSynt((*it), nick, username, msg);
        }
    }else{
        send(fd, "404 ERR_CANNOTSENDTOCHAN: \r\n", 28, 0);
            std::cout << "NOT IN THE CHANNEL!\n";
    }
}

void Command::createNewChnl(Server &server){
    Channel *chnl = new Channel(_arguments[0], _fd);
    int	_channelID = server.getChannelID();
	server.channelPushBack(chnl);
	delete chnl;

    newusrConnect(server, _fd, _nick, _channelID, _arguments[0]);
    std::cout << "NEw CHANNEL!" << _arguments[0] << " ADMIN IS " << _nick << std::endl;
    server.setChannelID(1);
}

int checkChannelErrors(vector<string> _arguments, int _fd){
    if(_arguments.size() == 0){
        string err = ERR_NEEDMOREPARAMS((string)"JOIN");
        send(_fd, err.c_str(), err.length() +1, 0);
        return 1;
    }
    if(_arguments.size() > 0){
        if(_arguments[0][0] != '#'){
            string err = ERR_BADCHANNELKEY((string)"JOIN");
            send(_fd, err.c_str(), err.length() + 1, 0);
            return 1;
        }
    }
}

void Command::JoinCmd(Server &server){
    int _chnlID = server.getChannelID();
    bool chnlNameExit = false;
    vector<Channel> tmpVector = server.getVectorChannels();
    chnlNameExit = checkChnlNameExist(tmpVector, _arguments[0]);
    int ifChnlError = checkChannelErrors(_arguments, _fd);
    if (ifChnlError)
        return ;
    if(!chnlNameExit)
        createNewChnl(server);
    else{
        for(vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
            if((*it).ChnlName() == _arguments[0]){
                if((*it).FdAdm()!= _fd){
                    vector<int>::iterator it2;
                    vector<int> tmpFd = (*it).FdVector();
                    for(it2 = tmpFd.begin(); it2 != tmpFd.end(); it2++){
                        if((*it2) == _fd)
                            break;
                    }
                    if (it2 == tmpFd.end()){
                        if(tmpFd.size() == 0)
                            (*it).setFdAdmin(_fd);
                        (*it).fdsPushBack(_fd);
                        server.channelVectorSetNew(tmpVector);
                        newusrConnect(server, _fd, _nick, _chnlID - 1, _arguments[0]);
                        std::cout << "NEW MEMBER AT " << server.getChannel(_chnlID - 1).ChnlName() << " BY FD " << _fd << " " << _nick << std::endl;
                    }
                }
            }
        }
    }
}

void    Command::PartCmd(Server &server){
    if (_arguments.size() < 1){
        string err = "461 *  PART :Not enough parameters\r\n";
        send(_fd, err.c_str(), err.length() + 1, 0);
        return ;
    }
    Channel tmpChannel;
    vector<Channel> tmpVector = server.getVectorChannelsRef();
    for(vector<Channel>::iterator it = tmpVector.begin(); it != tmpVector.end(); it++){
        if((*it).ChnlName() == _arguments[0]){
            if((*it).doPartFromChnl(_fd)){ //дописать ф-ию
                std::cout << _nick << " WAS PART FROM " << _arguments[0] << std::endl;
                string err = "YOU WAS PART FROM " + _arguments[0] + "\r\n";
                send(_fd, err.c_str(), err.length() + 1, 0);
                server.channelVectorSetNew(tmpVector);
                break;
            }
        }
    }
}

bool Channel::doPartFromChnl(int fd){
    if(checkUserInChnl(fd)){
        if(fd == _fdAdm){
            if(_fds.size() > 1){
                _fdAdm = _fds[1];
            }
        }
        vector<int>::iterator itb = _fds.begin();
        vector<int>::iterator ite = _fds.end();
        vector<int>::iterator it;
        for (it = itb; it != ite; it++) {
            if ((*it) == fd) {
                _fds.erase(it);
                return true;
            }
        }
    }
    return false;
}

void Command::KickCmd(Server &server){
    if(_arguments.size() < 2){
        string err = "461 *  KICK :Not enough parameters\r\n";
        send(_fd, err.c_str(), err.length() + 1, 0);
        return ;
    }

    vector<User> tmpVectorOfUsers = server.getVectorUsers();
    bool userExist = false;
    int userFd;
    for (vector<User>::iterator it = tmpVectorOfUsers.begin(); it != tmpVectorOfUsers.end(); it++){
        if((*it).getNick() == _arguments[1]){
            userExist = true;
            userFd = (*it).getFd();
            break;
        }
    }
    if (userExist){
        Channel tmpChannel;
        vector<Channel> tmpVector = server.getVectorChannelsRef();
        vector<Channel>::iterator it;
        for(it = tmpVector.begin(); it != tmpVector.end(); it++){
            if ((*it).ChnlName() == _arguments[0]){
                if((*it).KickFromChnl(_fd, userFd, _arguments[1])){
                    std::cout << _arguments[1] << " WAS KICKED FROM" << _arguments[0] << std::endl;
                    string err = "YOU WAS KICKED FROM " + _arguments[0] + "\r\n";
                    send(userFd, err.c_str(), err.length() + 1, 0);
                    server.channelVectorSetNew(tmpVector);
                    break;
                }
            }

        }
        if (it == tmpVector.end()){
            string chnlDoesntExist = ERR_NOSUCHCHANNEL(_arguments[0]);
            send(_fd, chnlDoesntExist.c_str(), chnlDoesntExist.length() + 1, 0);
        }
    }
    else{
        string usrDoesntExist = "441 * " + _arguments[1] + " " + _arguments[0] + " :They aren't on that channel\r\n";
        send(_fd, usrDoesntExist.c_str(), usrDoesntExist.length() + 1, 0);
    }
}

bool Channel::KickFromChnl(int fd, int userFd, string userName){
    if (fd == userFd){
        string err = "You can't KICK yourself\r\n";
        send(fd, err.c_str(), err.length() + 1, 0);
        return false;
    }
    if (fd == _fdAdm){
        vector<int>::iterator itb = _fds.begin();
        vector<int>::iterator ite = _fds.end();
		vector<int>::iterator it;
        for (it = itb; it != ite; it++){
            if ((*it) == userFd){
                _fds.erase(it);
                return true;
            }
        }
    if (it == ite){
        string err = "441 * " + userName + _chnlName + " :They aren't on that channel\n";
        send(fd, err.c_str(), err.length() + 1, 0);
		return false;
    }
    }else {
        string err = "482 *  " + _chnlName + " :You're not channel operator\n";
		send(fd, err.c_str(), err.length() + 1, 0);
		return false;
    }
    return false;
}

void Channel::printFds(){
    vector<int>::iterator itb = _fds.begin();
	vector<int>::iterator ite = _fds.end();
    std::cout << "Admin fd: " << _fdAdm << "\n";
	for (vector<int>::iterator it = itb; it != ite; it++){
		std::cout << "fd" << (*it) << "\n";
	}
}
Channel::~Channel() { }