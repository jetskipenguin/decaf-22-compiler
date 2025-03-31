#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iostream>

#include "ASTNodes.h"
#include "Scanner.h"

// Forward declarations
class ASTRootNode;
class Decl;
class FunctionDecl;
class VarDecl;
class Stmt;
class BlockStmt;
class Expr;
class Identifier;
class ASTNodeType;

class ASTBuilder {
private:
    std::vector<Token> tokens;
    std::vector<std::string> sourceCode; // For logging purposes only
    size_t currentTokenIndex;
    bool verbose;

    // Token handling helpers
    Token currentToken() const;
    void nextToken();
    bool match(TokenType type);
    bool check(TokenType type) const;
    void consume(TokenType type);
    void consume(TokenType type, std::string character);

    // AST node parsing methods
    std::shared_ptr<ASTRootNode> parseProgram();
    std::shared_ptr<Decl> parseDecl();
    std::shared_ptr<FunctionDecl> parseFunctionDecl(ASTNodeType* returnType, std::shared_ptr<Identifier> id, int line, int column);
    std::shared_ptr<VarDecl> parseVarDeclAfterType(ASTNodeType* type, std::shared_ptr<Identifier> id, int line, int column);
    std::shared_ptr<VarDecl> parseVarDecl();
    ASTNodeType* parseType();
    std::shared_ptr<BlockStmt> parseBlock();
    std::shared_ptr<Stmt> parseStmt();
    std::shared_ptr<Stmt> parseIfStmt();
    std::shared_ptr<Stmt> parseWhileStmt();
    std::shared_ptr<Stmt> parseForStmt();
    std::shared_ptr<Stmt> parseReturnStmt();
    std::shared_ptr<Stmt> parseBreakStmt();
    std::shared_ptr<Stmt> parsePrintStmt();
    std::shared_ptr<Stmt> parseExprStmt();

    // Expression parsing methods (in order of precedence)
    std::shared_ptr<Expr> parseExpr();
    std::shared_ptr<Expr> parseAssignment();
    std::shared_ptr<Expr> parseLogicalOr();
    std::shared_ptr<Expr> parseLogicalAnd();
    std::shared_ptr<Expr> parseEquality();
    std::shared_ptr<Expr> parseRelational();
    std::shared_ptr<Expr> parseAdditive();
    std::shared_ptr<Expr> parseMultiplicative();
    std::shared_ptr<Expr> parseUnary();
    std::shared_ptr<Expr> parseCall();
    std::shared_ptr<Expr> parsePrimary();

public:
    explicit ASTBuilder(const std::vector<Token>& tokens, const std::vector<std::string>& sourceCode) {
        this->currentTokenIndex = 0;
        this->tokens = tokens;
        this->sourceCode = sourceCode;
        this->verbose = false;
    }

    explicit ASTBuilder(const std::vector<Token>& tokens, const std::vector<std::string>& sourceCode, bool verbose) {
        this->currentTokenIndex = 0;
        this->tokens = tokens;
        this->sourceCode = sourceCode;
        this->verbose = verbose;
    }

    std::shared_ptr<ASTRootNode> buildAST();
};
