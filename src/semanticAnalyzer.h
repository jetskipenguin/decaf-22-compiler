#include "ASTNodes.h"

void analyzeAST(std::shared_ptr<ASTRootNode> &rootNode) {
    for (const auto& decl : rootNode->decls) {
        std::cout << decl->identifier->name << std::endl;
    }
}