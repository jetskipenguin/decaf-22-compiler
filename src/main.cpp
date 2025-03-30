#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include <cstring>

#include "ASTBuilder.h"

#define MAX_IDENTIFIER_LENGTH 31

void print_token_error(const Token& token) {

    if(token.error.type == ErrorType::E_IdentifierTooLong) 
    {
        std::cout << std::endl << "*** Error line " << token.line << "." << std::endl
        << "*** " << token.error.message << std::endl << std::endl; 

        // Truncate identifier to max length
        std::string truncated = token.text.substr(0, MAX_IDENTIFIER_LENGTH);
        std::cout <<  token.text << " line " << token.line
        << " cols " << token.column << "-" << token.column + token.length - 1
        << " is T_Identifier (truncated to " << truncated << ")" << std::endl;

        return;
    }

    if(token.error.type == ErrorType::E_UnterminatedString)
    {
        std::cout << std::endl << "*** Error line " << token.line << "." << std::endl
        << "*** " << token.error.message << ": " << token.text << std::endl; 
        return;
    }

    if(token.error.type == ErrorType::E_InvalidDirective) 
    {
        std::cout << std::endl << "*** Error line " << token.line << "." << std::endl
        << "*** " << token.error.message << std::endl << std::endl; 
        return;
    }    
}


void print_tokens(const std::vector<Token>& tokens) {
    for(Token token: tokens) {
        if(!token.error.message.empty()) {
            if(token.error.type != ErrorType::E_UnknownToken) {
                print_token_error(token);
                continue;
            }
        }  
        
        if(token.type != TokenType::T_Unknown) {
            std::cout << token.text;
        }

        // Add whitespace padding
        int padding = 13 - token.text.length(); // 13 based on longest string in string.out
        if (padding > 0) {
            std::cout << std::string(padding, ' ');
        } else {
            std::cout << ' ';  // Single space if text is longer
        }

        if(token.type == TokenType::T_Unknown) {
            std::cout << std::endl << "*** Error line " << token.line << "." << std::endl
            << "*** Unrecognized char: \'" << token.text << "\'" << std::endl << std::endl;
            continue;
        }
        
        std::cout << "line " << token.line
            << " cols " << token.column << "-" << token.column + token.length - 1
            << " is " << token_to_string(token)
            << std::endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc > 3 || argc <= 1) {
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
    
    Scanner scanner;
    std::vector<Token> tokens = scanner.tokenize(content);

    if (argv[2] != nullptr) {
        if (strcmp(argv[2], "--testScanner") == 0) {
            print_tokens(tokens);
            return 0;
        }
    }

    ASTBuilder builder(tokens);
    std::shared_ptr<ASTRootNode> ast = builder.buildAST();
    ast->print();
        
    return 0;
}
