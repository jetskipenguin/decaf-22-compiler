#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include "Token.h"

#define MAX_IDENTIFIER_LENGTH 31

// Scan string contents and return vector of tokens
std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;
    int line = 1;
    int column = 1;

    for(int i = 0; i < content.length(); ) {
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

        // Operators
        if (content[i] == '+' || content[i] == '-' || content[i] == '*' || content[i] == '/' ||
            content[i] == '=' || content[i] == '<' || content[i] == '>' || content[i] == '!' ||
            content[i] == '|' || content[i] == '.') {
            int start = i;
            i++;
            column++;
            int length = 1;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_Operator, text, line, column-length, length});
            //std::cout << "Found Operator Constant: " << text << std::endl;
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
            std::cout << "Found String Constant" << std::endl;
            int start = i;
            i++;
            column++;
            while (content[i] != '"') {
                i++;
                column++;
            }
            i++;  // Skip closing quote
            column++;
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_StringConstant, text, line, column-length, length});
            continue;
        }

        // Boolean constant
        if (content.substr(i, 4) == "true" || content.substr(i, 5) == "false") {
            int start = i;
            i += content[i] == 't' ? 4 : 5;
            column += content[i] == 't' ? 4 : 5;
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_BoolConstant, text, line,column-length, length});
            continue;
        }
        
        Error error = {ErrorType::E_UnknownToken, "Unknown token: \"" + content.substr(i, 1) + "\""};
        tokens.push_back({TokenType::T_Unknown, content.substr(i, 1), line, i, 1, error});
        //std::cout << "Unknown token at " << i << " text is: " << content[i] << std::endl;
        i++;
        column++;
    }

    return tokens;
}


void print_tokens(const std::vector<Token>& tokens) {
    for(Token token: tokens) {

        if(token.type == TokenType::T_Identifier && token.error.type == ErrorType::E_IdentifierTooLong) {
            std::cout << std::endl << std::endl << "*** Error line " << token.line << "." << std::endl
            << "*** " << token.error.message << std::endl << std::endl; 

            // Truncate identifier to max length
            std::string truncated = token.text.substr(0, MAX_IDENTIFIER_LENGTH);
            std::cout <<  "line " << token.line
            << " cols " << token.column << "-" << token.column + token.length - 1
            << " is " << truncated
            << std::endl;

            continue;
        }

        if(token.type != TokenType::T_Unknown) {
            std::cout << token.text;
        }

        // Only add padding if text is shorter than 8 characters
        if (token.text.length() < 8) {
            std::cout << std::string(8 - token.text.length(), ' ');
        } else {
            std::cout << ' ';  // Single space for long tokens
        }

        if(token.type == TokenType::T_Unknown) {
            std::cout << std::endl << "*** Error line " << token.line << "." << std::endl
            << "*** Unrecognized char: \'" << token.text << "\'" << std::endl << std::endl;
        }
        else {
            std::cout << "line " << token.line
            << " cols " << token.column << "-" << token.column + token.length - 1
            << " is " << token_to_string(token)
            << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << argv[1] << std::endl;
        return 1;
    }

    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();

    auto tokens = tokenize(content);

    print_tokens(tokens);

    return 0;
}
