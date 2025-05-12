#pragma once
#include <string>
#include "ASTNodeType.h"
#include "CodeGenerator.h"

enum Scope {
    GLOBAL, LOCAL, PARAMETER
};

struct IdentifierEntry {
    std::string name;
    ASTNodeType* type;
    int blockLevel;
    Register reg;
};

struct FunctionEntry {
    IdentifierEntry id;
    std::vector<ASTNodeType*> params;
};