#pragma once

#include <string>
#include <vector>
#include "IdentifierEntry.h"

// Block level 1 = global vars
// Block level 2 = params and local vars
class SymbolTable {
    public:
        IdentifierEntry install(std::string, int blockLevel);
        IdentifierEntry lookup(std::string, int blockLevel);
};