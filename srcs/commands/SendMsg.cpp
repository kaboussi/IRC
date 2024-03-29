#include "SendMsg.hpp"

std::string              SendMsg::_cmd;
std::string              SendMsg::_textToSend;
std::vector<std::string> SendMsg::_users;
std::vector<std::string> SendMsg::_channels;
Client*                  SendMsg::_sender = NULL;

// PRIVMSG #kshgk,#gsj,#jksd, #ksdj :kshf skdjf kjsfh

void SendMsg::sendMessage(Client*                         client,
                          const std::vector<std::string>& parameters,
                          const std::string&              command) {
    _cmd = command;
    _sender = client;
    if (parameters.empty()) {
        _errNoRecipent();
        return;
    }
    if (!_sender->getUserInfo().isRegistered()) {
        _errNotRegistered();
        return;
    }
    Parser::init(Utils::join(parameters));
    try {
        _parseReceivers();
        _parseText();
        _sendText();
    } catch (const std::exception& e) {
    }
    _clear();
}

void SendMsg::_parseReceivers() {
    while (!Parser::isAtEnd() && !Parser::check(TYPES::SPACE)) {
        if (Parser::check(TYPES::HASH))
            _addChannel();
        else
            _addUser();
        if (Parser::match(TYPES::SPACE))
            break;
        Parser::match(TYPES::COMMA);
    }
}

void SendMsg::_parseText() {
    if (Parser::isAtEnd())
        _errNoTextToSend();
    if (Parser::match(TYPES::COLON)) {
        Parser::advance(); // skip space
        while (!Parser::isAtEnd())
            _textToSend.append(Parser::advance().lexeme());
    } else {
        _textToSend.append(Parser::end().lexeme());
    }
    _textToSend.append(CR_LF);
}

void SendMsg::_sendText() {
    for (size_t i = 0; i < _users.size(); ++i) {
        _sendToUser(_users[i]);
    }
    for (size_t i = 0; i < _channels.size(); ++i) {
        _sendToChannel(_channels[i]);
    }
}

void SendMsg::_sendToUser(const std::string& name) {
    if (name == "Bot") {
        _sendToBot();
        return;
    }
    if (!ClientList::exist(name)) {
        _errNoSuchNick(name);
        return;
    }
    Client*     receiver = ClientList::get(name);
    std::string msg = ":" + _sender->getUserInfo().getNickname() + "!~" +
                      _sender->getUserInfo().getUsername() + "@" +
                      _sender->getIp() + " " + _cmd + " " + name + " :" +
                      _textToSend;

    send(receiver->getSockfd(), msg.c_str(), msg.length(), 0);
}

void SendMsg::_sendToChannel(const std::string& name) {
    if (!TChannels::exist(name)) {
        _errNoSuchChannel(name);
        return;
    }
    Channel& channel = TChannels::channel(name);

    if (!channel.exist(_sender)) {
        _errCannotSendToChannel(name);
        return;
    }
    std::string msg = ":" + _sender->getUserInfo().getNickname() + "!~" +
                      _sender->getUserInfo().getUsername() + "@" +
                      Reactor::getInstance().getServerIp() + " " + _cmd + " " +
                      name + " :" + _textToSend + CR_LF;

    channel.sendToAll(_sender, msg);
}

void SendMsg::_errCannotSendToChannel(const std::string& name) {
    if (_cmd != "NOTICE")
        Reply::errCannotSendToChan(_sender, name,
                                   _sender->getUserInfo().getNickname());
    throw std::exception();
}

void SendMsg::_errNoSuchChannel(const std::string& name) {
    if (_cmd != "NOTICE")
        Reply::errNoSuchChannel(_sender, name);
}

void SendMsg::_addChannel() {
    std::string channel = Parser::advance().lexeme();

    if (!Parser::isAtEnd() && !Parser::check(TYPES::COMMA))
        channel.append(Parser::advance().lexeme());
    _channels.push_back(channel);
}

void SendMsg::_addUser() {
    std::string nick = Parser::peek().lexeme();

    if (Parser::peek().lexeme() != _sender->getUserInfo().getNickname())
        _users.push_back(nick);
    Parser::advance();
}

void SendMsg::_clear() {
    _textToSend.clear();
    _users.clear();
    _channels.clear();
    _sender = NULL;
}

void SendMsg::_errNoRecipent() {
    if (_cmd == "NOTICE")
        return;
    Reply::errNoRecipient(_sender, _cmd);
}

void SendMsg::_errNoTextToSend() {
    if (_cmd != "NOTICE")
        Reply::errNoTextToSend(_sender);
    throw std::exception();
}

void SendMsg::_errNoSuchNick(const std::string& name) {
    if (_cmd != "NOTICE")
        Reply::errNoSuchNick(_sender, name);
}

void SendMsg::_errNotRegistered() {
    if (_cmd == "NOTICE")
        return;
    Reply::errNotRegistered(_sender);
}

void SendMsg::_sendToBot() {
    std::string msg = std::string(":Bot!~Bot@") +
                      Reactor::getInstance().getServerIp() + " " + _cmd +
                      " Bot :";

    if (_textToSend == "/logtime\r\n") {
        msg.append(std::string(Reactor::getInstance().getTime()) + CR_LF);
    } else if (_textToSend == "/clients\r\n") {
        msg.append(Utils::toStr(ClientList::size()) + CR_LF);
    } else {
        _unsupportedParam();
        return;
    }
    send(_sender->getSockfd(), msg.c_str(), msg.length(), 0);
}

void SendMsg::_unsupportedParam() {
    std::string reply = std::string(":") + Reactor::getServerName() +
                        " 421 Bot " + _cmd + " :Unsupported param\r\n";

    send(_sender->getSockfd(), reply.c_str(), reply.length(), 0);
}
