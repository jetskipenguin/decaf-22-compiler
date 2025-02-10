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

    std::vector<std::pair<std::regex, TokenType>> patterns = {
        {std::regex(R"(\b(void|int|double|string|while|if|else|return|break)\b)"), TokenType::T_Reserved},
        {std::regex(R"(\b(true|false)\b)"), TokenType::T_BoolConstant},
        {std::regex(R"([0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?)"), TokenType::T_DoubleConstant},
        {std::regex(R"([0-9]+)"), TokenType::T_IntConstant},
        {std::regex(R"(\"[^\"\n]*\")"), TokenType::T_StringConstant},
        {std::regex(R"(\b[a-zA-Z][a-zA-Z0-9_]*\b)"), TokenType::T_Identifier},
        {std::regex(R"(\+|-|\*|/|<|>|=|;|,|!|\{|\}|\(|\)|\|\||<=|>=|==)"), TokenType::T_Operator}
    };


    for (const auto& [pattern, type] : patterns) {
        auto matches_begin = std::sregex_iterator(content.begin(), content.end(), pattern);
        auto matches_end = std::sregex_iterator();

        for (std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
            std::smatch match = *i;
            size_t pos = match.position();
            
            size_t line_number = 1 + std::count(content.begin(), content.begin() + pos, '\n');
            size_t line_start = content.rfind('\n', pos);
            
            if (line_start == std::string::npos) {
                line_start = 0;
            }
            else {
                line_start++;
            }

            size_t column = pos - line_start + 1;

            tokens.push_back({
                type,
                match.str(),
                static_cast<int>(line_number),
                static_cast<int>(column),
                static_cast<int>(match.str().length())
            });
        }
    }

    // Sort tokens by line number
    std::sort(tokens.begin(), tokens.end(), [](const Token& a, const Token& b) {
        if (a.line != b.line) {
            return a.line < b.line;
        }
        return a.column < b.column;
    });

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
            << " is " << token_type_to_string(token.type)
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
