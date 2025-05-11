#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <vector>
#include <memory>
#include "IdentifierEntry.h"

// Block level 1 = global vars
// Block level 2 = params and local vars
class SymbolTable {
    private:
        std::vector<std::unordered_map<std::string, IdentifierEntry>> tables;
    public:
        // TODO: update to not have maps and have the block levels already initialized
        SymbolTable() {
            tables.emplace_back();
        }
        void install(std::string, int blockLevel);
        std::shared_ptr<IdentifierEntry> lookup(std::string, int blockLevel);
};