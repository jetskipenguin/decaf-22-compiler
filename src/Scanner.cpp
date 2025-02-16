#include "Scanner.h"
#include <iostream> // For debugging
#include <cctype>

#define MAX_IDENTIFIER_LENGTH 31

bool Scanner::check_for_reserve_op(const std::string& content, const std::string& token, const TokenType& type) {
    if (content.substr(this->i, token.size()) == token) {
        // Check if the next character is NOT alphanumeric or underscore
        int next_char_index = this->i + token.size();
        if (next_char_index < content.size()) {
            char next_char = content[next_char_index];
            if (std::isalnum(next_char) || next_char == '_') {
                return false; // It's part of a larger identifier
            }
        }

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
    if(check_for_reserve_op(content, "Print", TokenType::T_Print)) return true;
    if(check_for_reserve_op(content, "ReadInteger", TokenType::T_ReadInteger)) return true;
    if(check_for_reserve_op(content, "ReadLine", TokenType::T_ReadLine)) return true;

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

void Scanner::tokenize_scientific_notation(const std::string& content) {
    // Some doubles will have scientific notation
    if(content[i] == 'E') {
        i++;
        column++;
        if(content[i] == '+' || content[i] == '-') {
            i++;
            column++;
        }
        // If there is an E, a + or - must be present
        else {
            i -= 2;
            column -= 2;
            return;
        }
        
        // Ignore scientific notation if it's not valid
        if(!std::isdigit(content[i])) {
            i -= 2;
            column -=2;
            return;
        }

        while(std::isdigit(content[i])) {
            i++;
            column++;
        }
    }
}

bool Scanner::tokenize_double(const std::string& content) {
    // Double constant
    if (std::isdigit(content[i])) {
        int start = i;
        int start_column = column;
        while(std::isdigit(content[i])) {
            i++;
            column++;
        }

        if(content[i] == '.') {
            i++;
            column++;
        }
        // This is not a double if there is no period
        else {
            i = start;
            column = start_column;
            return false;
        }

        // Handle numbers after period
        while(std::isdigit(content[i])) {
            i++;
            column++;
        }

        tokenize_scientific_notation(content);

        int length = i - start;
        std::string text = content.substr(start, length);
        tokens.push_back({TokenType::T_DoubleConstant, text, line, start_column, length});

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

        // Handle multi line comment
        if(content.substr(i, 2) == "/*"){
            i += 2;
            column += 2;

            while(content.substr(i, 2) != "*/") {
                if(content[i] == '\n') {
                    line++;
                    i++;
                    column = 1;
                }
                else {
                    i++;
                    column++;
                }
            }
            i += 2;
            column += 2;
            continue;
        }

        // Skip the line when a comment is encountered
        if(content.substr(i, 2) == "//") {
            while(content[i] != '\n') {
                i++;
                column++;
            }
            continue;
        }

        if(tokenize_reserve_ops(content)) continue;

        // Identifier
        if (std::isalpha(content[i]) || content[i] == '_') {
            int start = i;
            int start_column = column;
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
                tokens.push_back({TokenType::T_Identifier, text, line, start_column, length});
            }
            continue;
        }

        if(tokenize_double(content)) continue;

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
            continue;
        }

        // String constant
        if (content[i] == '"') {
            int start = i;
            int start_line = line;
            int start_column = column;
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
                line++;
                column = 0;
            }

            i++;  // Skip closing quote or newline
            column++;
            int length = i - start;
            std::string text = content.substr(start, length);

            // Subtract 1 from length and start_line to ignore
            tokens.push_back({TokenType::T_StringConstant, text, start_line, start_column, length, error});
            continue;
        }

        // Ignore directives
        if (content[i] == '#') {
            int start = i;
            i++;
            column++;
            while(content[i] != '\n') {
                i++;
                column++;
            }
            int length = i - start; 
            std::string directive_text = content.substr(start, length);
            Error error = {ErrorType::E_InvalidDirective, "Invalid # directive"};
            tokens.push_back({TokenType::T_Unknown, directive_text, line, start, length, error});
            continue;
        }
        
        Error error = {ErrorType::E_UnknownToken, "Unknown token: \"" + content.substr(i, 1) + "\""};
        tokens.push_back({TokenType::T_Unknown, content.substr(i, 1), line, i, 1, error});
        this->i++;
        this->column++;
    }

    return tokens;
}