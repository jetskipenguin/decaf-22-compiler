#include "ASTNodes.h"

// Node implementation
Node::Node(int line, int column) : line(line), column(column) {}

// ASTNodeType implementations
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
    if (kind == Null) return true;  // null can be assigned to any reference type
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
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "Type: " << typeName() << std::endl;
}

// Identifier implementation
Identifier::Identifier(const std::string& name, int line, int column)
    : Node(line, column), name(name) {}

void Identifier::print(int indent) const {
    std::string indentStr(indent-3, ' ');
    std::cout << "  " << this->line << indentStr << "Identifier: " << name << std::endl;
}

// Literal expressions
IntLiteral::IntLiteral(int value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* IntLiteral::getType() const { 
    return ASTNodeType::intType; 
}

void IntLiteral::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "IntLiteral: " << value << std::endl;
}

DoubleLiteral::DoubleLiteral(double value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* DoubleLiteral::getType() const { 
    return ASTNodeType::doubleType; 
}

void DoubleLiteral::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "DoubleLiteral: " << value << std::endl;
}

BoolLiteral::BoolLiteral(bool value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* BoolLiteral::getType() const { 
    return ASTNodeType::boolType; 
}

void BoolLiteral::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "BoolLiteral: " << (value ? "true" : "false") << std::endl;
}

StringLiteral::StringLiteral(const std::string& value, int line, int column)
    : LiteralExpr(line, column), value(value) {}

ASTNodeType* StringLiteral::getType() const { 
    return ASTNodeType::stringType; 
}

void StringLiteral::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "StringConstant: " << value << std::endl;
}

NullLiteral::NullLiteral(int line, int column)
    : LiteralExpr(line, column) {}

ASTNodeType* NullLiteral::getType() const { 
    return ASTNodeType::nullType; 
}

void NullLiteral::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "NullLiteral" << std::endl;
}

// Variable reference
VarExpr::VarExpr(std::shared_ptr<Identifier> id, int line, int column)
    : Expr(line, column), id(id), varType(nullptr) {}

ASTNodeType* VarExpr::getType() const { 
    return varType ? varType : ASTNodeType::errorType; 
}

void VarExpr::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "VarExpr: " << std::endl;
    id->print(indent +3);
}

// Binary operation expression
BinaryExpr::BinaryExpr(BinaryOp op, std::shared_ptr<Expr> left, 
                      std::shared_ptr<Expr> right, int line, int column)
    : Expr(line, column), op(op), left(left), right(right) {}

ASTNodeType* BinaryExpr::getType() const {
    ASTNodeType* leftType = left->getType();
    ASTNodeType* rightType = right->getType();

    if (leftType->isError() || rightType->isError()) {
        return ASTNodeType::errorType;
    }

    // Handle numeric promotions
    if (leftType->isNumeric() && rightType->isNumeric()) {
        if (leftType->kind == ASTNodeType::Double || 
            rightType->kind == ASTNodeType::Double) {
            return ASTNodeType::doubleType;
        }
        return ASTNodeType::intType;
    }

    // Handle boolean operations
    if (op == And || op == Or) {
        return ASTNodeType::boolType;
    }

    // Handle comparisons
    if (op == Equal || op == NotEqual || 
        op == Less || op == LessEqual || 
        op == Greater || op == GreaterEqual) {
        return ASTNodeType::boolType;
    }

    return ASTNodeType::errorType;
}

void BinaryExpr::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::string opStr;
    
    switch (op) {
        case Plus: opStr = "+"; break;
        case Minus: opStr = "-"; break;
        case Multiply: opStr = "*"; break;
        case Divide: opStr = "/"; break;
        case Modulo: opStr = "%"; break;
        case Less: opStr = "<"; break;
        case LessEqual: opStr = "<="; break;
        case Greater: opStr = ">"; break;
        case GreaterEqual: opStr = ">="; break;
        case Equal: opStr = "=="; break;
        case NotEqual: opStr = "!="; break;
        case And: opStr = "&&"; break;
        case Or: opStr = "||"; break;
    }
    
    std::cout << indentStr << "ArithmeticExpr: " << opStr << std::endl;
    left->print(indent +3);
    right->print(indent +3);
}

// Unary operation expression
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
    std::string indentStr(indent, ' ');
    std::string opStr = (op == Minus) ? "-" : "!";
    std::cout << indentStr << "UnaryExpr: " << opStr << std::endl;
    expr->print(indent +3);
}

// Function call expression
CallExpr::CallExpr(std::shared_ptr<Identifier> id, int line, int column)
    : Expr(line, column), id(id), returnType(nullptr) {}

void CallExpr::addArg(std::shared_ptr<Expr> arg) {
    args.push_back(arg);
}

ASTNodeType* CallExpr::getType() const { 
    return returnType ? returnType : ASTNodeType::errorType; 
}

void CallExpr::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "CallExpr: " << std::endl;
    id->print(indent +3);
    std::cout << indentStr << "  Args: " << std::endl;
    for (const auto& arg : args) {
        arg->print(indent + 4);
    }
}

// Assignment expression
AssignExpr::AssignExpr(std::shared_ptr<Expr> left, 
                      std::shared_ptr<Expr> right, int line, int column)
    : Expr(line, column), left(left), right(right) {}

ASTNodeType* AssignExpr::getType() const { 
    return left->getType(); 
}

void AssignExpr::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "AssignExpr: " << std::endl;
    std::cout << indentStr << "  Left: " << std::endl;
    left->print(indent + 4);
    std::cout << indentStr << "  Right: " << std::endl;
    right->print(indent + 4);
}

// Statement implementations
ExprStmt::ExprStmt(std::shared_ptr<Expr> expr, int line, int column)
    : Stmt(line, column), expr(expr) {}

void ExprStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "ExprStmt: " << std::endl;
    expr->print(indent +3);
}

BlockStmt::BlockStmt(int line, int column) : Stmt(line, column) {}

void BlockStmt::addStmt(std::shared_ptr<Stmt> stmt) {
    stmts.push_back(stmt);
}

void BlockStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "(body) StmtBlock: " << std::endl;
    for (const auto& stmt : stmts) {
        stmt->print(indent + 3);
    }
}

IfStmt::IfStmt(std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> thenStmt,
               std::shared_ptr<Stmt> elseStmt, int line, int column)
    : Stmt(line, column), cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {}

void IfStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "IfStmt: " << std::endl;
    std::cout << indentStr << "  Condition: " << std::endl;
    cond->print(indent + 4);
    std::cout << indentStr << "  Then: " << std::endl;
    thenStmt->print(indent + 4);
    if (elseStmt) {
        std::cout << indentStr << "  Else: " << std::endl;
        elseStmt->print(indent + 4);
    }
}

WhileStmt::WhileStmt(std::shared_ptr<Expr> cond, 
                     std::shared_ptr<Stmt> body, int line, int column)
    : Stmt(line, column), cond(cond), body(body) {}

void WhileStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "WhileStmt: " << std::endl;
    std::cout << indentStr << "  Condition: " << std::endl;
    cond->print(indent + 4);
    std::cout << indentStr << "  Body: " << std::endl;
    body->print(indent + 4);
}

ForStmt::ForStmt(std::shared_ptr<Expr> init, std::shared_ptr<Expr> cond,
                 std::shared_ptr<Expr> update, std::shared_ptr<Stmt> body,
                 int line, int column)
    : Stmt(line, column), init(init), cond(cond), 
      update(update), body(body) {}

void ForStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "ForStmt: " << std::endl;
    if (init) {
        std::cout << indentStr << "  Init: " << std::endl;
        init->print(indent + 4);
    }
    if (cond) {
        std::cout << indentStr << "  Condition: " << std::endl;
        cond->print(indent + 4);
    }
    if (update) {
        std::cout << indentStr << "  Update: " << std::endl;
        update->print(indent + 4);
    }
    std::cout << indentStr << "  Body: " << std::endl;
    body->print(indent + 4);
}

ReturnStmt::ReturnStmt(std::shared_ptr<Expr> expr, int line, int column)
    : Stmt(line, column), expr(expr) {}

void ReturnStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "ReturnStmt: " << std::endl;
    if (expr) {
        expr->print(indent +3);
    }
}

BreakStmt::BreakStmt(int line, int column) : Stmt(line, column) {}

void BreakStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "BreakStmt" << std::endl;
}

PrintStmt::PrintStmt(int line, int column) : Stmt(line, column) {}

void PrintStmt::addArg(std::shared_ptr<Expr> arg) {
    args.push_back(arg);
}

void PrintStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "PrintStmt: " << std::endl;

    std::cout << "  " << this->line << indentStr << "(args) ";
    for (const auto& arg : args) {
        arg->print(0);
    }
}

// Declaration implementations
VarDecl::VarDecl(ASTNodeType* type, std::shared_ptr<Identifier> id,
                 std::shared_ptr<Expr> init, int line, int column)
    : Decl(line, column), type(type), id(id), init(init) {}

void VarDecl::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "VarDecl: " << std::endl;
    type->print(indent +3);
    id->print(indent +3);
    if (init) {
        std::cout << indentStr << "  Init: " << std::endl;
        init->print(indent + 4);
    }
}

// Implement VarDeclStmt
VarDeclStmt::VarDeclStmt(std::shared_ptr<VarDecl> varDecl, int line, int column)
    : Stmt(line, column), varDecl(varDecl) {}

void VarDeclStmt::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << indentStr << "VarDeclStmt:" << std::endl;
    varDecl->print(indent + 3);
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
    std::string indentStr(indent-3, ' ');
    std::cout << "  " << this->line << indentStr << "FnDecl: " << std::endl;
    
    std::cout << std::string(indent +3, ' ') 
              << "(return type) Type: " << returnType->typeName() 
              << std::endl;
    
    id->print(indent +3);
    
    if (formals.size() > 0) {
        for (const auto& formal : formals) {
            std::cout << "  " << line << indentStr << "(formals) VarDecl:" << std::endl ;
            formal->type->print(indent + 2);
            formal->id->print(indent + 2);
        }
    }

    if (body) {
        body->print(indent +3);
    }
}
// Root AST node
ASTRootNode::ASTRootNode(int line, int column) : Node(line, column) {}

void ASTRootNode::addDecl(std::shared_ptr<Decl> decl) {
    decls.push_back(decl);
}

void ASTRootNode::print(int indent) const {
    std::string indentStr(indent, ' ');
    std::cout << std::endl;
    std::cout << indentStr << "Program: " << std::endl;
    for (const auto& decl : decls) {
        decl->print(indent +3);
    }
}