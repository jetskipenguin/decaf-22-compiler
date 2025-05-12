#pragma once
#include "CodeGenerator.h"

CodeGenerator::CodeGenerator() {
    for (int i = 0; i < 32; i++) {
        Register reg = Register();
        reg.name = "r" + i;
        this->registers.push_back(reg);
        this->availableRegisters.push_back(reg);
    }
}

void CodeGenerator::emit(Instruction instruction) {
    std::string strInstruction = instruction.op;
    
    strInstruction.append(instruction.src.name);
    strInstruction.append(instruction.dest.name);
    
    this->instructions.push_back(strInstruction);
}

void CodeGenerator::emit(Label label) {
    this->instructions.push_back(sLabelToString(label));
}

Register CodeGenerator::getNextRegister() {
    Register result = availableRegisters.at(0);
    availableRegisters.pop_back();
    return result;
}