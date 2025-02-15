#pragma once
#include <string>

enum class TokenType {
    T_Identifier,
    T_IntConstant,
    T_DoubleConstant,
    T_StringConstant,
    T_BoolConstant,
    T_Operator,
    T_Void,
    T_Int,
    T_Double,
    T_String,
    T_While,
    T_If,
    T_Else,
    T_Return,
    T_Break,
    T_Or,
    T_LessEqual,
    T_GreaterEqual,
    T_Equal,
    T_Unknown
};

enum class ErrorType {
    E_IdentifierTooLong,
    E_UnterminatedString,
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

inline std::string token_to_string(Token token) {
    switch (token.type) {
        case TokenType::T_Identifier: return "T_Identifier";
        case TokenType::T_IntConstant: return "T_IntConstant (value = " + token.text + ")";
        case TokenType::T_DoubleConstant: return "T_DoubleConstant (value = " + token.text + ")";
        case TokenType::T_StringConstant: return "T_StringConstant (value = " + token.text + ")";
        case TokenType::T_BoolConstant: return "T_BoolConstant (value = " + token.text + ")";
        case TokenType::T_Operator: return "\'" + token.text + "\'";
        case TokenType::T_Void: return "T_Void";
        case TokenType::T_Int: return "T_Int";
        case TokenType::T_Double: return "T_Double";
        case TokenType::T_String: return "T_String";
        case TokenType::T_While: return "T_While";
        case TokenType::T_If: return "T_If";
        case TokenType::T_Else: return "T_Else";
        case TokenType::T_Return: return "T_Return";
        case TokenType::T_Break: return "T_Break";
        case TokenType::T_Or: return "T_Or";
        case TokenType::T_LessEqual: return "T_LessEqual";
        case TokenType::T_GreaterEqual: return "T_GreaterEqual";
        case TokenType::T_Equal: return "T_Equal";
        default: return "Unknown";
    }
}