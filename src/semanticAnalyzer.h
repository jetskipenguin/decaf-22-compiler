#include "SymbolTable.h"
#include <stdexcept>

void analyzeAST(std::shared_ptr<ASTRootNode> &rootNode, bool verbose) {

    SymbolTable table(verbose);

    for (const auto& decl : rootNode->decls) {
        
        // Check if global variable, then record in 
        std::shared_ptr<VarDecl> varDecl = std::dynamic_pointer_cast<VarDecl>(decl);
        if(varDecl) {
            if(verbose) {
                std::cout << "Analyzing VarDecl: " << varDecl->identifier->name << std::endl;
            }
            table.installVariable(varDecl->identifier->name, 1);
            std::shared_ptr<IdentifierEntry> entry = table.lookupVariable(varDecl->identifier->name, 1);
            entry->type = varDecl->type;
            varDecl->check(table, 1);
            continue;
        }

        std::shared_ptr<FunctionDecl> functionDecl = std::dynamic_pointer_cast<FunctionDecl>(decl);
        if(functionDecl) {
            if(verbose) {
                std::cout << "Analyzing FunctionDecl: " << functionDecl->identifier->name << std::endl;
            }

            table.installFunction(functionDecl->identifier->name, 1);
            std::shared_ptr<FunctionEntry> entry = table.lookupFunction(functionDecl->identifier->name, 1);
            entry->id.type = functionDecl->returnType;
            
            if(verbose) {
                std::cout << "Checking for function params..." << std::endl;
            }
            // Record function params in symbol table
            for(auto &param: functionDecl->formals) {
                table.installVariable(param->identifier->name, 2);
                std::shared_ptr<IdentifierEntry> entry = table.lookupVariable(param->identifier->name, 2);
                entry->type = param->type;
            }

            if(verbose) {
                std::cout << "Checking for local variables..." << std::endl;
            }
            
            // Record types of function params
            for(auto &param : functionDecl->formals) {
                entry->params.push_back(param->type);
            }

            // Record local variables in symbol table
            for(auto &stmt : functionDecl->body->stmts) {
                std::shared_ptr<VarDeclStmt> maybeLocalVar = std::dynamic_pointer_cast<VarDeclStmt>(stmt);
                if(!maybeLocalVar) {
                    continue;
                }
                table.installVariable(maybeLocalVar->varDecl->identifier->name, 2);
                std::shared_ptr<IdentifierEntry> entry = table.lookupVariable(maybeLocalVar->varDecl->identifier->name, 2);
                entry->type = maybeLocalVar->varDecl->type;
            }

            functionDecl->check(table, 2);

            table.clearBlockLevelTwo();
            
            continue;
        }

        std::cout << "TypeError: Declaration is neither functional or variable declaration" << std::endl;
    }
}

void analyzeFunction(std::shared_ptr<FunctionDecl> functionDecl, SymbolTable &table) {
    

    for (const auto& decl : functionDecl->body->stmts) {

    }
}

// Prints error if declaration for given identifier name does not exist.
void verifyVarDeclExists(std::string identifierName, SymbolTable &table) {

    std::shared_ptr<IdentifierEntry> entry = table.lookupVariable(identifierName, 2);
    if(entry != nullptr) {
        return;
    }

    entry = table.lookupVariable(identifierName, 1);
    if(entry != nullptr) {
        return;
    }

    std::cout << "*** No declaration for Function '" << identifierName << "' found";
    return;
}