#pragma once

#include <string>
#include <vector>

struct Register {
    std::string name;
};

struct Instruction {
    std::string op;
    Register src;
    Register dest;
};

enum Label {
    ELSE,
    END,
};

inline std::string sLabelToString(Label label) {
    switch(label) {
        case ELSE:
            return "ELSE";
        case END:
            return "END";
    }
    return "";
}

class CodeGenerator {
    private:
        std::vector<std::string> instructions;
        std::vector<Register> registers;
        std::vector<Register> availableRegisters;
    public:
        CodeGenerator();
        void emit(Instruction instruction);
        void emit(Label label);
        Register getNextRegister();
};