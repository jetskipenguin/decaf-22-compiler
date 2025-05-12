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
        bool verbose;
        std::vector<std::unordered_map<std::string, IdentifierEntry>> variableTable;
        std::vector<std::unordered_map<std::string, IdentifierEntry>> functionTable; 
    public:
        SymbolTable(bool verbose);
        void installVariable(std::string, int blockLevel);
        void installFunction(std::string, int blockLevel);
        std::shared_ptr<IdentifierEntry> lookupVariable(std::string, int blockLevel);
        std::shared_ptr<IdentifierEntry> lookupFunction(std::string, int blockLevel);
        void clearBlockLevelTwo();
};