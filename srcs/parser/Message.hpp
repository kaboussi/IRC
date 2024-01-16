#pragma once

#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include "../commands/Command.hpp"
#include "ErrorReplies.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

class Message {
  public:
    Message();
    Message(const std::string& message);
    ~Message();

    void parse(const std::string& message);
    void execute(const std::string& password);

    const std::string&              getCommand() const;
    const std::vector<std::string>& getParameters() const;

    static const std::string& getPassword();

  private:
    Message(const Message&);
    Message& operator=(const Message&);

    enum { MAX_MSG_LEN = 512, MAX_PARAMS = 15, CMDS_NBR = 8 };

    static TYPES::TokenType _commandTypes[CMDS_NBR];
    static std::string      _commandsStr[CMDS_NBR];

    std::string              _cmd;
    std::vector<std::string> _parameters;
    static std::string       _password;
    Command*                 _cmdfunc;

    TYPES::TokenType _whichCommand();

    void _command();
    void _params();
    bool _nospcrlfcl();
    void _trailing();
    void _middle();

    void _skipSpaces();
    bool _isCommand();
};

#endif