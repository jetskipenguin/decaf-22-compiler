#include <stdexcept>
#include "SymbolTable.h"

SymbolTable::SymbolTable(bool verbose) {
    this->verbose = verbose;

    if(this->verbose) {
        std::cout << "Created symbol table" << std::endl;
    }
    
    for(int i = 0; i < 3; i++) {
        this->variableTable.emplace_back();
    }
}

void SymbolTable::install(std::string symbolName, int blockLevel) {

    if(this->verbose) {
        std::cout << "Installed symbol: " << symbolName << " at blockLevel: " << blockLevel << std::endl;
    }
    
    IdentifierEntry entryToAdd;
    entryToAdd.blockLevel = blockLevel;
    entryToAdd.name = symbolName;

    if(this->variableTable.at(blockLevel).count(symbolName)) {
        throw std::invalid_argument("Symbol name given is already installed in the symbol table");
    }

    this->variableTable.at(blockLevel).insert({symbolName, entryToAdd});
}

// TODO: if blockLevel == 2, try looking in blockLevel == 1 for global vars
std::shared_ptr<IdentifierEntry> SymbolTable::lookupVariable(std::string symbolName, int blockLevel) {
    if(this->verbose) {
        std::cout << "Looking up symbol: " << symbolName << " at blockLevel: " << blockLevel << std::endl;
    }

    try {
        return std::make_shared<IdentifierEntry>(this->variableTable.at(blockLevel).at(symbolName));
    }
    catch(std::out_of_range e) {
        return nullptr;
    }
}


void SymbolTable::clearBlockLevelTwo() {
    if(this->verbose) {
        std::cout << "Clearing block level 2" << std::endl;
    }

    this->variableTable.at(2).clear();
}