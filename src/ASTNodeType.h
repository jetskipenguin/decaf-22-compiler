#include <iostream>

enum TypeKind {
        Void, Int, Double, Bool, String, Null, Error
    };

class Node {
public:
    int line;
    int column;

    Node(int line = 0, int column = 0);
    virtual ~Node() = default;
    virtual void print(int indent = 0) const = 0;
};

class ASTNodeType : public Node {
public:
    TypeKind kind;

    ASTNodeType(TypeKind kind, int line = 0, int column = 0);
    bool isError() const;
    bool isVoid() const;
    bool isNumeric() const;
    bool isEquivalentTo(const ASTNodeType* other) const;
    bool isAssignableTo(const ASTNodeType* other) const;
    const char* typeName() const;
    void print(int indent = 0) const override;

    static ASTNodeType* voidType;
    static ASTNodeType* intType;
    static ASTNodeType* doubleType;
    static ASTNodeType* boolType;
    static ASTNodeType* stringType;
    static ASTNodeType* nullType;
    static ASTNodeType* errorType;
};