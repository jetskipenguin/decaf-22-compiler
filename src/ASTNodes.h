#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Token.h"
#include "SymbolTable.h"

// Forward declarations
class ASTNodeType;
class Identifier;

class Expr : public Node {
protected:
    bool isArgument = false;
public:
    Expr(int line, int column) : Node(line, column) {}
    virtual ASTNodeType* getType() const = 0;
    void setIsArgument(bool isArg) { isArgument = isArg; }
    bool getIsArgument() const { return isArgument; }
};

class Identifier : public Node {
public:
    std::string name;
    Identifier(const std::string& name, int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class LiteralExpr : public Expr {
public:
    using Expr::Expr;
};

class IntLiteral : public LiteralExpr {
public:
    int value;
    IntLiteral(int value, int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class DoubleLiteral : public LiteralExpr {
public:
    double value;
    DoubleLiteral(double value, int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class BoolLiteral : public LiteralExpr {
public:
    bool value;
    BoolLiteral(bool value, int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class StringLiteral : public LiteralExpr {
public:
    std::string value;
    StringLiteral(const std::string& value, int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class NullLiteral : public LiteralExpr {
public:
    NullLiteral(int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class VarExpr : public Expr {
public:
    std::shared_ptr<Identifier> id;
    ASTNodeType* varType;

    VarExpr(std::shared_ptr<Identifier> id, int line = 0, int column = 0, ASTNodeType* type = ASTNodeType::errorType);
    ASTNodeType* getType() const override;
    void check(SymbolTable &table, int blockLevel);
    void print(int indent = 0) const override;
};

class BinaryExpr : public Expr {
public:
    enum BinaryOp {
        Plus, Minus, Multiply, Divide, Modulo,
        Less, LessEqual, Greater, GreaterEqual,
        Equal, NotEqual, And, Or
    };

    BinaryOp op;
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;

    BinaryExpr(BinaryOp op, std::shared_ptr<Expr> left, std::shared_ptr<Expr> right,
              int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class UnaryExpr : public Expr {
public:
    enum UnaryOp { Minus, Not };
    
    UnaryOp op;
    std::shared_ptr<Expr> expr;

    UnaryExpr(UnaryOp op, std::shared_ptr<Expr> expr, int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
};

class CallExpr : public Expr {
public:
    std::shared_ptr<Identifier> id;
    std::vector<std::shared_ptr<Expr>> args;
    ASTNodeType* returnType;

    CallExpr(std::shared_ptr<Identifier> id, int line = 0, int column = 0);
    void addArg(std::shared_ptr<Expr> arg);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
    void check(SymbolTable &table, int blockLevel);
};

class AssignExpr : public Expr {
public:
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;

    AssignExpr(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right,
              int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent = 0) const override;
    void check(SymbolTable &table, int blockLevel);
};

class Stmt : public Node {
public:
    using Node::Node;
};

class ExprStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;

    ExprStmt(std::shared_ptr<Expr> expr, int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class BlockStmt : public Stmt {
public:
    std::vector<std::shared_ptr<Stmt>> stmts;

    BlockStmt(int line = 0, int column = 0);
    void addStmt(std::shared_ptr<Stmt> stmt);
    void print(int indent = 0) const override;
};

class IfStmt : public Stmt {
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> thenStmt;
    std::shared_ptr<Stmt> elseStmt;

    IfStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> thenStmt,
           std::shared_ptr<Stmt> elseStmt = nullptr, int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class WhileStmt : public Stmt {
public:
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Stmt> body;

    WhileStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> body,
             int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class ForStmt : public Stmt {
public:
    std::shared_ptr<Expr> init;
    std::shared_ptr<Expr> cond;
    std::shared_ptr<Expr> update;
    std::shared_ptr<Stmt> body;

    ForStmt(std::shared_ptr<Expr> init, std::shared_ptr<Expr> cond,
            std::shared_ptr<Expr> update, std::shared_ptr<Stmt> body,
            int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class ReturnStmt : public Stmt {
public:
    std::shared_ptr<Expr> expr;

    ReturnStmt(std::shared_ptr<Expr> expr = nullptr, int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class BreakStmt : public Stmt {
public:
    BreakStmt(int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class PrintStmt : public Stmt {
public:
    std::vector<std::shared_ptr<Expr>> args;

    PrintStmt(int line = 0, int column = 0);
    void addArg(std::shared_ptr<Expr> arg);
    void print(int indent = 0) const override;
};

class ReadIntegerExpr : public Expr {
public:
    ReadIntegerExpr(int line = 0, int column = 0);
    ASTNodeType* getType() const override;
    void print(int indent) const override;
};

class Decl : public Node {
public:
    using Node::Node;
    std::shared_ptr<Identifier> identifier;
};

// Var declaration that includes assignment
class VarDecl : public Decl {
public:
    ASTNodeType* type;
    std::shared_ptr<Expr> init;

    VarDecl(ASTNodeType* type, std::shared_ptr<Identifier> id,
            std::shared_ptr<Expr> init = nullptr, int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

// Var declaration that doesn't include assignment
class VarDeclStmt : public Stmt {
public:
    std::shared_ptr<VarDecl> varDecl;
    VarDeclStmt(std::shared_ptr<VarDecl> varDecl, int line = 0, int column = 0);
    void print(int indent = 0) const override;
};

class FunctionDecl : public Decl {
public:
    ASTNodeType* returnType;
    std::vector<std::shared_ptr<VarDecl>> formals;
    std::shared_ptr<BlockStmt> body;

    FunctionDecl(ASTNodeType* returnType, std::shared_ptr<Identifier> id,
                 int line = 0, int column = 0);
    void addFormal(std::shared_ptr<VarDecl> formal);
    void setBody(std::shared_ptr<BlockStmt> functionBody);
    void print(int indent = 0) const override;
};

class ASTRootNode : public Node {
public:
    std::vector<std::shared_ptr<Decl>> decls;

    ASTRootNode(int line = 0, int column = 0);
    void addDecl(std::shared_ptr<Decl> decl);
    void print(int indent = 0) const override;
};