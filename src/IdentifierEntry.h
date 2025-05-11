#pragma once
#include <string>
#include "ASTNodeType.h"

enum Scope {
    GLOBAL, LOCAL, PARAMETER
};

struct IdentifierEntry {
    std::string name;
    ASTNodeType* type;
    int blockLevel;
};