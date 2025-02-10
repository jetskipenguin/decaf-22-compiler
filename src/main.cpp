#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>
#include "Token.h"

// Scan string contents and return vector of tokens
std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;

    std::vector<std::pair<std::regex, TokenType>> patterns = {
        {std::regex(R"(\b(void|int|double|string|while|if|else|return|break)\b)"), TokenType::T_Reserved},
        {std::regex(R"(\b(true|false)\b)"), TokenType::T_BoolConstant},
        {std::regex(R"([0-9]+\.[0-9]*([Ee][+-]?[0-9]+)?)"), TokenType::T_DoubleConstant},
        {std::regex(R"([0-9]+)"), TokenType::T_IntConstant},
        {std::regex(R"([a-zA-Z][a-zA-Z0-9_]*)"), TokenType::T_Identifier},
        {std::regex(R"(\"[^\"\\n]*\")"), TokenType::T_StringConstant},
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
    for(const auto& token: tokens) {
        std::cout << token.text
            << " line " << token.line
            << " cols " << token.column << "-" << token.column + token.length
            << " is " << token_type_to_string(token.type)
            << std::endl;
    }
}

int main() {
    std::ifstream file("/home/jetskipenguin/C++/decaf-22-compiler/samples/badbool.frag");
    if (!file.is_open()) {
        std::cerr << "Failed to open badbool.frag" << std::endl;
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
