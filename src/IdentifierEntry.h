#pragma once
#include <string>
#include "ASTNodes.h"

enum Scope {
    GLOBAL, LOCAL, PARAMETER
};

struct IdentifierEntry {
    std::string name;
    TypeKind dataType;
    int blockLevel;
};