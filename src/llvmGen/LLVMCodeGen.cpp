#include "llvmGen/LLVMCodeGen.h"
#include "llvmGen/LLVMUtility.h"
#include "symbolTable/SymbolTable.h"

SymbolTable symbolTable;

 LLVMCodeGen::LLVMCodeGen()
     : module(new llvm::Module("MyModule", context)), builder(context) {
     //Initialize module and IR builder
 }

 LLVMCodeGen::~LLVMCodeGen() {
     delete module;
 }

 void LLVMCodeGen::visit(const Program* program) {
     for (const auto& decl : program->declarations) {
         decl->accept(this);
     }
     for (const auto& func : program->functions) {
		 func->accept(this);
	 }
     
     for (const auto& stmt : program->statements) {
		 stmt->accept(this);
	 }

     for (const auto& expr : program->expressions) {
        expr->accept(this);
     }
     
 }

 void LLVMCodeGen::visit(const IntDeclaration* decl) {
	 llvm::Constant* initVal = llvm::ConstantInt::get(context, llvm::APInt(32, std::stoi(decl->number), true));
	 
	 if (currentFunction) { //need testing
		 // Handle as local variable
		 llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());
		 llvm::AllocaInst* alloca = tmpBuilder.CreateAlloca(llvm::Type::getInt32Ty(context), 0, decl->name);
		 builder.CreateStore(initVal, alloca);

		 // Remember the variable for later use within the function
		 currentLocals[decl->name] = alloca;

	 }
	 else {
		 
		 // Create a global variable in the module.
		 llvm::GlobalVariable* gVar = new llvm::GlobalVariable(
			 *module,
			 llvm::Type::getInt32Ty(context), // Assuming 32-bit integers
			 false,                           // isConstant: false since it's a variable
			 llvm::GlobalValue::ExternalLinkage,
			 initVal,                         // Initializer
			 decl->name                       // Variable name
		 );

		 // Optionally, set alignment
		 gVar->setAlignment(llvm::MaybeAlign(4));
		 globals[decl->name] = gVar;
	 }
 }

 void LLVMCodeGen::visit(const FloatDeclaration* decl) {
	 
	 llvm::ConstantFP* initVal = llvm::ConstantFP::get(context, llvm::APFloat(std::stof(decl->number)));

	 if (currentFunction) {
		 // Handle as local variable
		 llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());
		 llvm::AllocaInst* alloca = tmpBuilder.CreateAlloca(llvm::Type::getFloatTy(context), nullptr, decl->name);
		 builder.CreateStore(initVal, alloca);

		 // Remember the variable for later use within the function
		 currentLocals[decl->name] = alloca;

	 }
	 else {
		 // Handle as global variable
		 llvm::GlobalVariable* gVar = new llvm::GlobalVariable(
			 *module,
			 llvm::Type::getFloatTy(context), // Assuming 32-bit integers
			 false,                           // isConstant: false since it's a variable
			 llvm::GlobalValue::ExternalLinkage,
			 initVal,                         // Initializer
			 decl->name                       // Variable name
		 );

		 // Optionally, set alignment
		 gVar->setAlignment(llvm::MaybeAlign(4));

		 globals[decl->name] = gVar;
		 
	 }

 }

 void LLVMCodeGen::visit(const StringDeclaration* decl) {
	 if (currentFunction) {
		 auto& context = builder.getContext();
		 llvm::Type* charType = llvm::Type::getInt8Ty(context);
		 auto strValue = builder.CreateGlobalStringPtr(decl->value, "globalStr");
		 llvm::ArrayType* strType = llvm::ArrayType::get(charType, decl->value.size() + 1); // +1 for null terminator
		 auto alloca = builder.CreateAlloca(strType, nullptr, decl->name);
		 builder.CreateStore(strValue, alloca);
		 
		 currentLocals[decl->name] = alloca;
	 }

	 else {
		 // Global string variable case
		 auto& context = builder.getContext();
		 auto strValue = llvm::ConstantDataArray::getString(context, decl->value, true); // true for adding null terminator
		 auto strGlobalVar = new llvm::GlobalVariable(*module, strValue->getType(), true, llvm::GlobalValue::PrivateLinkage, strValue, decl->name + ".globalStr");

		 // Create a constant pointer to the first element of the string
		 auto zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
		 std::vector<llvm::Constant*> indices{ zero, zero };
		 auto strPtr = llvm::ConstantExpr::getGetElementPtr(strValue->getType(), strGlobalVar, indices);

		 // Store the pointer in a global variable
		 llvm::GlobalVariable* gVar = new llvm::GlobalVariable(*module, strPtr->getType(), false, llvm::GlobalValue::ExternalLinkage, strPtr, decl->name);

		 // Optionally, set alignment for the global variable
		 gVar->setAlignment(llvm::MaybeAlign(1)); // Alignment for characters is typically 1

		 // Save the global variable in globals for later reference
		 globals[decl->name] = gVar;
	 }
 }

 void LLVMCodeGen::visit(const BoolDeclaration* decl) {
	 llvm::Constant* initVal = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), decl->value == "true" ? 1 : 0);

	 if (currentFunction) {
		 // Handle as a local variable within a function
		 llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());
		 llvm::AllocaInst* alloca = tmpBuilder.CreateAlloca(llvm::Type::getInt1Ty(context), nullptr, decl->name);
		 builder.CreateStore(initVal, alloca);

		 // Remember the variable for later use within the function
		 currentLocals[decl->name] = alloca;
	 }
	 else {
		 // Handle as a global variable
		 llvm::GlobalVariable* gVar = new llvm::GlobalVariable(
			 *module,
			 llvm::Type::getInt1Ty(context), // Type for boolean
			 false,                          // isConstant: false since it's a variable
			 llvm::GlobalValue::ExternalLinkage,
			 initVal,                        // Initializer
			 decl->name                      // Variable name
		 );

		 // Optionally, set alignment
		 gVar->setAlignment(llvm::MaybeAlign(1)); // Alignment for boolean is typically 1

		 // Save the global variable in globals for later reference
		 globals[decl->name] = gVar;
	 }
 }


 void LLVMCodeGen::visit(const ReturnStatement* stmt) {
	 //Generate LLVM IR for a return statement.
 }

 void LLVMCodeGen::visit(const LoopStatement* stmt) {
	 //Generate LLVM IR for a loop statement.
 }

 void LLVMCodeGen::visit(const ForLoopStatement* stmt) {
	 //Generate LLVM IR for a for loop statement.
 }

 void LLVMCodeGen::visit(const WhileLoopStatement* stmt) {
	 //Generate LLVM IR for a while loop statement.
 }

 void LLVMCodeGen::visit(const IfStatement* stmt) {
	 //Generate LLVM IR for an if statement.
 }

 void LLVMCodeGen::visit(const ElseStatement* stmt) {
	 //Generate LLVM IR for an else statement.
 }

 void LLVMCodeGen::visit(const AssignmentStatement* stmt) {
	 //Generate LLVM IR for an assignment statement.
 }

 void LLVMCodeGen::visit(const PrintStatement* stmt) {
	 //Generate LLVM IR for a print statement.
 }

 void LLVMCodeGen::visit(const BlockStatement* stmt) {
	 //Generate LLVM IR for a block statement.
 }

 void LLVMCodeGen::visit(const ExpressionStatement* stmt) {
	 //Generate LLVM IR for an expression statement.
 }

 void LLVMCodeGen::visit(const BinaryExpression* expr) {
	 //Generate LLVM IR for a binary expression.
 }

 void LLVMCodeGen::visit(const LogicAndExpression* expr) {
	 //Generate LLVM IR for a logical expression.
 }

 void LLVMCodeGen::visit(const LogicOrExpression* expr) {
	 //Generate LLVM IR for a logical OR expression.
 }

 void LLVMCodeGen::visit(const EqualityExpression* expr) {
	 //Generate LLVM IR for an equality expression.
 }

 void LLVMCodeGen::visit(const ComparisonExpression* expr) {
	 //Generate LLVM IR for a relational expression.
 }

 void LLVMCodeGen::visit(const TermExpression* expr) {
	 //Generate LLVM IR for an additive expression.
 }

 void LLVMCodeGen::visit(const FactorExpression* expr) {
	 //Generate LLVM IR for a multiplicative expression.
 }

 void LLVMCodeGen::visit(const UnaryExpression* expr) {
	 //Generate LLVM IR for a unary expression.
 }

 void LLVMCodeGen::visit(const PrimaryExpression* expr) {
	 //Generate LLVM IR for a primary expression.
 }

 void LLVMCodeGen::visit(const AssignmentExpression* expr) {
	 //Generate LLVM IR for an assignment expression.
 }

 void LLVMCodeGen::visit(const FunctionDefinition* funcDef) {
	 std::vector<llvm::Type*> paramTypes;
	 for (const auto& param : funcDef->parameters) {
		 llvm::Type* type = nullptr;
		 if (param.type == "int") {
			 type = llvm::Type::getInt32Ty(context);
		 }
		 else if (param.type == "float") {
			 type = llvm::Type::getFloatTy(context);
		 }
		 else if (param.type == "bool") {
			 type = llvm::Type::getInt1Ty(context);
		 }
		 //else if (param.type == "string") { // not sure if this is correct
			// type = llvm::Type::getInt8PtrTy(context);
		 //}
		 else if (param.type == "void") {
			 type = llvm::Type::getVoidTy(context);
		 }
		 // Add more type mappings as necessary
		 if (type) {
			 paramTypes.push_back(type);
		 }
		 else {
			 std::cerr << "Unsupported parameter type: " << param.type << std::endl;
			 return; // Skip unsupported types
		 }
	 }

	 llvm::Type* returnType = nullptr;
	 if (funcDef->returnType == "int") {
		 returnType = llvm::Type::getInt32Ty(context);
	 }
	 else if (funcDef->returnType == "float") {
		 returnType = llvm::Type::getFloatTy(context);
	 }
	 else if (funcDef->returnType == "bool") {
		 returnType = llvm::Type::getInt1Ty(context);
	 }
	 //else if (funcDef->returnType == "string") { // not sure if this is correct
		// returnType = llvm::Type::getInt8PtrTy(context);
	 //}
	 else if (funcDef->returnType == "void") {
		 returnType = llvm::Type::getVoidTy(context);
	 } // Add more type mappings as necessary

	 llvm::FunctionType* functionType = llvm::FunctionType::get(returnType, paramTypes, false);
	 llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, funcDef->name, module);

	 // Create a new basic block to start insertion into.
	 llvm::BasicBlock* block = llvm::BasicBlock::Create(context, "entry", function);
	 builder.SetInsertPoint(block);

	 // Record the function arguments in the NamedValues map.
	 unsigned idx = 0;
	 for (auto& arg : function->args()) {
		 // Create an alloca for this variable.
		 llvm::AllocaInst* alloca = llvm_util::createEntryBlockAlloca(function, std::string(arg.getName()), arg.getType());

		 // Store the initial value into the alloca.
		 builder.CreateStore(&arg, alloca);

		 // Add arguments to variable currentLocals
		 currentLocals[std::string(arg.getName())] = alloca;

		 idx++;
	 }

	 currentFunction = function; // Track the current function

	 // Visit each statement in the function body to generate their IR
	 for (const auto& stmt : funcDef->body) {
		 stmt->accept(this);
	 }

	 // Add a return statement for void functions if not present
	 if (returnType == llvm::Type::getVoidTy(context) && !block->getTerminator()) {
		 builder.CreateRetVoid();
	 }

	 currentFunction = nullptr; // Clear the current function

	 // Verify the generated code, checking for consistency.
	 llvm::verifyFunction(*function);

	 // Optimize the function.
	 // (You may add optimization passes here)

	 // Reset the builder's insert point
	 builder.ClearInsertionPoint();
 }

 void LLVMCodeGen::visit(const FunctionCall* call) {
	 //Generate LLVM IR for a function call.
 }


