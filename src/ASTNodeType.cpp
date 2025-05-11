#include "ASTNodeType.h"

Node::Node(int line, int column) : line(line), column(column) {}

ASTNodeType* ASTNodeType::voidType = new ASTNodeType(TypeKind::Void);
ASTNodeType* ASTNodeType::intType = new ASTNodeType(TypeKind::Int);
ASTNodeType* ASTNodeType::doubleType = new ASTNodeType(TypeKind::Double);
ASTNodeType* ASTNodeType::boolType = new ASTNodeType(TypeKind::Bool);
ASTNodeType* ASTNodeType::stringType = new ASTNodeType(TypeKind::String);
ASTNodeType* ASTNodeType::nullType = new ASTNodeType(TypeKind::Null);
ASTNodeType* ASTNodeType::errorType = new ASTNodeType(TypeKind::Error);

ASTNodeType::ASTNodeType(TypeKind kind, int line, int column) 
    : Node(line, column), kind(kind) {}

bool ASTNodeType::isError() const { return kind == Error; }
bool ASTNodeType::isVoid() const { return kind == Void; }
bool ASTNodeType::isNumeric() const { return kind == Int || kind == Double; }

bool ASTNodeType::isEquivalentTo(const ASTNodeType* other) const {
    return this->kind == other->kind;
}

bool ASTNodeType::isAssignableTo(const ASTNodeType* other) const {
    if (kind == Null) return true;
    if (kind == Error || other->kind == Error) return false;
    if (isNumeric() && other->isNumeric()) return true;
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