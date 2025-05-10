#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <vector>
#include "IdentifierEntry.h"

// Block level 1 = global vars
// Block level 2 = params and local vars
class SymbolTable {
    private:
        std::vector<std::unordered_map<std::string, IdentifierEntry>> tables;
    public:
        void install(std::string, int blockLevel);
        IdentifierEntry lookup(std::string, int blockLevel);
};