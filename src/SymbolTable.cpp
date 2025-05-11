#include <stdexcept>
#include "SymbolTable.h"

void SymbolTable::install(std::string symbolName, int blockLevel) {
    IdentifierEntry entryToAdd;
    entryToAdd.blockLevel = blockLevel;
    entryToAdd.name = symbolName;

    if(this->tables.at(blockLevel).count(symbolName)) {
        throw std::invalid_argument("Symbol name given is already installed in the symbol table");
    }

    this->tables.at(blockLevel).insert({symbolName, entryToAdd});
}

std::shared_ptr<IdentifierEntry> SymbolTable::lookup(std::string symbolName, int blockLevel) {
    return std::make_shared<IdentifierEntry>(this->tables.at(blockLevel).at(symbolName));
}