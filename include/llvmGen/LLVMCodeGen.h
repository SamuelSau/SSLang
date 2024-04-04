#ifndef LLVM_CODE_GEN_H
#define LLVM_CODE_GEN_H

#include "ast/ASTNodes.h"
#include "visitor/Visitor.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

class LLVMCodeGen : public IVisitor {
public:  
    LLVMCodeGen();
    ~LLVMCodeGen();

    llvm::Module* getModule() const;
    llvm::Value* evaluateExpression(Expression* expr);
    void tryLoadAndDebug(llvm::Value* ptr, const PrimaryExpression* expr);
    void ensureMainFunctionExist();

    // Visitor functions for different AST nodes
    void visit(const Program* program) override;
    void visit(const IntDeclaration* decl) override;
    void visit(const FloatDeclaration* decl) override;
    void visit(const StringDeclaration* decl) override;
    void visit(const BoolDeclaration* decl) override;
    void visit(const ReturnStatement* stmt) override;
    void visit(const ForLoopStatement* stmt) override;
    void visit(const WhileLoopStatement* stmt) override;
    void visit(const IfStatement* stmt) override;
    void visit(const AssignmentStatement* stmt) override;
    void visit(const PrintStatement* stmt) override;
    void visit(const BlockStatement* stmt) override;
    void visit(const ExpressionStatement* stmt) override;
    void visit(const BinaryExpression* expr) override;
    void visit(const UnaryExpression* expr) override;
    void visit(const PrimaryExpression* expr) override;
    void visit(const AssignmentExpression* expr) override;
    void visit(const FunctionDefinition* expr) override;
    void visit(const FunctionCall* call) override;

private:
    
    llvm::LLVMContext context;
    llvm::Module* module;
    llvm::IRBuilder<> builder;

    llvm::Function* currentFunction = nullptr;
    llvm::Value* lastValue = nullptr;


    std::unordered_map<std::string, llvm::Value*> currentLocals; // Current function's local variables
    std::unordered_map<std::string, llvm::GlobalVariable*> globals; // Global variables
    
    std::vector<std::string> functionCalls;

    // Add any additional state needed for code generation

}; 
#endif // LLVM_CODE_GEN_H