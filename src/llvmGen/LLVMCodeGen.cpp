#include "llvmGen/LLVMCodeGen.h"
#include "llvmGen/LLVMUtility.h"
#include "symbolTable/SymbolTable.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Type.h"

LLVMCodeGen::LLVMCodeGen()
	: module(new llvm::Module("MyModule", context)), builder(context) {
}

LLVMCodeGen::~LLVMCodeGen() {
	delete module;
}

llvm::Module* LLVMCodeGen::getModule() const {
	return module;
}

 llvm::Value* LLVMCodeGen::evaluateExpression(Expression* expr) {
	 std::cout << "Beginning of evaluateExpression()\n";
	 // This function should visit the expression and return an llvm::Value* representing its value.
	 expr->accept(this);
	 std::cout << "After accepting the expression\n";
	 if (!lastValue) {
		 std::cerr << "Error evaluating expression.\n";
	 }
	 std::cout << "Last value: " << lastValue << "\n";
	 llvm::Value* result = lastValue;

	 lastValue = nullptr;
	 return result;
 }

 void LLVMCodeGen::tryLoadAndDebug(llvm::Value* ptr, const PrimaryExpression* expr) {
	 std::cout<< "Attempting to load and debug in tryLoadAndDebug\n";

	 if (!ptr) {
		 std::cerr << "Error: nullptr passed to tryLoadAndDebug.\n";
		 return;
	 }
	 std::cout << "Ptr is not nullptr in tryLoadAndDebug\n";

	 llvm::Type* type = nullptr;

	 std::cout << "Setting type for load instruction to nullptr\n";

	 if (auto allocaInst = llvm::dyn_cast<llvm::AllocaInst>(ptr)) {
		 if (!allocaInst) {
			 std::cerr << "Expected an AllocaInst for local variable: " << expr->name << std::endl;
		 }
		 std::cout << "Got allocaInst\n";
		 type = allocaInst->getAllocatedType();
		 std::cout << "Set the type for allocaInst\n";
	 }
	 else if (auto globalVar = llvm::dyn_cast<llvm::GlobalVariable>(ptr)) {
		 if (!globalVar) {
			 std::cerr << "Expected a GlobalVariable for global variable: " << expr->name << std::endl;
			 return;
		 }
		 // Check if the global variable is a string (i.e., an array of i8)
		 if (globalVar->getValueType()->isArrayTy() &&
			 globalVar->getValueType()->getArrayElementType()->isIntegerTy(8)) {
			 // For global strings, we don't load the value; we use the pointer directly.
			 // This is a simplification. In real code, consider more robust checks and logic.
			 lastValue = globalVar;
			 std::cout << "Using global string variable directly: " << expr->name << std::endl;
		 }
		 type = globalVar->getValueType();
	 }
	 else {
		 std::cerr << "Error: Unsupported llvm::Value type for loading.\n";
		 return;
	 }

	 if (type) {
		 std::cout << "Attempting to create load instruction...\n";
		 lastValue = builder.CreateLoad(type, ptr, expr->name);
		 if (lastValue) {
			 lastValue->print(llvm::errs()); // Print to the standard error stream
			 llvm::errs() << "\n"; // Add a newline for readability
			 std::cout << "Load instruction created successfully.\n";
		 }
		 else {
			 llvm::errs() << "lastValue is nullptr\n";
			 std::cerr << "Failed to create load instruction.\n";
		 }
	 }
	 else {
		 std::cerr << "Error: Failed to determine type for loading.\n";
		 lastValue = nullptr;
	 }
 }

 void LLVMCodeGen::ensureMainFunctionExist() {
	 llvm::Function* mainFunction = module->getFunction("main");
	 if (!mainFunction) {
		 // Create the 'main' function if it does not exist
		 std::cout << "Creating main function" << std::endl;
		 llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false);
		 mainFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", module);
		 llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context, "entry", mainFunction);
	 }

	 // Ensure there's a basic block to insert into
	 if (mainFunction->empty()) {
		 std::cout << "Creating basic block for main function\n";
		 llvm::BasicBlock::Create(context, "entry", mainFunction);
	 }

	 // Set insertion point to the start of 'main' by default, or to a specific place as needed
	 builder.SetInsertPoint(&mainFunction->getEntryBlock());
 }

 void LLVMCodeGen::visit(const Program* program) {

	for (const auto& decl : program->declarations) {
		decl->accept(this);
	}
	for (const auto& stmt : program->statements) {
		stmt->accept(this);
	}

	for (const auto& expr : program->expressions) {
		expr->accept(this);
	}

	for (const auto& func : program->functions) {
		func->accept(this);
	}

	ensureMainFunctionExist();

	// Complete the 'main' function with a return statement. Return 0 as a conventional success code.
	builder.CreateRet(llvm::ConstantInt::get(context, llvm::APInt(32, 0)));

	std::cout << "Finished creating return statement for main function\n";

	// Reset the builder's insertion point to avoid dangling references
	builder.ClearInsertionPoint();

    std::cout << "Finished visiting program\n";

	currentLocals.clear();
	globals.clear();
	 
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
		 //auto& context = builder.getContext();
		 //llvm::Type* charType = llvm::Type::getInt8Ty(context);
		 //auto strValue = builder.CreateGlobalStringPtr(decl->value, "globalStr");
		 //llvm::ArrayType* strType = llvm::ArrayType::get(charType, decl->value.size() + 1); // +1 for null terminator
		 //auto alloca = builder.CreateAlloca(strType, nullptr, decl->name);
		 //builder.CreateStore(strValue, alloca);
		 //
		 //currentLocals[decl->name] = alloca;
	 }

	 else {
		 std::cout << "String declared globally" << std::endl;
		 auto& context = builder.getContext();


		 llvm::Constant* strConstant = llvm::ConstantDataArray::getString(context, decl->value, true);
		 llvm::GlobalVariable* gVar = new llvm::GlobalVariable(
			 *module,
			 strConstant->getType(),
			 true, // isConstant
			 llvm::GlobalValue::PrivateLinkage,
			 strConstant,
			 decl->name
		 );
		 gVar->setAlignment(llvm::MaybeAlign(1));

		 ////If you need a pointer to the first element of this array for some reason:
		 //auto zero = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
		 //std::vector<llvm::Constant*> indices = { zero, zero };
		 //llvm::Constant* strPtr = llvm::ConstantExpr::getGetElementPtr(strConstant->getType(), gVar, indices);
		 //std::cout << "This is the decl->name: " << decl->name << "\n";
		 //
		 //globalStringPointers[decl->name] = strPtr;		 
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
	 // First, evaluate the expression to get its value
	 llvm::Value* returnValue = evaluateExpression(stmt->expression.get());

	 if (!returnValue) {
		 std::cerr << "Error evaluating return expression." << std::endl;
		 return;
	 }

	 builder.CreateRet(returnValue);
 }



 void LLVMCodeGen::visit(const ForLoopStatement* stmt) {
	 llvm::Function* function = builder.GetInsertBlock()->getParent();

	 // Pre-loop setup. Assuming 'start' initializes the loop variable
	 llvm::Value* startVal = evaluateExpression(stmt->start.get());
	 llvm::AllocaInst* loopVar = llvm_util::createEntryBlockAlloca(function, "loopVar", startVal->getType());
	 builder.CreateStore(startVal, loopVar);

	 // Create BasicBlocks for the loop condition check, loop body, and loop end (to exit the loop)
	 llvm::BasicBlock* condBB = llvm::BasicBlock::Create(context, "forCond", function);
	 llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(context, "forBody", function);
	 llvm::BasicBlock* endBB = llvm::BasicBlock::Create(context, "forEnd", function);

	 // Branch to condition block to start loop execution
	 builder.CreateBr(condBB);

	 // Loop condition block
	 builder.SetInsertPoint(condBB);
	 llvm::Value* endVal = evaluateExpression(stmt->end.get());
	 llvm::Value* loopVarValue = builder.CreateLoad(loopVar->getAllocatedType(), loopVar, "loopVar");
	 // Assuming 'end' evaluates whether to continue the loop
	 llvm::Value* condValue = builder.CreateICmpSLT(loopVarValue, endVal, "loopcond"); // Compare if loopVar < endVal
	 builder.CreateCondBr(condValue, bodyBB, endBB);

	 // Loop body
	 builder.SetInsertPoint(bodyBB);
	 stmt->body->accept(this); // Execute the loop body

	 // Loop increment
	 llvm::Value* stepVal = llvm::ConstantInt::get(context, llvm::APInt(32, 1)); // Assuming an increment by 1
	 llvm::Value* nextVar = builder.CreateAdd(loopVarValue, stepVal, "nextVar");
	 builder.CreateStore(nextVar, loopVar);

	 // After body, jump back to check condition again
	 builder.CreateBr(condBB);

	 // Continue with the rest of the code after the loop
	 builder.SetInsertPoint(endBB);
 }


 void LLVMCodeGen::visit(const WhileLoopStatement* stmt) {
	 llvm::Function* function = builder.GetInsertBlock()->getParent();

	 // Create BasicBlocks for condition check, loop body, and exit
	 llvm::BasicBlock* conditionBB = llvm::BasicBlock::Create(context, "whileCond", function);
	 llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(context, "whileBody", function);
	 llvm::BasicBlock* loopExitBB = llvm::BasicBlock::Create(context, "whileExit", function);

	 // Branch to the condition block to begin the loop
	 builder.CreateBr(conditionBB);

	 // Populate conditionBB
	 builder.SetInsertPoint(conditionBB);
	 llvm::Value* condValue = evaluateExpression(stmt->condition.get());

	 // Ensure condValue is a boolean i1 for LLVM's conditional branch instruction
	 if (!condValue->getType()->isIntegerTy(1)) {
		 // This is a simplified example; you'll need actual logic to handle this
		 throw std::logic_error("Condition expression must evaluate to a boolean (i1) type");
	 }

	 builder.CreateCondBr(condValue, loopBodyBB, loopExitBB);

	 // Populate loopBodyBB
	 builder.SetInsertPoint(loopBodyBB);
	 stmt->body->accept(this);
	 // Jump back to conditionBB to re-evaluate the condition
	 builder.CreateBr(conditionBB);

	 // Continue with the rest of the code after the loop
	 builder.SetInsertPoint(loopExitBB);
 }


 void LLVMCodeGen::visit(const IfStatement* stmt) {
	 auto& context = builder.getContext();
	 auto* function = builder.GetInsertBlock()->getParent();

	 // Create blocks for the then, else (optional), and merge parts of the if statement
	 llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", function);
	 llvm::BasicBlock* elseBB = stmt->elseBody ? llvm::BasicBlock::Create(context, "else", function) : nullptr;

	 // Assuming 'condition' is an Expression that can be evaluated to a value
	 llvm::Value* condValue = evaluateExpression(stmt->condition.get());; // Evaluate the condition expression
     
	 if (elseBB) {
		 builder.CreateCondBr(condValue, thenBB, elseBB);
	 }

	 // Populate the 'then' block
	 builder.SetInsertPoint(thenBB);
	 // Visit/translate the body of the 'then' part
	 stmt->thenBody->accept(this);

	 if (stmt->elseBody) {
		 //function->getBasicBlockList().push_back(elseBB);
		 builder.SetInsertPoint(elseBB);
		 stmt->elseBody->accept(this);
	 }

 }

 void LLVMCodeGen::visit(const AssignmentStatement* stmt) {
	 llvm::Value* valueToAssign = evaluateExpression(stmt->expression.get());
	 if (!valueToAssign) {
		 std::cerr << "Error evaluating expression for assignment to " << stmt->name << std::endl;
		 return;
	 }

	 // Check if the variable is a local variable in the current function scope
	 auto localIt = currentLocals.find(stmt->name);
	 if (localIt != currentLocals.end()) {
		 // It's a local variable, generate a store instruction to update its value
		 builder.CreateStore(valueToAssign, localIt->second);
	 }
	 else {
		 // If not found in local, check global variables
		 auto globalIt = globals.find(stmt->name);
		 if (globalIt != globals.end()) {
			 // It's a global variable, generate a store instruction to update its value
			 builder.CreateStore(valueToAssign, globalIt->second);
		 }
		 else {
			 std::cerr << "Variable " << stmt->name << " not found for assignment." << std::endl;
		 }
	 }

 }

 void LLVMCodeGen::visit(const PrintStatement* stmt) {
	 
	 std::cout << "Visiting print statement in LLVMCodeGen" << std::endl;

	 llvm::Function* printfFunc = module->getFunction("printf");
	 if (!printfFunc) {
		 llvm::FunctionType* printfType = llvm::FunctionType::get(
			 llvm::IntegerType::getInt32Ty(context),
			 llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context)),
			 true); // Indicates a variadic function
		 printfFunc = llvm::Function::Create(
			 printfType,
			 llvm::Function::ExternalLinkage,
			 "printf",
			 module);
		 std::cout << "Declared printf in module" << std::endl;
	 }

	 std::cout << "Found printf function " << std::endl;

	 // Evaluate the expression
	 llvm::Value* valueToPrint = evaluateExpression(stmt->expr.get());
	 if (!valueToPrint) {
		 std::cerr << "Failed to evaluate expression for print statement.\n";
		 return;
	 }

	 std::cout << "Evaluated expression for print statement" << std::endl;

	 llvm::Value* formatStr = nullptr;

	 std::cout << "Created format string for print statement" << std::endl;
	 

	 if (valueToPrint->getType()->isIntegerTy(32)) {
		 // Integer
		 std::cout << "Integer value to print" << std::endl;
		 formatStr = builder.CreateGlobalStringPtr("%d\n", "printedFormatInt");
	 }
	 else if (valueToPrint->getType()->isFloatTy()) {
		 // Float
		 std::cout << "Float value to print" << std::endl;
		 formatStr = builder.CreateGlobalStringPtr("%f\n", "printedFloatInt");
	 }
	 else if (valueToPrint->getType()->getArrayElementType()->isIntegerTy(8)) {
		 // Check if the pointer points to an i8 type, common for strings
		
		std::cout << "String value to print" << std::endl;
		formatStr = builder.CreateGlobalStringPtr("%s\n", "formatStr");
	 }

	 else if (valueToPrint->getType()->isIntegerTy(1)) {
		 // Boolean
		 std::cout << "Boolean value to print" << std::endl;
		 formatStr = builder.CreateGlobalStringPtr("%s\n", "printedFormatBool");
		 // Convert the boolean to a string for printing
		 llvm::Value* trueStr = builder.CreateGlobalStringPtr("true", "trueStr");
		 llvm::Value* falseStr = builder.CreateGlobalStringPtr("false", "falseStr");
		 valueToPrint = builder.CreateSelect(valueToPrint, trueStr, falseStr);
	 }
	 else {
		 std::cerr << "Unsupported type for print statement" << std::endl;
		 valueToPrint->getType()->print(llvm::errs());
		 llvm::errs() << "\n"; // Print the type to standard error
		 return;
	 }

	 std::vector<llvm::Value*> printfArgs = { formatStr, valueToPrint};
	 // Create the call to printf
	 builder.CreateCall(printfFunc, printfArgs);

	 std::cout << "Finished print statement" << std::endl;
 }


 void LLVMCodeGen::visit(const BlockStatement* stmt) {
	 std::cout << "Visiting block statement" << std::endl;
	 // Save the current state of local variables for the current scope
	 auto previousLocals = currentLocals;

	 // Process each statement in the block
	 for (const auto& statement : stmt->statements) {
		 statement->accept(this);
	 }

	 // Restore the previous state of local variables, exiting the current scope
	 currentLocals = previousLocals;

	 std::cout << "Finished block statement" << std::endl;
 }

 void LLVMCodeGen::visit(const ExpressionStatement* stmt) {
	 //Generate LLVM IR for an expression statement.
	 llvm::Value* exprValue = evaluateExpression(stmt->expression.get());
 }

 void LLVMCodeGen::visit(const BinaryExpression* expr) {
	 //Generate LLVM IR for a binary expression.
     // Evaluate the left and right subexpressions
	 llvm::Value* left = evaluateExpression(expr->left.get());
	 llvm::Value* right = evaluateExpression(expr->right.get());

	 if (!left || !right) {
		 std::cerr << "Error evaluating binary expression" << std::endl;
		 lastValue = nullptr;
		 return;
	 }
	 
	 if (expr->op == "+") {
		 lastValue = builder.CreateAdd(left, right, "addtmp");
		 std::cout << "Successfully added left and right values" << std::endl;
		 return;
	 }
	 else if (expr->op == "-") {
		lastValue = builder.CreateSub(left, right, "subtmp");
		return;

	 }
	 else if (expr->op == "*") {
		 lastValue = builder.CreateMul(left, right, "multmp");
		 return;

	 }
	 else if (expr->op == "/") {
		 lastValue = builder.CreateSDiv(left, right, "divtmp");
		 return;

	 }
	 if (expr->op == "<") {
		 lastValue = builder.CreateICmpSLT(left, right, "cmptmp");
		 return;

	 }
	 else if (expr->op == ">") {
		 lastValue = builder.CreateICmpSGT(left, right, "cmptmp");
		 return;

	 }
	 else if (expr->op == "<=") {
		 lastValue = builder.CreateICmpSLE(left, right, "cmptmp");
		 return;

	 }
	 else if (expr->op == ">=") {
		 lastValue = builder.CreateICmpSGE(left, right, "cmptmp");
		 return;

	 }
	 else if (expr->op == "equals") {
		 std::cout << "Comparing left and right values for equals" << std::endl;
		 lastValue = builder.CreateICmpEQ(left, right, "cmptmp");
		 return;

	 }
	 else if (expr->op == "notEquals") {
		 std::cout << "Comparing left and right values for not equal" << std::endl;
		 lastValue = builder.CreateICmpNE(left, right, "cmptmp");
		 return;

	 }
	 else {
		 std::cerr << "Unsupported binary operation: " << expr->op << std::endl;
		 lastValue = nullptr;
		 return;

	 }
 }

 void LLVMCodeGen::visit(const UnaryExpression* expr) {
	 //Generate LLVM IR for a unary expression.

	 llvm::Value* operand = evaluateExpression(expr->expr.get());
	 if (!operand) {
		 std::cerr << "Null operand in unary expression." << std::endl;
		 return;
	 }

	 if (expr->op == "-") {
		 // Assuming the operand is an integer
		 lastValue = builder.CreateNeg(operand, "negtmp");
	 }
	 else if (expr->op == "not") {
		 // Assuming the operand is a boolean
		 lastValue = builder.CreateNot(operand, "nottmp");
	 }
	 else {
		 std::cerr << "Unsupported unary operation: " << expr->op << std::endl;
		 lastValue = nullptr;
	 }
 }

 void LLVMCodeGen::visit(const PrimaryExpression* expr) {

	 std::cout << "Looking for primary expression: " << expr->name << std::endl;
	 std::cout << "Current locals: ";
	 for (const auto& pair : currentLocals) {
		 std::cout << pair.first << " ";
	 }
	 std::cout << std::endl;
	 std::cout << "Globals: ";
	 for (const auto& pair : globals) {
		 std::cout << pair.first << " ";
	 }
	 std::cout << std::endl;

	 static const std::regex intRegex("^[-+]?[0-9]+$");
	 static const std::regex floatRegex("^[-+]?([0-9]*\\.[0-9]+|[0-9]+\\.[0-9]*)$");
	 static const std::regex stringRegex("^\".*\"$");

	 std::cout << "length of expr->name: " << expr->name.length() << "\n";
	 std::cout << "expr->name: " << expr->name << "\n";

	 if (std::regex_match(expr->name, intRegex)) {
		 int value = std::stoi(expr->name);
		 lastValue = llvm::ConstantInt::get(context, llvm::APInt(32, value, true));
		 return;
	 }
	 else if (std::regex_match(expr->name, floatRegex)) {
		 std::cout << "Primary expression is float: " << expr->name << std::endl;
		 float value = std::stof(expr->name);
		 lastValue = llvm::ConstantFP::get(context, llvm::APFloat(value));
		 return;
	 }
	 else if (expr->name == "true" || expr->name == "false") {
		 std::cout << "Primary expression is boolean: " << expr->name << std::endl;
		 bool value = (expr->name == "true");
		 lastValue = llvm::ConstantInt::get(context, llvm::APInt(1, value, true));
		 return;
	 }
	 else if (std::regex_match(expr->name, stringRegex)) {
		 // Strings are a bit more complex due to their global nature
		 std::cout << "Primary expression is string: " << expr->name << std::endl;
		 std::string strLiteral = expr->name.substr(1, expr->name.length() - 2); // Remove quotes
		 lastValue = builder.CreateGlobalStringPtr(strLiteral, "strLiteral");
		 return;
	 }
	 else {
		 std::cout << "Primary expression is identifier: " << expr->name << std::endl;
		 // Assume it's a variable name. Look up its value in `currentLocals`.
		 auto localVarIt = currentLocals.find(expr->name);
		 std::cout << "Trying to find variable: " << expr->name << " in currentLocals\n";
		 if (localVarIt != currentLocals.end()) {
			 std::cout << "Found variable: " << expr->name << " in currentLocals" << std::endl;
			 tryLoadAndDebug(localVarIt->second, expr);
			 std::cout << "Finished tryLoadAndDebug\n";
			 return;
		 }

		 //auto globalStringPtrIt = globalStringPointers.find(expr->name);
		 //if (globalStringPtrIt != globalStringPointers.end()) {
			// std::cout << "Found variable: " << expr->name << " in globalStringPointers" << std::endl;
			// // Directly use the pointer to the first element for operations requiring a string.
			// lastValue = globalStringPtrIt->second;
			// std::cout << "Using pointer to first element of global string: " << expr->name << std::endl;
			// return;
		 //}
		 
		 // If not found locally, try to find it in global variables
		 auto globalIt = globals.find(expr->name);
		 if (globalIt != globals.end()) {
			 std::cout << "Found variable: " << expr->name << " in globals" << std::endl;
			 tryLoadAndDebug(globalIt->second, expr);
			 return;
		 }
		
		 std::cerr << "Variable not found: " << expr->name << std::endl;
		 lastValue = nullptr;
		 return;
	 }
 }

 void LLVMCodeGen::visit(const AssignmentExpression* expr) {
	 std::cout << "Assignment expression: " << expr->name << " =" << std::endl;
	 llvm::Value* valueToAssign = evaluateExpression(expr->expression.get());
	 if (!valueToAssign) {
		 std::cerr << "Error evaluating the expression to assign." << std::endl;
		 return;
	 }
	 std::cout << "Value to assign was evaluated successfully" << std::endl;

	 // Look for the variable in the local variables first, then in the globals
	 auto localVarIt = currentLocals.find(expr->name);
	 if (localVarIt != currentLocals.end()) {
		 std::cout << "Assignment Expression: Found variable: " << expr->name << " in currentLocals" << std::endl;
		 builder.CreateStore(valueToAssign, localVarIt->second);
		 return;
	 }
	 else {
		 auto globalVarIt = globals.find(expr->name);

		 if (globalVarIt != globals.end()) {
			 std::cout << "Assignment Expression: Found variable: " << expr->name << " in globals assignment expression" << std::endl;
			 if (valueToAssign->getType() != globalVarIt->second->getValueType()) {
				 llvm::errs() << "Type mismatch between value to assign and target global variable.\n";
				 // Optionally print the types for debugging
				 valueToAssign->getType()->print(llvm::errs());
				 llvm::errs() << "\n";
				 globalVarIt->second->getType()->print(llvm::errs());
				 llvm::errs() << "\n";
				 return;
			 }

			 builder.CreateStore(valueToAssign, globalVarIt->second);
			 std::cout << "Was able to create store instruction" << std::endl;
			 return;
		 }
		 else {
			 std::cerr << "Variable " << expr->name << " not found." << std::endl;
			 lastValue = nullptr;
			 return;
		 }
	 }
 }


 void LLVMCodeGen::visit(const FunctionDefinition* funcDef) {
	 std::cout << funcDef->toString() << std::endl;
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
		 else if (param.type == "string") {
			 type = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context));
		 }
		 else if (param.type == "void") {
			 type = llvm::Type::getVoidTy(context);
		 }
		 // Add more type mappings as necessary
		 if (type) {
			 paramTypes.push_back(type);
		 }
		 else {
			 std::cerr << "Unsupported parameter type: " << param.type << std::endl;
			 lastValue = nullptr;
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
	 else if (funcDef->returnType == "string") { 
		 returnType = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context));
	 }
	 else if (funcDef->returnType == "void") {
		 returnType = llvm::Type::getVoidTy(context);
	 }

	 llvm::FunctionType* functionType = llvm::FunctionType::get(returnType, paramTypes, false);
	 llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, funcDef->name, module);
	 std::cout << "Function created\n";	
	 // Create a new basic block to start insertion into.
	 llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(context, "entry", function);
	 builder.SetInsertPoint(entryBB);

	 currentFunction = function; // Track the current function
	 
	 // Record the function arguments in the NamedValues map.
	 unsigned idx = 0;
	 for (auto it = function->arg_begin(); it != function->arg_end(); ++it) {
		 llvm::Argument& arg = *it;
		 
		 arg.setName(funcDef->parameters[idx].name);

		 // Create an alloca for this variable.
		 llvm::AllocaInst* alloca = llvm_util::createEntryBlockAlloca(function, std::string(arg.getName()), arg.getType());

		 // Store the initial value into the alloca.
		 builder.CreateStore(&arg, alloca);

		 // Add arguments to variable currentLocals
		 currentLocals[arg.getName().str()] = alloca;

		 idx++;
	 }

	 // Visit each statement in the function body to generate their IR
	 for (const auto& stmt : funcDef->body) {
		 std::cout << "Visiting function body" << std::endl;
		 stmt->accept(this);
	 }

	 std::cout << "Finished visiting function body" << std::endl;

	 //After visiting all statements, ensure the current block has a terminator
	 auto currentBB = builder.GetInsertBlock();
	 if (!currentBB->getTerminator()) {
		// Handle other types or throw an error indicating a missing return
		std::cout << "Error: Missing return statement in non-void function '" << funcDef->name << "'.\n"; 
	 }

	 currentFunction = nullptr; // Clear the current function
	 //currentLocals.clear();

	 // Reset the builder's insert point
	 builder.ClearInsertionPoint();

	 std::cout << "Resetted builder's insertion point" << std::endl;

     // Verify the generated code, checking for consistency.
	 llvm::verifyFunction(*function, &llvm::errs());

	 std::cout << "Function definition visited" << std::endl;
 }

 void LLVMCodeGen::visit(const FunctionCall* call) {
		// Step 1: Find the LLVM function in the current module by name.
		
	    ensureMainFunctionExist();
		
		std::cout << "Main function existed in FunctionCall" <<	std::endl;

		
		std::cout << "Function call name: " << call->name << std::endl;

		llvm::Function* calleeFunction = module->getFunction(call->name);
		if (!calleeFunction) {
			std::cerr << "Unknown function referenced: " << call->name << std::endl;
			return;
		}

		std::cout << "Found function in module" << std::endl;

		// Step 2: Evaluate the arguments and prepare them for the call instruction.
		std::vector<llvm::Value*> argsValues;
		for (auto& arg : call->arguments) {
			llvm::Value* argValue = evaluateExpression(arg.get());
			std::cout << "Evaluated an argument for function call\n";
			if (!argValue) {
				std::cout << "Argument evaluation failed for function call: " << call->name << std::endl;
				std::cerr << "Argument evaluation failed for function call: " << call->name << std::endl;
				return;
			}
			argsValues.push_back(argValue);
		}

		std::cout << "Evaluated arguments for function call" << std::endl;

		// Step 3: Create the call instruction.

		if (!builder.GetInsertBlock()) {
			std::cerr << "No insertion block set for IRBuilder.\n";
			return;
		}
		
		llvm::CallInst* callInst = builder.CreateCall(calleeFunction, argsValues);
		
		std::cout << "Created call instruction" << std::endl;

		functionCalls.push_back(call->name);

		std::cout << "Added function call to functionCalls\n";

		// For demonstration purposes, we'll assume the function calls do not return void.
		lastValue = callInst;

		std::cout << "Last value for call instruction: " << lastValue << std::endl;

		std::cout << "Function call visited" << std::endl;
}