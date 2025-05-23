#include "ASTNodes.h"
#include <iostream>
#include <string>

Node::Node(int line, int column) : line(line), column(column) {}

ASTNodeType* ASTNodeType::voidType = new ASTNodeType(ASTNodeType::Void);
ASTNodeType* ASTNodeType::intType = new ASTNodeType(ASTNodeType::Int);
ASTNodeType* ASTNodeType::doubleType = new ASTNodeType(ASTNodeType::Double);
ASTNodeType* ASTNodeType::boolType = new ASTNodeType(ASTNodeType::Bool);
ASTNodeType* ASTNodeType::stringType = new ASTNodeType(ASTNodeType::String);
ASTNodeType* ASTNodeType::nullType = new ASTNodeType(ASTNodeType::Null);
ASTNodeType* ASTNodeType::errorType = new ASTNodeType(ASTNodeType::Error);

ASTNodeType::ASTNodeType(TypeKind kind, int line, int column) 
    : Node(line, column), kind(kind) {}

bool ASTNodeType::isError() const { return kind == Error; }
bool ASTNodeType::isVoid() const { return kind == Void; }
bool ASTNodeType::isNumeric() const { return kind == Int || kind == Double; }

bool ASTNodeType::isEquivalentTo(const ASTNodeType* other) const {
    return kind == other->kind;
}

bool ASTNodeType::isAssignableTo(const ASTNodeType* other) const {
    if (kind == Null) return true;
    if (kind == Error || other->kind == Error) return false;
    return isEquivalentTo(other);
}

const char* ASTNodeType::typeName() const {
    switch (kind) {
        case Void: return "void";
        case Int: return "int";
        case Double: return "double";
        case Bool: return "bool";
        case String: return "string";
        case Null: return "null";
        case Error: return "error";
        default: return "unknown";
    }
}

void ASTNodeType::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Type: " << typeName() << std::endl;
}

Identifier::Identifier(const std::string& name, int line, int column)
    : Node(line, column), name(name) {}

void Identifier::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "Identifier: " << name << std::endl;
}

IntLiteral::IntLiteral(int value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* IntLiteral::getType() const { 
    return ASTNodeType::intType; 
}

void IntLiteral::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "IntConstant: " << value << std::endl;
}

DoubleLiteral::DoubleLiteral(double value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* DoubleLiteral::getType() const { 
    return ASTNodeType::doubleType; 
}

void DoubleLiteral::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "DoubleConstant: " << value << std::endl;
}

BoolLiteral::BoolLiteral(bool value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* BoolLiteral::getType() const { 
    return ASTNodeType::boolType; 
}

void BoolLiteral::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "BoolConstant: " << (value ? "true" : "false") << std::endl;
}

StringLiteral::StringLiteral(const std::string& value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* StringLiteral::getType() const { 
    return ASTNodeType::stringType; 
}

void StringLiteral::print(int indent) const {
    if (isArgument) {
        std::cout << "  " << line << "         (args) StringConstant: " << value << std::endl;
    } else {
        std::cout << std::string(indent, ' ') << "StringConstant: " << value << std::endl;
    }
}

NullLiteral::NullLiteral(int line, int column)
    : LiteralExpr(line, column) {}

ASTNodeType* NullLiteral::getType() const { 
    return ASTNodeType::nullType; 
}

void NullLiteral::print(int indent) const {
    std::cout << std::string(indent, ' ') << "NullLiteral" << std::endl;
}

VarExpr::VarExpr(std::shared_ptr<Identifier> id, int line, int column, ASTNodeType* type)
    : Expr(line, column), id(id), varType(type) {}

ASTNodeType* VarExpr::getType() const { 
    return varType; 
}

void VarExpr::print(int indent) const {
    if (isArgument) {
        std::cout << "  " << line << std::string(indent, ' ') << "(actuals) FieldAccess: " << std::endl;
    } else {
        std::cout << "  " << line << std::string(indent, ' ') << "FieldAccess: " << std::endl;
    }
    id->print(indent + 3);
}

BinaryExpr::BinaryExpr(BinaryOp op, std::shared_ptr<Expr> left, 
                      std::shared_ptr<Expr> right, int line, int column)
    : Expr(line, column), op(op), left(left), right(right) {}

ASTNodeType* BinaryExpr::getType() const {
    ASTNodeType* leftType = left->getType();
    ASTNodeType* rightType = right->getType();

    if (leftType->isError() || rightType->isError()) {
        return ASTNodeType::errorType;
    }

    if (leftType->isNumeric() && rightType->isNumeric()) {
        if (leftType->kind == ASTNodeType::Double || 
            rightType->kind == ASTNodeType::Double) {
            return ASTNodeType::doubleType;
        }
        return ASTNodeType::intType;
    }

    if (op == And || op == Or) {
        return ASTNodeType::boolType;
    }

    if (op == Equal || op == NotEqual || 
        op == Less || op == LessEqual || 
        op == Greater || op == GreaterEqual) {
        return ASTNodeType::boolType;
    }

    return ASTNodeType::errorType;
}

void BinaryExpr::print(int indent) const {
    std::string exprType;
    switch (op) {
        case Plus: case Minus: case Multiply: case Divide: case Modulo:
            exprType = "ArithmeticExpr"; break;
        case Less: case LessEqual: case Greater: case GreaterEqual:
            exprType = "RelationalExpr"; break;
        case Equal: case NotEqual:
            exprType = "EqualityExpr"; break;
        case And: case Or:
            exprType = "LogicalExpr"; break;
        default: exprType = "BinaryExpr";
    }
    
    std::cout << "  " << line << std::string(indent, ' ') << exprType << ": " << std::endl;
    left->print(indent + 3);
    
    std::cout << "  " << line << std::string(indent, ' ') << "  Operator: ";
    switch(op) { 
        case Plus: std::cout << "+"; break;
        case Minus: std::cout << "-"; break;
        case Multiply: std::cout << "*"; break;
        case Divide: std::cout << "/"; break;
        case Modulo: std::cout << "%"; break;
        case Less: std::cout << "<"; break;
        case LessEqual: std::cout << "<="; break;
        case Greater: std::cout << ">"; break;
        case GreaterEqual: std::cout << ">="; break;
        case Equal: std::cout << "=="; break;
        case NotEqual: std::cout << "!="; break;
        case And: std::cout << "&&"; break;
        case Or: std::cout << "||"; break;
        default: std::cout << "unknown_op"; break;
    }
    std::cout << std::endl;
    
    right->print(indent + 3);
}

UnaryExpr::UnaryExpr(UnaryOp op, std::shared_ptr<Expr> expr, int line, int column)
    : Expr(line, column), op(op), expr(expr) {}

ASTNodeType* UnaryExpr::getType() const {
    ASTNodeType* exprType = expr->getType();
    if (exprType->isError()) {
        return ASTNodeType::errorType;
    }

    switch (op) {
        case Minus:
            return exprType->isNumeric() ? exprType : ASTNodeType::errorType;
        case Not:
            return (exprType->kind == ASTNodeType::Bool) ? 
                   ASTNodeType::boolType : ASTNodeType::errorType;
        default:
            return ASTNodeType::errorType;
    }
}

void UnaryExpr::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "LogicalExpr: " << std::endl;
    std::cout << "  " << line << std::string(indent, ' ') << "  Operator: " 
              << (op == Minus ? "-" : "!") << std::endl;
    expr->print(indent + 3);
}

CallExpr::CallExpr(std::shared_ptr<Identifier> id, int line, int column)
    : Expr(line, column), id(id), returnType(nullptr) {}

void CallExpr::addArg(std::shared_ptr<Expr> arg) {
    arg->setIsArgument(true);
    args.push_back(arg);
}

ASTNodeType* CallExpr::getType() const { 
    return returnType ? returnType : ASTNodeType::errorType; 
}

void CallExpr::print(int indent) const {
    if (isArgument) {
        std::cout <<  "  " << line << std::string(indent, ' ') << "(args) Call:" << std::endl;
    } else {
        std::cout <<  "  " << line << std::string(indent, ' ') << "Call:" << std::endl;
    }
    id->print(indent+3);
    for (const auto& arg : args) {
        arg->print(indent + 3);
    }
}

AssignExpr::AssignExpr(std::shared_ptr<Expr> left, 
                      std::shared_ptr<Expr> right, int line, int column)
    : Expr(line, column), left(left), right(right) {}

ASTNodeType* AssignExpr::getType() const { 
    return left->getType(); 
}

void AssignExpr::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "AssignExpr: " << std::endl;
    left->print(indent + 3);
    std::cout << "  " << line << std::string(indent, ' ') << "   Operator: =" << std::endl;
    right->print(indent + 3);
}

ExprStmt::ExprStmt(std::shared_ptr<Expr> expr, int line, int column)
    : Stmt(line, column), expr(expr) {}

void ExprStmt::print(int indent) const {
    expr->print(indent);
}

BlockStmt::BlockStmt(int line, int column) : Stmt(line, column) {}

void BlockStmt::addStmt(std::shared_ptr<Stmt> stmt) {
    stmts.push_back(stmt);
}

void BlockStmt::print(int indent) const {
    std::cout << std::string(indent+3, ' ') << "(body) StmtBlock: " << std::endl;
    for (const auto& stmt : stmts) {
        stmt->print(indent + 3);
    }
}

IfStmt::IfStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> thenStmt,
               std::shared_ptr<Stmt> elseStmt, int line, int column)
    : Stmt(line, column), cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {}

void IfStmt::print(int indent) const {
    std::cout << std::string(indent, ' ') << "IfStmt: " << std::endl;
    std::cout << std::string(indent, ' ') << "  Condition: " << std::endl;
    cond->print(indent + 4);
    std::cout << std::string(indent, ' ') << "  Then: " << std::endl;
    thenStmt->print(indent + 4);
    if (elseStmt) {
        std::cout << std::string(indent, ' ') << "  Else: " << std::endl;
        elseStmt->print(indent + 4);
    }
}

WhileStmt::WhileStmt(std::shared_ptr<Expr> cond, 
                     std::shared_ptr<Stmt> body, int line, int column)
    : Stmt(line, column), cond(cond), body(body) {}

void WhileStmt::print(int indent) const {
    std::cout << std::string(indent, ' ') << "WhileStmt: " << std::endl;
    std::cout << std::string(indent, ' ') << "  Condition: " << std::endl;
    cond->print(indent + 4);
    std::cout << std::string(indent, ' ') << "  Body: " << std::endl;
    body->print(indent + 4);
}

ForStmt::ForStmt(std::shared_ptr<Expr> init, std::shared_ptr<Expr> cond,
                 std::shared_ptr<Expr> update, std::shared_ptr<Stmt> body,
                 int line, int column)
    : Stmt(line, column), init(init), cond(cond), 
      update(update), body(body) {}

void ForStmt::print(int indent) const {
    std::cout << std::string(indent, ' ') << "ForStmt: " << std::endl;
    if (init) {
        std::cout << std::string(indent, ' ') << "  Init: " << std::endl;
        init->print(indent + 4);
    }
    if (cond) {
        std::cout << std::string(indent, ' ') << "  Condition: " << std::endl;
        cond->print(indent + 4);
    }
    if (update) {
        std::cout << std::string(indent, ' ') << "  Update: " << std::endl;
        update->print(indent + 4);
    }
    std::cout << std::string(indent, ' ') << "  Body: " << std::endl;
    body->print(indent + 4);
}

ReturnStmt::ReturnStmt(std::shared_ptr<Expr> expr, int line, int column)
    : Stmt(line, column), expr(expr) {}

void ReturnStmt::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "ReturnStmt: " << std::endl;
    if (expr) {
        expr->print(indent + 3);
    }
}

BreakStmt::BreakStmt(int line, int column) : Stmt(line, column) {}

void BreakStmt::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BreakStmt" << std::endl;
}

PrintStmt::PrintStmt(int line, int column) : Stmt(line, column) {}

void PrintStmt::addArg(std::shared_ptr<Expr> arg) {
    arg->setIsArgument(true);
    args.push_back(arg);
}

void PrintStmt::print(int indent) const {
    std::cout << std::string(indent+4, ' ') << "PrintStmt: " << std::endl;
    for (const auto& arg : args) {
        arg->print(indent + 3);
    }
}

ReadIntegerExpr::ReadIntegerExpr(int line, int column) : Expr(line, column) {}

ASTNodeType* ReadIntegerExpr::getType() const { 
    return ASTNodeType::intType; 
}

void ReadIntegerExpr::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "ReadIntegerExpr: " << std::endl;
}

VarDecl::VarDecl(ASTNodeType* type, std::shared_ptr<Identifier> id,
                 std::shared_ptr<Expr> init, int line, int column)
    : Decl(line, column), type(type), id(id), init(init) {}

void VarDecl::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "VarDecl: " << std::endl;
    type->print(indent + 6);
    id->print(indent + 3);
    if (init) {
        std::cout << "  " << line << std::string(indent-3, ' ') << "   Init: " << std::endl;
        init->print(indent + 4);
    }
}

VarDeclStmt::VarDeclStmt(std::shared_ptr<VarDecl> varDecl, int line, int column)
    : Stmt(line, column), varDecl(varDecl) {}

void VarDeclStmt::print(int indent) const {
    varDecl->print(indent);
}

FunctionDecl::FunctionDecl(ASTNodeType* returnType, 
                          std::shared_ptr<Identifier> id, int line, int column)
    : Decl(line, column), returnType(returnType), id(id) {}

void FunctionDecl::addFormal(std::shared_ptr<VarDecl> formal) {
    formals.push_back(formal);
}

void FunctionDecl::setBody(std::shared_ptr<BlockStmt> functionBody) {
    body = functionBody;
}

void FunctionDecl::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') <<  "FnDecl: " << std::endl;
    std::cout << std::string(indent+6, ' ') << "(return type) Type: " 
              << returnType->typeName() << std::endl;
    id->print(indent+3);
    
    for (const auto& formal : formals) {
        std::cout << "  " << formal->line  << std::string(indent+3, ' ')
                  << "(formals) VarDecl: " << std::endl;
        formal->type->print(indent + 9);
        formal->id->print(indent + 6);
    }

    if (body) {
        body->print(indent+3);
    }
}

ASTRootNode::ASTRootNode(int line, int column) : Node(line, column) {}

void ASTRootNode::addDecl(std::shared_ptr<Decl> decl) {
    decls.push_back(decl);
}

void ASTRootNode::print(int indent) const {
    std::cout << std::endl << "   Program: " << std::endl;
    for (const auto& decl : decls) {
        decl->print(indent + 3);
    }
}