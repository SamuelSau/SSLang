// #include "llvmGen/LLVMCodeGen.h"

// LLVMCodeGen::LLVMCodeGen()
//     : module(new llvm::Module("MyModule", context)), builder(context) {
//     Initialize module and IR builder
// }

// LLVMCodeGen::~LLVMCodeGen() {
//     delete module;
// }

// void LLVMCodeGen::visit(const Program* program) {
//     for (const auto& decl : program->declarations) {
//         decl->accept(this);
//     }
//     Similarly, visit other components of the program
// }

// void LLVMCodeGen::visit(const IntDeclaration* decl) {
//     Generate LLVM IR for an integer declaration.
//     This is just a placeholder logic. You'll need to replace it with actual IR generation code.
//     llvm::Value* intVal = llvm::ConstantInt::get(context, llvm::APInt(32, std::stoi(decl->number), true));
//     Assuming 'number' is a string representing an integer. Adjust the logic based on your AST.
//     You'll likely need to insert this value into some function or global scope, depending on your language semantics.
// }

// void LLVMCodeGen::visit(const FloatDeclaration* decl) {
//     Similar to the integer declaration but for floats.
// }

// Implement other visitor methods...

