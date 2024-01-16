#include "Parser.hpp"

Lexer Parser::_lexer;
Token Parser::_token;
Token Parser::_prev;

void Parser::init(const std::string& source) {
    _lexer = Lexer(source);
    _token = _lexer.getNextToken();
    _prev = _token;
}

void Parser::consume(const TYPES::TokenType& type, const char* message) {
    if (!match(type))
        throw std::runtime_error(message);
}

bool Parser::match(const TYPES::TokenType& type) {
    if (isAtEnd() || !check(type))
        return false;
    advance();
    return true;
}

const Token& Parser::advance() {
    if (isAtEnd())
        return _token;
    _prev = _token;
    _token = _lexer.getNextToken();
    return _prev;
}

const Token& Parser::previous() {
    return _prev;
}

const Token& Parser::peek() {
    return _token;
}

bool Parser::check(const TYPES::TokenType& type) {
    return _token.type() == type;
}

bool Parser::isAtEnd() {
    return _token.type() == TYPES::END;
}

bool Parser::skipSpaces() {
    if (!check(TYPES::SPACE))
        return false;
    while (check(TYPES::SPACE))
        advance();
    return true;
}