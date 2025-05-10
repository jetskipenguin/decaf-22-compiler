#include "SymbolTable.h"

void SymbolTable::install(std::string symbolName, int blockLevel) {
    IdentifierEntry entryToAdd;
    entryToAdd.blockLevel = blockLevel;
    entryToAdd.name = symbolName;
    this->tables.at(blockLevel).insert({symbolName, entryToAdd});
}

IdentifierEntry SymbolTable::lookup(std::string symbolName, int blockLevel) {
    return this->tables.at(blockLevel).at(symbolName);
}