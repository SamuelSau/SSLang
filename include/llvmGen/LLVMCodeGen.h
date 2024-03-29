//#ifndef LLVM_CODE_GEN_H
//#define LLVM_CODE_GEN_H
//
//#include "ast/ASTNodes.h"
//#include "visitor/Visitor.h"
//
//#include "llvm/ir/llvmcontext.h"
//#include "llvm/ir/irbuilder.h"
//#include "llvm/ir/llvmcontext.h"
//
//class LLVMCodeGen : public IVisitor {
//public:
//    LLVMCodeGen();
//    ~LLVMCodeGen();
//
//    llvm::Module* getModule() const { return module; }
//
//    // Visitor functions for different AST nodes
//    void visit(const Program* program) override;
//    void visit(const IntDeclaration* decl) override;
//    void visit(const FloatDeclaration* decl) override;
//    // Add other necessary visitor functions here...
//
//private:
//    llvm::LLVMContext context;
//    llvm::Module* module;
//    llvm::IRBuilder<> builder;
//    // Add any additional state needed for code generation
//};
//
//#endif // LLVM_CODE_GEN_H
