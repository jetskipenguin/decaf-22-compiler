#include "ASTNodes.h"
#include <iostream>
#include <string>

std::vector<std::string> SourceInfo::sourceCode;

Identifier::Identifier(const std::string& name, int line, int column)
    : Node(line, column), name(name) {}

void Identifier::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "Identifier: " << name << std::endl;
}

bool Expr::check(SymbolTable &table, int blockLevel) {
    return true;
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

bool VarExpr::check(SymbolTable &table, int blockLevel) {
    if(table.lookup(id->name, blockLevel) == nullptr) {
        std::cout << "*** No declaration for variable '" << id->name << "' found" << std::endl;
        return false;
    }
    return true;
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

std::string BinaryExpr::getOpAsString() {
    switch(this->op) {
    case BinaryOp::And:
        return "&&";
    case BinaryOp::Or:
        return "||";
    case BinaryOp::NotEqual:
        return "!=";
    case BinaryOp::Equal:
        return "==";
    case BinaryOp::GreaterEqual:
        return ">=";
    case BinaryOp::Greater:
        return ">";
    case BinaryOp::LessEqual:
        return "<=";
    case BinaryOp::Less:
        return "<";
    case BinaryOp::Modulo:
        return "%";
    case BinaryOp::Divide:
        return "/";
    case BinaryOp::Multiply:
        return "*";
    case BinaryOp::Minus:
        return "-";
    case BinaryOp::Plus:
        return "+";
    }
    return "";
}

bool BinaryExpr::check(SymbolTable &table, int blockLevel) {
    std::cout << "In binary expr" << std::endl;

    this->right->check(table, blockLevel);
    this->left->check(table, blockLevel);

    ASTNodeType* leftType = this->left->getType();
    ASTNodeType* rightType = this->right->getType();

    if(!isValidOperandForGivenTypes()) {
        std::string indentStr(this->column+1, ' ');
        std::cout << "*** Error line " << this->line << "." << std::endl;
        std::cout << SourceInfo::sourceCode.at(this->line-1) << std::endl;
        std::cout << indentStr << "^" << std::endl;
        std::cout << "*** Incompatible operands: " << leftType->typeName() << " " << this->getOpAsString() << " " << rightType->typeName() << std::endl;
        std::cout << std::endl;
        return false;
    }
    return true;
}

bool BinaryExpr::isValidOperandForGivenTypes() {
    ASTNodeType* leftType = this->left->getType();
    ASTNodeType* rightType = this->right->getType();
    
    if(this->op == BinaryOp::Minus) {
        if(leftType == ASTNodeType::intType && rightType == ASTNodeType::doubleType) {
            return false;
        }
    }

    if(this->op == BinaryOp::Divide) {
        if(leftType == ASTNodeType::doubleType && rightType == ASTNodeType::intType) {
            return false;
        }
    }

    return true;
}

ASTNodeType* BinaryExpr::getType() const {
    ASTNodeType* leftType = left->getType();
    ASTNodeType* rightType = right->getType();

    if (leftType->isError() || rightType->isError()) {
        return ASTNodeType::errorType;
    }

    if (leftType->isNumeric() && rightType->isNumeric()) {
        if (leftType->kind == TypeKind::Double || 
            rightType->kind == TypeKind::Double) {
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

bool UnaryExpr::check(SymbolTable &table, int blockLevel) {
    return this->expr->check(table, blockLevel);
}

ASTNodeType* UnaryExpr::getType() const {
    ASTNodeType* exprType = expr->getType();
    if (exprType->isError()) {
        return ASTNodeType::errorType;
    }

    switch (op) {
        case Minus:
            return exprType->isNumeric() ? exprType : ASTNodeType::errorType;
        case Not:
            return (exprType->kind == TypeKind::Bool) ? 
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


bool CallExpr::check(SymbolTable &table, int blockLevel) {
    // if(table.lookup(id->name, blockLevel) == nullptr) {
    //     std::cout << "*** No declaration for Function '" << id->name << "' found" << std::endl;
    // } //TODO: figure out a function table
    return true;
}

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

bool AssignExpr::check(SymbolTable &table, int blockLevel) {

    std::cout << "In assignment EXPR" << std::endl;

    if(!this->right->check(table, blockLevel)) {
        return false;
    }

    TypeKind rightType = this->right->getType()->kind;
    TypeKind leftType = this->left->getType()->kind;

    if(leftType != rightType) {
        std::string indentStr(this->column+1, ' ');
        std::cout << "*** Error line " << this->line << "." << std::endl;
        std::cout << SourceInfo::sourceCode.at(this->line-1) << std::endl;
        std::cout << indentStr << "^" << std::endl;
        std::cout << "*** Incompatible operands: " << this->left->getType()->typeName() << " = " << this->right->getType()->typeName() << std::endl;
        std::cout << std::endl;
        return false;
    }

    return true;
}

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

void ExprStmt::check(SymbolTable &table, int blockLevel) {
    expr->check(table, blockLevel);
}

void ExprStmt::print(int indent) const {
    expr->print(indent);
}

BlockStmt::BlockStmt(int line, int column) : Stmt(line, column) {}

void BlockStmt::check(SymbolTable &table, int blockLevel) {
    for(auto &stmt : this->stmts) {
        stmt->check(table, blockLevel);
    }
}

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

void IfStmt::check(SymbolTable &table, int blockLevel) {
    cond->check(table, blockLevel);
    thenStmt->check(table, blockLevel);
    elseStmt->check(table, blockLevel);
}

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

void WhileStmt::check(SymbolTable &table, int blockLevel) {
    this->cond->check(table, blockLevel);
    this->body->check(table, blockLevel);
}

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

void ForStmt::check(SymbolTable &table, int blockLevel) {
    init->check(table, blockLevel);
    cond->check(table, blockLevel);
    update->check(table, blockLevel);
    body->check(table, blockLevel);
}

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

void ReturnStmt::check(SymbolTable &table, int blockLevel) {
    this->expr->check(table, blockLevel);
}

void ReturnStmt::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "ReturnStmt: " << std::endl;
    if (expr) {
        expr->print(indent + 3);
    }
}

BreakStmt::BreakStmt(int line, int column) : Stmt(line, column) {}

void BreakStmt::check(SymbolTable &table, int blockLevel) {
    return;
}

void BreakStmt::print(int indent) const {
    std::cout << std::string(indent, ' ') << "BreakStmt" << std::endl;
}

PrintStmt::PrintStmt(int line, int column) : Stmt(line, column) {}

void PrintStmt::check(SymbolTable &table, int blockLevel) {
    for( auto &expr : this->args) {
        expr->check(table, blockLevel);
    }
}

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
    : Decl(line, column), type(type), init(init) {
        identifier = id;
    }

void VarDecl::check(SymbolTable &table, int blockLevel) {
    std::cout << "Checking var decl..." << std::endl;
    if(this->init) {
        this->init->check(table, blockLevel);
    }
}

void VarDecl::print(int indent) const {
    std::cout << "  " << line << std::string(indent, ' ') << "VarDecl: " << std::endl;
    type->print(indent + 6);
    identifier->print(indent + 3);
    if (init) {
        std::cout << "  " << line << std::string(indent-3, ' ') << "   Init: " << std::endl;
        init->print(indent + 4);
    }
}

VarDeclStmt::VarDeclStmt(std::shared_ptr<VarDecl> varDecl, int line, int column)
    : Stmt(line, column), varDecl(varDecl) {}

void VarDeclStmt::check(SymbolTable &table, int blockLevel) {
    this->varDecl->check(table, blockLevel);
}

void VarDeclStmt::print(int indent) const {
    varDecl->print(indent);
}

FunctionDecl::FunctionDecl(ASTNodeType* returnType, 
                          std::shared_ptr<Identifier> id, int line, int column)
    : Decl(line, column), returnType(returnType) {
        identifier = id;
    }

void FunctionDecl::check(SymbolTable &table, int blockLevel) {

    std::cout << "Checking func decl..." << std::endl;

    for(auto &formal : this->formals) {
        formal->check(table, blockLevel);
    }

    this->body->check(table, blockLevel);
}

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
    identifier->print(indent+3);
    
    for (const auto& formal : formals) {
        std::cout << "  " << formal->line  << std::string(indent+3, ' ')
                  << "(formals) VarDecl: " << std::endl;
        formal->type->print(indent + 9);
        formal->identifier->print(indent + 6);
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