#pragma once

#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include "../client/Client.hpp"
#include "../commands/Command.hpp"
#include "../reply/Reply.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

class Message {
  public:
    Message();
    Message(const std::string& message);
    ~Message();

    void run(Client* client);
    void parse(const std::string& message);
    void execute(const std::string& password);

    static const std::string& getPassword();

  private:
    Message(const Message&);
    Message& operator=(const Message&);

    enum { MAX_PARAMS = 15, CMDS_NBR = 12 };

    static TYPES::TokenType _commandTypes[CMDS_NBR];
    static std::string      _commandsStr[CMDS_NBR];

    Client*                  _client;
    std::string              _message;
    std::string              _cmd;
    std::vector<std::string> _parameters;
    static std::string       _password;
    Command*                 _cmdfunc;
    static uint8_t           _nbrOfParams;

    TYPES::TokenType _whichCommand();

    std::string _getMessage(std::string& msg);
    void        _command();
    void        _params();
    bool        _nospcrlfcl();
    void        _trailing();
    void        _middle();
    void        _crlf();
    void        _errUnknownCommand(const std::string& cmd);

    void _skipSpaces();
    bool _isCommand();

    void _reset();
};

#endif
