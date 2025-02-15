#include "Scanner.h"
#include <iostream> // For debugging
#include <cctype>

#define MAX_IDENTIFIER_LENGTH 31

bool Scanner::check_for_reserve_op(const std::string& content, const std::string& token, const TokenType& type) {
    if(content.substr(this->i, token.size()) == token) {
        Token t = {type, token, line, column, token.size()};
        tokens.push_back(t);
        column += token.size();
        i += token.size();
        return true;
    }
    return false;
}

bool Scanner::tokenize_reserve_ops(const std::string& content) {

    if(check_for_reserve_op(content, "void", TokenType::T_Void)) return true;
    if(check_for_reserve_op(content, "int", TokenType::T_Int)) return true;
    if(check_for_reserve_op(content, "double", TokenType::T_Double)) return true;
    if(check_for_reserve_op(content, "string", TokenType::T_String)) return true;
    if(check_for_reserve_op(content, "while", TokenType::T_While)) return true;
    if(check_for_reserve_op(content, "if", TokenType::T_If)) return true;
    if(check_for_reserve_op(content, "else", TokenType::T_Else)) return true;
    if(check_for_reserve_op(content, "return", TokenType::T_Return)) return true;
    if(check_for_reserve_op(content, "break", TokenType::T_Break)) return true;
    if(check_for_reserve_op(content, "||", TokenType::T_Or)) return true;
    if(check_for_reserve_op(content, "<=", TokenType::T_LessEqual)) return true;
    if(check_for_reserve_op(content, ">=", TokenType::T_GreaterEqual)) return true;
    if(check_for_reserve_op(content, "==", TokenType::T_Equal)) return true;
    if(check_for_reserve_op(content, "true", TokenType::T_BoolConstant)) return true;
    if(check_for_reserve_op(content, "false", TokenType::T_BoolConstant)) return true;

    if (content[i] == '+' || content[i] == '-' || content[i] == '*' || content[i] == '/' ||
        content[i] == '=' || content[i] == '<' || content[i] == '>' || content[i] == '!' ||
        content[i] == '|' || content[i] == '.' || content[i] == ';' || content[i] == ',' ||
        content[i] == '{' || content[i] == '}' || content[i] == '(' || content[i] == ')') {
        int start = i;
        i++;
        column++;
        int length = 1;
        std::string text = content.substr(start, length);
        tokens.push_back({TokenType::T_Operator, text, line, column-length, length});
        return true;
    }

    return false;
}

Scanner::Scanner() {
    i = 0;
    line = 1;
    column = 1;
    tokens = std::vector<Token>();
}

// Scan string contents and return vector of tokens
std::vector<Token> Scanner::tokenize(const std::string& content) {

    for(i = 0; i < content.size(); ) {
        if(content[i] == '\n') {
            line++;
            i++;
            column = 1;
            continue;
        }

        // Skip whitespace
        if (std::isspace(content[i])) {
            i++;
            column++;
            continue;
        }

        if(tokenize_reserve_ops(content)) {
            continue;
        }

        // Identifier
        if (std::isalpha(content[i]) || content[i] == '_') {
            int start = i;
            while (std::isalnum(content[i]) || content[i] == '_') {
                i++;
                column++;
            }
            int length = i - start;
            std::string text = content.substr(start, length);

            if(length > MAX_IDENTIFIER_LENGTH) {
                Error error = {ErrorType::E_IdentifierTooLong, "Identifier too long: \"" + text + "\""};
                tokens.push_back({TokenType::T_Identifier, text, line, column-length, length, error}); 
            }
            else {
                tokens.push_back({TokenType::T_Identifier, text, line, column-length, length});
            }
            //std::cout << "Found Identifier: " << text << std::endl;
            continue;
        }

        // Double constant
        if (content[i+1] == '.' && std::isdigit(content[i]) && std::isdigit(content[i + 2])) {
            int start = i;
            i++; 
            column++;
            // Skip numbers before decimal point
            while (std::isdigit(content[i])) {
                i++;
                column++;
            }
            // Skip decimal point
            i++;
            column++;
            // Skip numbers after decimal point
            while (std::isdigit(content[i])) {
                i++;
                column++;
            }

            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_DoubleConstant, text, line, column-length, length});
            //std::cout << "Found Double Constant: " << text << std::endl;
            continue;
        }

        // Integer constant
        if (std::isdigit(content[i]) && (content[i+1] != '.')) {
            int start = i;
            while (std::isdigit(content[i])) {
                i++;
                column++;
            }
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_IntConstant, text, line, column-length, length});
            //std::cout << "Found Integer Constant: " << text << std::endl;
            continue;
        }

        // String constant
        if (content[i] == '"') {
            //std::cout << "Found String Constant" << std::endl;
            int start = i;
            i++;
            column++;
            
            while (content[i] != '"' && content[i] != '\n') {
                i++;
                column++;
            }

            // Strings must terminate on same line as they started
            Error error = {};
            if(content[i] == '\n') {
                error.type = ErrorType::E_UnterminatedString;
                error.message = "Unterminated string constant";
            }

            i++;  // Skip closing quote or newline
            column++;
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_StringConstant, text, line, column-length, length, error});
            continue;
        }
        
        Error error = {ErrorType::E_UnknownToken, "Unknown token: \"" + content.substr(i, 1) + "\""};
        tokens.push_back({TokenType::T_Unknown, content.substr(i, 1), line, i, 1, error});
        //std::cout << "Unknown token at " << i << " text is: " << content[i] << std::endl;
        this->i++;
        this->column++;
    }

    return tokens;
}