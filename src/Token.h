#pragma once
#include <string>

enum class TokenType {
    T_Identifier,
    T_IntConstant,
    T_DoubleConstant,
    T_StringConstant,
    T_BoolConstant,
    T_Operator,
    T_Reserved,
    T_Unknown
};

enum class ErrorType {
    E_IdentifierTooLong,
    E_UnknownToken
};

struct Error {
    ErrorType type;
    std::string message;
};

struct Token {
    TokenType type;
    std::string text;
    int line;
    int column;
    int length;
    Error error;
};

std::string token_to_string(Token token) {
    switch (token.type) {
        case TokenType::T_Identifier: return "T_Identifier";
        case TokenType::T_IntConstant: return "T_IntConstant (value = " + token.text + ")";
        case TokenType::T_DoubleConstant: return "T_DoubleConstant (value = " + token.text + ")";
        case TokenType::T_StringConstant: return "T_StringConstant (value = " + token.text + ")";
        case TokenType::T_BoolConstant: return "T_BoolConstant";
        case TokenType::T_Operator: return "\'" + token.text + "\'";
        case TokenType::T_Reserved: return "T_Reserved";
        default: return "Unknown";
    }
}