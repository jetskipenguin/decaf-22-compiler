#pragma once
#include <string>

enum Type {
    Void, Int, Double, Bool, String, Null, Error
};

enum Scope {
    GLOBAL, LOCAL, PARAMETER
};

struct IdentifierEntry {
    std::string name;
    Type dataType;
    int blockLevel;
};