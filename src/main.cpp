#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include "Token.h"

// Scan string contents and return vector of tokens
std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;

    for(int i = 0; i < content.length(); ) {
        // Skip whitespace
        if (std::isspace(content[i])) {
            i++;
            continue;
        }

        // Identifier
        if (std::isalpha(content[i]) || content[i] == '_') {
            int start = i;
            while (std::isalnum(content[i]) || content[i] == '_') {
                i++;
            }
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_Identifier, text, 1, start, length});
            continue;
        }

        // Integer constant
        if (std::isdigit(content[i])) {
            int start = i;
            while (std::isdigit(content[i])) {
                i++;
                std::cout << "Counting an integer" << std::endl;
            }
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_IntConstant, text, 1, start, length});
            continue;
        }

        // Double constant
        if (content[i] == '.' && std::isdigit(content[i + 1]) && std::isdigit(content[i - 1])) {
            int start = i;
            i++;
            while (std::isdigit(content[i])) {
                i++;
            }
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_DoubleConstant, text, 1, start, length});
            continue;
        }

        // String constant
        if (content[i] == '"') {
            int start = i;
            i++;
            while (content[i] != '"') {
                i++;
            }
            i++;  // Skip closing quote
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_StringConstant, text, 1, start, length});
            continue;
        }

        // Boolean constant
        if (content.substr(i, 4) == "true" || content.substr(i, 5) == "false") {
            int start = i;
            i += content[i] == 't' ? 4 : 5;
            int length = i - start;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_BoolConstant, text, 1, start, length});
            continue;
        }

        // Operators
        if (content[i] == '+' || content[i] == '-' || content[i] == '*' || content[i] == '/' ||
            content[i] == '=' || content[i] == '<' || content[i] == '>' || content[i] == '!' ||
            content[i] == '&' || content[i] == '|' || content[i] == '.') {
            int start = i;
            i++;
            int length = 1;
            std::string text = content.substr(start, length);
            tokens.push_back({TokenType::T_Operator, text, 1, start, length});
            continue;
        }

    }

    return tokens;
}


void print_tokens(const std::vector<Token>& tokens) {
    for(Token token: tokens) {
        std::cout << token.text;
        // Only add padding if text is shorter than 8 characters
        if (token.text.length() < 8) {
            std::cout << std::string(8 - token.text.length(), ' ');
        } else {
            std::cout << ' ';  // Single space for long tokens
        }


        std::cout << "line " << token.line
            << " cols " << token.column << "-" << token.column + token.length - 1
            << " is " << token_to_string(token)
            << std::endl;
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
