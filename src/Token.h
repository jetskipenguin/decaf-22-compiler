#pragma once
#include <string>
#include <cmath>
#include <iostream>
#include <sstream>

enum class TokenType {
    T_Identifier,
    T_IntConstant,
    T_DoubleConstant,
    T_StringConstant,
    T_BoolConstant,
    T_Operator,
    T_Void,
    T_Int,
    T_Bool,
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
    T_Print,
    T_ReadInteger,
    T_ReadLine,
    T_For,
    T_Unknown
};

enum class ErrorType {
    E_IdentifierTooLong,
    E_UnterminatedString,
    E_InvalidDirective,
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


inline double convert_scientific_not_to_double(const std::string& double_str) {
    size_t e_pos = double_str.find('E');
    if (e_pos == std::string::npos) {
        return std::stod(double_str);
    }

    std::string base_str = double_str.substr(0, e_pos);
    double base = std::stod(base_str);

    std::string exponent_str = double_str.substr(e_pos + 1);
    int exponent = std::stoi(exponent_str);

    return base * pow(10, exponent);
}

inline std::string convert_double_to_str(const double& num) {
    std::stringstream ss;
    ss << std::fixed << num;
    std::string str = ss.str();

    // Remove trailing zeros after the decimal point
    size_t decimal_pos = str.find('.');
    if (decimal_pos != std::string::npos) {
        size_t last_non_zero = str.find_last_not_of('0');
        if (last_non_zero != std::string::npos && last_non_zero > decimal_pos) {
            str.erase(last_non_zero + 1);
        } else if (last_non_zero == decimal_pos) {
            str.erase(decimal_pos); // Remove decimal point if no digits after it
        }
    }

    // Remove leading zeros before the decimal point, but leave one 0 if the number is between -1 and 1 exclusive
    if(decimal_pos != std::string::npos) {
        size_t first_non_zero = str.find_first_not_of('0');
        if(first_non_zero != std::string::npos && first_non_zero < decimal_pos) {
            if(first_non_zero > 0 || str[0] == '-')
                str.erase(0, first_non_zero);
            else
                str.erase(0, first_non_zero);
        }
    }
    else {
        size_t first_non_zero = str.find_first_not_of('0');
        if(first_non_zero != std::string::npos) {
            if(first_non_zero > 0 || str[0] == '-')
                str.erase(0, first_non_zero);
            else
                str.erase(0, first_non_zero);
        }
    }
    
    return str;
}

inline std::string remove_leading_zeros(const std::string& str) {
    size_t first_non_zero = str.find_first_not_of('0');
    if (first_non_zero == std::string::npos) {
        return "0";
    }
    return str.substr(first_non_zero);
}


inline std::string token_to_string(Token token) {
    switch (token.type) {
        case TokenType::T_Identifier: return "T_Identifier";
        case TokenType::T_IntConstant: return "T_IntConstant (value = " + remove_leading_zeros(token.text) + ")";
        case TokenType::T_DoubleConstant: return "T_DoubleConstant (value = " + convert_double_to_str(convert_scientific_not_to_double(token.text)) + ")";
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
        case TokenType::T_Print: return "T_Print";
        case TokenType::T_ReadInteger: return "T_ReadInteger";
        case TokenType::T_ReadLine: return "T_ReadLine";
        case TokenType::T_Bool: return "T_Bool";
        default: return "Unknown";
    }
}

inline std::string token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::T_Identifier: return "T_Identifier";
        case TokenType::T_IntConstant: return "T_IntConstant";
        case TokenType::T_DoubleConstant: return "T_DoubleConstant";
        case TokenType::T_StringConstant: return "T_StringConstant";
        case TokenType::T_BoolConstant: return "T_BoolConstant";
        case TokenType::T_Operator: return "T_Operator";
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
        case TokenType::T_Print: return "T_Print";
        case TokenType::T_ReadInteger: return "T_ReadInteger";
        case TokenType::T_ReadLine: return "T_ReadLine";
        case TokenType::T_Bool: return "T_Bool";
        default: return "Unknown";
    }
}