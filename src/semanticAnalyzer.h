#include "SymbolTable.h"
#include <stdexcept>

void analyzeAST(std::shared_ptr<ASTRootNode> &rootNode) {

    SymbolTable table;


    for (const auto& decl : rootNode->decls) {
        
        try {
            table.install(decl->identifier->name, 1);
        }
        catch(std::invalid_argument e) {
             std::cout << "Error: Identifier " << decl->identifier->name << " cannot be used more than once" << std::endl;
        }

        // Check if global variable, then record in 
        std::shared_ptr<VarDecl> varDecl = std::dynamic_pointer_cast<VarDecl>(decl);
        if(varDecl) {
            std::shared_ptr<IdentifierEntry> entry = table.lookup(varDecl->identifier->name, 1);
            entry->type = varDecl->type;
            varDecl->check(table, 1);
            continue;
        }

        std::shared_ptr<FunctionDecl> functionDecl = std::dynamic_pointer_cast<FunctionDecl>(decl);
        if(functionDecl) {
            std::shared_ptr<IdentifierEntry> entry = table.lookup(functionDecl->identifier->name, 1);
            entry->type = functionDecl->returnType;
            
            // Record function params in symbol table
            for(auto &param: functionDecl->formals) {
                table.install(param->identifier->name, 2);
            }
            
            // Record local variables in symbol table
            for(auto &stmt : functionDecl->body->stmts) {
                std::shared_ptr<VarDeclStmt> maybeLocalVar = std::dynamic_pointer_cast<VarDeclStmt>(stmt);
                if(!maybeLocalVar) {
                    continue;
                }

                table.install(maybeLocalVar->varDecl->identifier->name, 2);
            }

            functionDecl->check(table, 2);

            // TODO: clear block level 2 before next function
            
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

    std::shared_ptr<IdentifierEntry> entry = table.lookup(identifierName, 2);
    if(entry != nullptr) {
        return;
    }

    entry = table.lookup(identifierName, 1);
    if(entry != nullptr) {
        return;
    }

    std::cout << "*** No declaration for Function '" << identifierName << "' found";
    return;
}

void verifyExprOperands(ASTNodeType* leftOperand, ASTNodeType* rightOperand) {
    if(!leftOperand->isAssignableTo(rightOperand)) {
        std::cout << "*** Incompatible operands: " << leftOperand->typeName() << " * " << rightOperand->typeName() << std::endl;
    }
}