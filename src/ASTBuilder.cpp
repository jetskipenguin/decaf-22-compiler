#include <numeric>
#include "ASTBuilder.h"
#include <algorithm>

// Helper methods for token handling
Token ASTBuilder::currentToken() const {
    if (currentTokenIndex >= tokens.size()) {
        // Return an EOF token if we've reached the end
        Token eofToken;
        eofToken.type = TokenType::T_Unknown;
        eofToken.text = "EOF";
        return eofToken;
    }
    return tokens[currentTokenIndex];
}


void ASTBuilder::nextToken() {
    if (currentTokenIndex <= tokens.size()) {
        currentTokenIndex++;
        return;
    }
    std::cout << "Throwing exception, reached end of file" << std::endl;
    throw std::out_of_range("Reached EOF");
}

bool ASTBuilder::match(TokenType type) {
    if (check(type)) {
        nextToken();
        return true;
    }
    return false;
}

bool ASTBuilder::check(TokenType type) const {
    return currentToken().type == type;
}

void ASTBuilder::consume(TokenType type) {
    if (check(type)) {
        if(verbose) {
            std::cout << "Consumed " << currentToken().text << " as " << token_type_to_string(type) << std::endl;
        }
        nextToken();
    } else {
        if(verbose) {
            std::cerr << "Error: Expected " <<  token_type_to_string(type)
                 << " but got " << token_type_to_string(currentToken().type)
                 << " at line " << currentToken().line 
                 << ", column " << currentToken().column << std::endl;
        }
    }
}

void ASTBuilder::consume(TokenType type, std::string character) {
    if(check(type) && currentToken().text == character) {
        if(verbose) {
            std::cout << "Consumed " << currentToken().text << " as " << token_type_to_string(type) << " " << character << std::endl;
        }
        nextToken();
    } else {
        std::cout << std::endl << "*** Error line " << currentToken().line << "." << std::endl
            << sourceCode.at(currentToken().line-1) << std::endl
            << "  ^" << std::endl
            << "*** syntax error" << std::endl << std::endl;
        throw std::runtime_error("Syntax error");
    }
}

void ASTBuilder::pushScope() {
    symbolTable.push_back({});
}

void ASTBuilder::popScope() {
    if (!symbolTable.empty()) {
        symbolTable.pop_back();
    }
}

void ASTBuilder::addToCurrentScope(const std::string& name, ASTNodeType* type) {
    std::cout << "Adding variable " << name << " to current scope" << std::endl;
    if (symbolTable.empty()) {
        pushScope(); // Ensure there's at least one scope
    }
    symbolTable.back()[name] = type;
}

ASTNodeType* ASTBuilder::lookupVariable(const std::string& name) {
    for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        auto& scope = *it;
        auto found = scope.find(name);
        if (found != scope.end()) {
            return found->second;
        }
    }
    return ASTNodeType::errorType; // Not found
}

// Main entry point for building the AST
std::shared_ptr<ASTRootNode> ASTBuilder::buildAST() {
    return parseProgram();
}

// Program -> Decl+
std::shared_ptr<ASTRootNode> ASTBuilder::parseProgram() {
    int line = currentToken().line;
    int column = currentToken().column;
    auto program = std::make_shared<ASTRootNode>(line, column);
    
    while (currentTokenIndex < tokens.size()) {
            auto decl = parseDecl();
            if (decl) {
                program->addDecl(decl);
            } else {
            // Skip invalid declaration and try to recover
                    nextToken();
        }
    }
    
    return program;
}

// Decl -> VarDecl | FunctionDecl
std::shared_ptr<Decl> ASTBuilder::parseDecl() {
    // Check for type to determine if it's a declaration
    ASTNodeType* type = parseType();
    if (!type) return nullptr;
    
    // Now we need to check if it's a function or variable declaration
    // Both start with a type and an identifier
    if (!check(TokenType::T_Identifier)) {
        return nullptr;
    }
    
    std::string identifierName = currentToken().text;
    int line = currentToken().line;
    int column = currentToken().column;
    consume(TokenType::T_Identifier);
    
    auto id = std::make_shared<Identifier>(identifierName, line, column);
    
    // If the next token is '(', it's a function declaration
    if (check(TokenType::T_Operator) && currentToken().text == "(") {
        return parseFunctionDecl(type, id, line, column);
    } else {
        // Otherwise, it's a variable declaration
        return parseVarDeclAfterType(type, id, line, column);
    }
}

std::shared_ptr<FunctionDecl> ASTBuilder::parseFunctionDecl(
    ASTNodeType* returnType, std::shared_ptr<Identifier> id, int line, int column) {
    
    auto funcDecl = std::make_shared<FunctionDecl>(returnType, id, line, column);
    
    consume(TokenType::T_Operator, "(");
    pushScope();

    // Parse parameters
    if (!check(TokenType::T_Operator) || currentToken().text != ")") {
        while(true) {
            ASTNodeType* paramType = parseType();
            if (!paramType) {
                std::cerr << "Error: Expected parameter type at line " << currentToken().line << std::endl;
                break;
            }
            
            if (!check(TokenType::T_Identifier)) {
                std::string indentStr(currentToken().column-1, ' ');
                std::cout << std::endl << "*** Error line " << currentToken().line << "." << std::endl
                << sourceCode.at(currentToken().line-1) << std::endl 
                << indentStr << "^" << std::endl
                << "*** syntax error" << std::endl << std::endl;
                throw std::runtime_error("Syntax error");
            }
            
            std::string paramName = currentToken().text;
            int paramLine = currentToken().line;
            int paramColumn = currentToken().column;
            consume(TokenType::T_Identifier);
            
            auto paramId = std::make_shared<Identifier>(paramName, paramLine, paramColumn);
            auto param = std::make_shared<VarDecl>(paramType, paramId, nullptr, paramLine, paramColumn);
            addToCurrentScope(paramName, paramType);
            funcDecl->addFormal(param);
            
            if (currentToken().text != ",") {
                if(verbose) {
                    std::cout << "Exiting function declaration" << std::endl;
                }
                break;
            }
            consume(TokenType::T_Operator, ",");
        }
    }

    // Now explicitly check for and consume the closing parenthesis
    if (!check(TokenType::T_Operator) || currentToken().text != ")") {
        std::cerr << "Error: Expected ')' at line " << currentToken().line << std::endl;
    } else {
        consume(TokenType::T_Operator, ")");
    }
    
    // Parse function body
    auto body = parseBlock();
    popScope();
    funcDecl->setBody(body);
    
    return funcDecl;
}

// Helper function to parse variable declaration after the type and identifier
std::shared_ptr<VarDecl> ASTBuilder::parseVarDeclAfterType(
    ASTNodeType* type, std::shared_ptr<Identifier> id, int line, int column) {
    
    std::shared_ptr<Expr> init = nullptr;
    
    // Check for initialization
    if (check(TokenType::T_Operator) && currentToken().text == "=") {
        consume(TokenType::T_Operator, "="); // Consume '='
        init = parseExpr();
    }
    
    consume(TokenType::T_Operator, ";"); // Consume ';'
    addToCurrentScope(id->name, type);
    
    return std::make_shared<VarDecl>(type, id, init, line, column);
}


std::shared_ptr<VarDecl> ASTBuilder::parseVarDecl() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    ASTNodeType* type = parseType();
    if (!type) return nullptr;
    
    if (!check(TokenType::T_Identifier)) {
        std::cerr << "Error: Expected identifier after type at line " << line << std::endl;
        return nullptr;
    }
    
    std::string name = currentToken().text;
    consume(TokenType::T_Identifier);
    
    auto id = std::make_shared<Identifier>(name, line, column);
    std::shared_ptr<Expr> init = nullptr;
    
    // Check for initialization
    if (check(TokenType::T_Operator) && currentToken().text == "=") {
        consume(TokenType::T_Operator, "="); // Consume '='
        init = parseExpr();
    }
    
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    return std::make_shared<VarDecl>(type, id, init, line, column);
}

// Type -> 'void' | 'int' | 'double' | 'bool' | 'string'
ASTNodeType* ASTBuilder::parseType() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    if (match(TokenType::T_Void)) {
        return ASTNodeType::voidType;
    } else if (match(TokenType::T_Int)) {
        return ASTNodeType::intType;
    } else if (match(TokenType::T_Double)) {
        return ASTNodeType::doubleType;
    } else if (match(TokenType::T_Bool)) {
        return ASTNodeType::boolType;
    } else if (match(TokenType::T_String)) {
        return ASTNodeType::stringType;
    }
    
    return nullptr;
}

// Block -> '{' Stmt* '}'
std::shared_ptr<BlockStmt> ASTBuilder::parseBlock() {
    int line = currentToken().line;
    int column = currentToken().column;

    if (!check(TokenType::T_Operator) || currentToken().text != "{") {
        std::cerr << "Error: Expected '{' at line " << line << std::endl; // Removed line-1
        return nullptr;
    }
    
    consume(TokenType::T_Operator, "{"); // Consume '{'
    pushScope();
    
    auto block = std::make_shared<BlockStmt>(line, column);
    
    // Parse statements until we hit '}'
    while (!check(TokenType::T_Operator) || currentToken().text != "}") {
        auto stmt = parseStmt();
        if (stmt) {
            block->addStmt(stmt);
        } else {
            // Skip invalid statement and try to recover
            nextToken();
        }
        
        // Break if we've reached the end of the token stream
        if (currentToken().type == TokenType::T_Unknown) {
            if(verbose) {
                std::cerr << "Warning: Unexpected end of file while parsing block" << std::endl;
            }
            break;
        }
    }

    consume(TokenType::T_Operator, "}"); // Consume '}'
    popScope(); // Exit scope
    return block;
}

// Stmt ->  Block | IfStmt | WhileStmt | ForStmt | ReturnStmt | BreakStmt | PrintStmt | ExprStmt | VarDeclStatement
std::shared_ptr<Stmt> ASTBuilder::parseStmt() {
    // Check for block statement
    if (check(TokenType::T_Operator) && currentToken().text == "{") {
        return parseBlock();
    }
    
    // Check for if statement
    if (check(TokenType::T_If)) {
        return parseIfStmt();
    }
    
    // Check for while statement
    if (check(TokenType::T_While)) {
        return parseWhileStmt();
    }
    
    // Check for for statement
    if (check(TokenType::T_For)) {
        return parseForStmt();
    }
    
    // Check for return statement
    if (check(TokenType::T_Return)) {
        return parseReturnStmt();
    }
    
    // Check for break statement
    if (check(TokenType::T_Break)) {
        return parseBreakStmt();
    }
    
    // Check for print statement
    if (check(TokenType::T_Print)) {
        return parsePrintStmt();
    }

    // if (check(TokenType::T_ReadInteger)) {
    //     return parseReadIntegerStmt();
    // }
    
    // Check for variable declaration
    TokenType type = currentToken().type;
    if (type == TokenType::T_Int || type == TokenType::T_Double || 
        type == TokenType::T_String || type == TokenType::T_Void || 
        type == TokenType::T_Bool) {  // Corrected to T_Bool
        
        auto varDecl = parseVarDecl();
        if (!varDecl) return nullptr;
        
        addToCurrentScope(varDecl->identifier->name, varDecl->type);
        return std::make_shared<VarDeclStmt>(varDecl, varDecl->line, varDecl->column);
    }
    
    // Otherwise, it's an expression statement
    return parseExprStmt();
}

// IfStmt -> 'if' '(' Expr ')' Stmt ('else' Stmt)?
std::shared_ptr<Stmt> ASTBuilder::parseIfStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    consume(TokenType::T_If);
    consume(TokenType::T_Operator, "("); // Consume '('
    
    auto condition = parseExpr();
    
    consume(TokenType::T_Operator, ")"); // Consume ')'
    
    auto thenStmt = parseStmt();
    std::shared_ptr<Stmt> elseStmt = nullptr;
    
    if (match(TokenType::T_Else)) {
        elseStmt = parseStmt();
    }
    
    return std::make_shared<IfStmt>(condition, thenStmt, elseStmt, line, column);
}

// WhileStmt -> 'while' '(' Expr ')' Stmt
std::shared_ptr<Stmt> ASTBuilder::parseWhileStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    consume(TokenType::T_While);
    consume(TokenType::T_Operator, "("); // Consume '('
    
    auto condition = parseExpr();
    
    consume(TokenType::T_Operator, ")"); // Consume ')'
    
    auto body = parseStmt();
    
    return std::make_shared<WhileStmt>(condition, body, line, column);
}

// ForStmt -> 'for' '(' (Expr)? ';' Expr? ';' Expr? ')' Stmt
std::shared_ptr<Stmt> ASTBuilder::parseForStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    consume(TokenType::T_For);
    consume(TokenType::T_Operator, "("); // Consume '('
    
    // Parse initialization expression (optional)
    std::shared_ptr<Expr> init = nullptr;
    if (!check(TokenType::T_Operator) || currentToken().text != ";") {
        init = parseExpr();
    }
    
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    // Parse condition expression (optional)
    std::shared_ptr<Expr> cond = nullptr;
    if (!check(TokenType::T_Operator) || currentToken().text != ";") {
        cond = parseExpr();
    }
    
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    // Parse update expression (optional)
    std::shared_ptr<Expr> update = nullptr;
    if (!check(TokenType::T_Operator) || currentToken().text != ")") {
        update = parseExpr();
    }
    
    consume(TokenType::T_Operator, ")"); // Consume ')'
    
    auto body = parseStmt();
    
    return std::make_shared<ForStmt>(init, cond, update, body, line, column);
}

// ReturnStmt -> 'return' Expr? ';'
std::shared_ptr<Stmt> ASTBuilder::parseReturnStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    consume(TokenType::T_Return);
    
    std::shared_ptr<Expr> value = nullptr;
    if (!check(TokenType::T_Operator) || currentToken().text != ";") {
        value = parseExpr();
    }
    
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    return std::make_shared<ReturnStmt>(value, line, column);
}

// BreakStmt -> 'break' ';'
std::shared_ptr<Stmt> ASTBuilder::parseBreakStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    consume(TokenType::T_Break);
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    return std::make_shared<BreakStmt>(line, column);
}

// PrintStmt -> 'Print' '(' Expr (',' Expr)* ')' ';'
std::shared_ptr<Stmt> ASTBuilder::parsePrintStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    consume(TokenType::T_Print);
    consume(TokenType::T_Operator, "("); // Consume '('
    
    auto printStmt = std::make_shared<PrintStmt>(line, column);
    
    // Parse at least one expression
    if (!check(TokenType::T_Operator) || currentToken().text != ")") {
        do {
            auto arg = parseExpr();
            printStmt->addArg(arg);
            if (!check(TokenType::T_Operator) || currentToken().text != ",") break;
            consume(TokenType::T_Operator, ",");
        } while (true);
    }
    
    consume(TokenType::T_Operator, ")"); // Consume ')'
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    return printStmt;
}

// ExprStmt -> Expr ';'
std::shared_ptr<Stmt> ASTBuilder::parseExprStmt() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseExpr();
    
    consume(TokenType::T_Operator, ";"); // Consume ';'
    
    return std::make_shared<ExprStmt>(expr, line, column);
}

// Expression parsing methods follow precedence rules
// Expr -> Assignment
std::shared_ptr<Expr> ASTBuilder::parseExpr() {
    return parseAssignment();
}

// Assignment -> LogicalOr ('=' Assignment)?
std::shared_ptr<Expr> ASTBuilder::parseAssignment() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseLogicalOr();
    
    if (check(TokenType::T_Operator) && currentToken().text == "=") {
        consume(TokenType::T_Operator, "="); // Consume '='
        auto value = parseAssignment();
        expr = std::make_shared<AssignExpr>(expr, value, line, column);
    }
    
    return expr;
}

// LogicalOr -> LogicalAnd ('||' LogicalAnd)*
std::shared_ptr<Expr> ASTBuilder::parseLogicalOr() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseLogicalAnd();
    
    while (check(TokenType::T_Or)) {
        consume(TokenType::T_Or);
        auto right = parseLogicalAnd();
        expr = std::make_shared<BinaryExpr>(BinaryExpr::Or, expr, right, line, column);
    }
    
    return expr;
}

// LogicalAnd -> Equality ('&&' Equality)*
std::shared_ptr<Expr> ASTBuilder::parseLogicalAnd() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseEquality();
    
    while (check(TokenType::T_And)) {
        consume(TokenType::T_And); // Consume '&&'
        auto right = parseEquality();
        expr = std::make_shared<BinaryExpr>(BinaryExpr::And, expr, right, line, column);
    }
    
    return expr;
}

// Equality -> Relational (('==' | '!=') Relational)*
std::shared_ptr<Expr> ASTBuilder::parseEquality() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseRelational();
    
    while (true) {
        if (check(TokenType::T_Equal)) {
            consume(TokenType::T_Equal);
            auto right = parseRelational();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Equal, expr, right, line, column);
        } else if (check(TokenType::T_NotEqual)) {
            consume(TokenType::T_NotEqual); // Consume '!='
            auto right = parseRelational();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::NotEqual, expr, right, line, column);
        } else {
            break;
        }
    }
    
    return expr;
}

// Relational -> Additive (('<' | '<=' | '>' | '>=') Additive)*
std::shared_ptr<Expr> ASTBuilder::parseRelational() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseAdditive();
    
    while (true) {
        if (check(TokenType::T_Operator) && currentToken().text == "<") {
            consume(TokenType::T_Operator, "<"); // Consume '<'
            auto right = parseAdditive();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Less, expr, right, line, column);
        } else if (check(TokenType::T_LessEqual)) {
            consume(TokenType::T_LessEqual);
            auto right = parseAdditive();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::LessEqual, expr, right, line, column);
        } else if (check(TokenType::T_Operator) && currentToken().text == ">") {
            consume(TokenType::T_Operator, ">"); // Consume '>'
            auto right = parseAdditive();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Greater, expr, right, line, column);
        } else if (check(TokenType::T_GreaterEqual)) {
            consume(TokenType::T_GreaterEqual);
            auto right = parseAdditive();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::GreaterEqual, expr, right, line, column);
        } else {
            break;
        }
    }
    
    return expr;
}

// Additive -> Multiplicative (('+' | '-') Multiplicative)*
std::shared_ptr<Expr> ASTBuilder::parseAdditive() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseMultiplicative();
    
    while (true) {
        if (check(TokenType::T_Operator) && currentToken().text == "+") {
            consume(TokenType::T_Operator, "+"); // Consume '+'
            auto right = parseMultiplicative();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Plus, expr, right, line, column);
        } else if (check(TokenType::T_Operator) && currentToken().text == "-") {
            consume(TokenType::T_Operator, "-"); // Consume '-'
            auto right = parseMultiplicative();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Minus, expr, right, line, column);
        } else {
            break;
        }
    }
    
    return expr;
}

// Multiplicative -> Unary (('*' | '/' | '%') Unary)*
std::shared_ptr<Expr> ASTBuilder::parseMultiplicative() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parseUnary();
    
    while (true) {
        if (check(TokenType::T_Operator) && currentToken().text == "*") {
            consume(TokenType::T_Operator, "*"); // Consume '*'
            auto right = parseUnary();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Multiply, expr, right, line, column);
        } else if (check(TokenType::T_Operator) && currentToken().text == "/") {
            consume(TokenType::T_Operator, "/"); // Consume '/'
            auto right = parseUnary();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Divide, expr, right, line, column);
        } else if (check(TokenType::T_Operator) && currentToken().text == "%") {
            consume(TokenType::T_Operator, "%"); // Consume '%'
            auto right = parseUnary();
            expr = std::make_shared<BinaryExpr>(BinaryExpr::Modulo, expr, right, line, column);
        } else {
            break;
        }
    }
    
    return expr;
}

// Unary -> ('-' | '!') Unary | Call
std::shared_ptr<Expr> ASTBuilder::parseUnary() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    if (check(TokenType::T_Operator) && currentToken().text == "-") {
        consume(TokenType::T_Operator, "-"); // Consume '-'
        auto right = parseUnary();
        return std::make_shared<UnaryExpr>(UnaryExpr::Minus, right, line, column);
    } else if (check(TokenType::T_Operator) && currentToken().text == "!") {
        consume(TokenType::T_Operator, "!"); // Consume '!'
        auto right = parseUnary();
        return std::make_shared<UnaryExpr>(UnaryExpr::Not, right, line, column);
    }
    
    return parseCall();
}

// Call -> Primary ('(' Args ')')?
std::shared_ptr<Expr> ASTBuilder::parseCall() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    auto expr = parsePrimary();
    
    if (check(TokenType::T_Operator) && currentToken().text == "(") {
        // Handle function call
        consume(TokenType::T_Operator, "("); // Consume '('
        
        // Check if it's a function identifier
        if (auto var = std::dynamic_pointer_cast<VarExpr>(expr)) {
            auto callExpr = std::make_shared<CallExpr>(var->id, line, column);
            
            // Parse arguments if any
            if (!check(TokenType::T_Operator) || currentToken().text != ")") {
                // Parse the first argument
                auto arg = parseExpr();
                callExpr->addArg(arg);

                // Parse all other args
                while (check(TokenType::T_Operator) && currentToken().text == ",") {
                    consume(TokenType::T_Operator, ",");
                    arg = parseExpr();
                    callExpr->addArg(arg);
                }
            }
            consume(TokenType::T_Operator, ")"); // Consume ')'
            
            return callExpr;
        } else {
            std::cerr << "Error: Cannot call non-function at line " << line << std::endl;
            // Skip to closing parenthesis
            while (!check(TokenType::T_Operator) || currentToken().text != ")") {
                nextToken();
            }
            consume(TokenType::T_Operator, ")"); // Consume ')'
            return expr;
        }
    }
    
    return expr;
}

// Primary -> IntConstant | DoubleConstant | StringConstant | BoolConstant | 'null' | '(' Expr ')' | Identifier
std::shared_ptr<Expr> ASTBuilder::parsePrimary() {
    int line = currentToken().line;
    int column = currentToken().column;
    
    // Parse integer literal
    if (check(TokenType::T_IntConstant)) {
        int value = std::stoi(currentToken().text);
        consume(TokenType::T_IntConstant);
        return std::make_shared<IntLiteral>(value, line, column);
    }
    
    // Parse double literal
    if (check(TokenType::T_DoubleConstant)) {
        double value = std::stod(currentToken().text);
        consume(TokenType::T_DoubleConstant);
        return std::make_shared<DoubleLiteral>(value, line, column);
    }
    
    // Parse string literal
    if (check(TokenType::T_StringConstant)) {
        std::string value = currentToken().text;
        consume(TokenType::T_StringConstant);
        return std::make_shared<StringLiteral>(value, line, column);
    }
    
    // Parse boolean literal
    if (check(TokenType::T_BoolConstant)) {
        bool value = (currentToken().text == "true");
        consume(TokenType::T_BoolConstant);
        return std::make_shared<BoolLiteral>(value, line, column);
    }
    
    // Parse null literal
    if (check(TokenType::T_Operator) && currentToken().text == "null") {
        consume(TokenType::T_Operator, "null");
        return std::make_shared<NullLiteral>(line, column);
    }
    
    // Parse parenthesized expression
    if (check(TokenType::T_Operator) && currentToken().text == "(") {
        consume(TokenType::T_Operator, "("); // Consume '('
        auto expr = parseExpr();
        consume(TokenType::T_Operator, ")"); // Consume ')'
        return expr;
    }
    
    // Parse identifier (variable reference)
    if (check(TokenType::T_Identifier)) {
        std::string name = currentToken().text;
        consume(TokenType::T_Identifier);
        auto id = std::make_shared<Identifier>(name, line, column);
        return std::make_shared<VarExpr>(id, line, column, lookupVariable(name));
    }

    if (check(TokenType::T_ReadInteger)) {
        int line = currentToken().line;
        int column = currentToken().column;
        consume(TokenType::T_ReadInteger);
        consume(TokenType::T_Operator, "(");
        consume(TokenType::T_Operator, ")");
        return std::make_shared<ReadIntegerExpr>(line, column);
    }

    if (check(TokenType::T_ReadLine)) {
        int line = currentToken().line;
        int column = currentToken().column;
        consume(TokenType::T_ReadLine);
        consume(TokenType::T_Operator, "(");
        consume(TokenType::T_Operator, ")");
        return std::make_shared<ReadIntegerExpr>(line, column);
    }
    
    // Remove white space
    std::string srcLine = sourceCode.at(line-1);
    std::string::iterator end_pos = std::remove(srcLine.begin(), srcLine.end(), ' ');
    srcLine.erase(end_pos, srcLine.end());

    // Print syntax error
    std::string errorHighlight(srcLine.size(), '^');
    std::cout << std::endl << "*** Error line " << line << "." << std::endl 
              << sourceCode.at(line-1) << std::endl 
              << "    " << errorHighlight<< std::endl
              << "*** syntax error" << std::endl << std::endl;
    
    throw std::runtime_error("Syntax error");
}

// Usage example:
// std::vector<Token> tokens = lexer.tokenize(sourceCode);
// ASTBuilder builder(tokens);
// std::shared_ptr<Program> ast = builder.buildAST();
// ast->print(); // Print the AST for debugging