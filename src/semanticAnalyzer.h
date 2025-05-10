#include "ASTNodes.h"
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

        if(decl->declType == DeclType::VAR_DECL) {
            
        }

        

        std::cout << decl->identifier->name << std::endl;
    }
}