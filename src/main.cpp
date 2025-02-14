#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include "Token.h"
#include <boost/regex.hpp>

// Scan string contents and return vector of tokens
std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;

    std::vector<std::pair<boost::regex, TokenType>> patterns = {
        {boost::regex(R"(\b(void|int|double|string|while|if|else|return|break)\b)"), TokenType::T_Reserved},
        {boost::regex(R"(\b(true|false)\b)"), TokenType::T_BoolConstant},
        {boost::regex(R"([0-9]+\.[0-9]*(?:[Ee][+-]?[0-9]+)?)"), TokenType::T_DoubleConstant},
        {boost::regex(R"(\b[0-9]+\b)"), TokenType::T_IntConstant},
        {boost::regex(R"(\"[^\"\n]*\")"), TokenType::T_StringConstant},
        {boost::regex(R"([a-zA-Z][a-zA-Z0-9_]*)"), TokenType::T_Identifier},
        {boost::regex(R"([\.])"), TokenType::T_Operator},
        {boost::regex(R"([\+])"), TokenType::T_Operator},
        {boost::regex(R"([\-])"), TokenType::T_Operator},
        {boost::regex(R"([*/<=>;,!{}()])"), TokenType::T_Operator}
    };

    for (const auto& [pattern, type] : patterns) {
        boost::sregex_iterator matches_begin(content.begin(), content.end(), pattern);
        boost::sregex_iterator matches_end;

        for (boost::sregex_iterator i = matches_begin; i != matches_end; ++i) {
            boost::smatch match = *i;
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
            << " is " << token_type_to_string(token)
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
