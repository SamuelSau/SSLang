// LLVMOptimizer.h
#ifndef LLVM_OPTIMIZER_H
#define LLVM_OPTIMIZER_H

#include "llvm/IR/Module.h"

class LLVMOptimizer {
public:
    static void optimize(llvm::Module* module);
};

#endif // LLVM_OPTIMIZER_H