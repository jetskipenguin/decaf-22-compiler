#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include "Token.h"

class Scanner {
    private:
        int i; // Character index
        int column;
        int line;
        std::vector<Token> tokens;
        bool check_for_reserve_op(const std::string& content, const std::string& token, const TokenType& type);
        bool tokenize_reserve_ops(const std::string& content);
    public:
        Scanner();
        std::vector<Token> tokenize(const std::string& content);
};
#endif