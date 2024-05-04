#include "llvmGen/LLVMCodeGen.h"
#include "llvmGen/LLVMUtility.h"
#include "symbolTable/SymbolTable.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Type.h"
#include "llvm/ADT/StringRef.h"

LLVMCodeGen::LLVMCodeGen()
	: module(new llvm::Module("MyModule", context)), builder(context) {
	initializeExternalFunctions();
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
		 if (globalVar->getValueType()->isArrayTy() && globalVar->getValueType()->getArrayElementType()->isIntegerTy(8)) {
			 lastValue = globalVar;
			 std::cout << "Using global string variable directly: " << expr->name << std::endl;
			 return;
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
		 auto& context = builder.getContext();
		 llvm::Type* charType = llvm::Type::getInt8Ty(context);
		 auto strValue = builder.CreateGlobalStringPtr(decl->value, "globalStr");
		 llvm::ArrayType* strType = llvm::ArrayType::get(charType, decl->value.size() + 1); // +1 for null terminator
		 auto alloca = builder.CreateAlloca(strType, nullptr, decl->name);
		 builder.CreateStore(strValue, alloca);
		 
		 currentLocals[decl->name] = alloca;
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

 void LLVMCodeGen::visit(const ArrayDeclaration* decl) {
	 //llvm::Type* elementType = llvm::Type::getInt32Ty(context);  // Assuming array of integers
	 //std::vector<llvm::Constant*> initValues;

	 //// Evaluate each element expression to initialize the array
	 //for (const auto& expr : decl->elements) {
		// llvm::Value* eval = evaluateExpression(expr.get());
		// if (auto constEval = llvm::dyn_cast<llvm::ConstantInt>(eval)) {
		//	 initValues.push_back(constEval);
		// }
		// else {
		//	 std::cerr << "Non-constant expression in array initializer is not supported." << std::endl;
		//	 return;
		// }
	 //}

	 //llvm::ArrayType* arrayType = llvm::ArrayType::get(elementType, decl->elements.size());
	 //llvm::Constant* arrayInit = llvm::ConstantArray::get(arrayType, initValues);

	 //if (currentFunction) {
		// llvm::IRBuilder<> tmpBuilder(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());
		// llvm::AllocaInst* alloca = tmpBuilder.CreateAlloca(arrayType, nullptr, decl->name);
		// builder.CreateStore(arrayInit, alloca);
		// currentLocals[decl->name] = alloca;
	 //}
	 //else {
		// // Global array
		// llvm::GlobalVariable* gVar = new llvm::GlobalVariable(
		//	 *module,
		//	 arrayType,
		//	 false, // isConstant: false since it's a variable
		//	 llvm::GlobalValue::InternalLinkage,
		//	 arrayInit, 
		//	 decl->name 
		// );
		// gVar->setAlignment(llvm::MaybeAlign(4)); // Alignment for 32-bit integers
		// globals[decl->name] = gVar;
	 //}
	 //__________________________________________________________________________
	//std::cout << "Creating array declaration\n";
	//llvm::Type* elementType = llvm::Type::getInt32Ty(context); // Example with int type
	//llvm::Value* initialSize = llvm::ConstantInt::get(context, llvm::APInt(32, decl->size, true));
	//llvm::Value* arrayPtr = createDynamicArray(elementType, decl->size);

	//std::cout << "Able to create elementType, initialSize and arrayPtr\n";

	//// Assuming `createDynamicArray` correctly sets up an array with initial capacity
	//for (size_t i = 0; i < decl->size; ++i) {
	//	llvm::Value* elemVal = evaluateExpression(decl->elements[i].get());
	//	llvm::Value* elemPtr = builder.CreateGEP(elementType, arrayPtr, {
	//		llvm::ConstantInt::get(context, llvm::APInt(32, i))
	//		});
	//	builder.CreateStore(elemVal, elemPtr);
	//}

	//currentLocals[decl->name] = arrayPtr;

	 //std::cout << "Creating global array declaration\n";
	 //llvm::Type* elementType = llvm::Type::getInt32Ty(context); // Assuming arrays of int type for simplicity

	 // Initialize the IRBuilder with the correct context
	 //llvm::IRBuilder<> tmpBuilder(module->getContext());

	 // Create a temporary block for initialization (not attached to any function)
	 //llvm::BasicBlock* tempBlock = llvm::BasicBlock::Create(context, "init", nullptr);
	 //tmpBuilder.SetInsertPoint(tempBlock);

	 // Create an initial size constant
	 //llvm::Value* initialSize = llvm::ConstantInt::get(context, llvm::APInt(32, decl->size, true));

	 // Create the dynamic array at global scope
	 //llvm::Value* arrayPtr = createDynamicArray(elementType, decl->size);

	 // Register the global array in the module
	 //std::string arrayName = decl->name; // Ensure `name` is part of ArrayDeclaration
	 //llvm::GlobalVariable* globalArray = new llvm::GlobalVariable(
		// *module,
		// llvm::PointerType::getUnqual(elementType), // Pointer to the array type
		// false, // isConstant
		// llvm::GlobalValue::ExternalLinkage,
		// nullptr, // No initializer here, initializer is handled in `createDynamicArray`
		// arrayName
	 //);

	 //std::cout << "Registered global array in module" << std::endl;

	 // Store the array pointer in the global variable
	 //tmpBuilder.CreateStore(arrayPtr, globalArray);

	 //std::cout << "Stored array pointer in global variable" <<std::endl;

	 // Cleanup: Remove the temporary block after use
	 //tempBlock->eraseFromParent();

	 //std::cout << "Global array " << arrayName << " created and registered.\n";
	 //__________________________________________________________________________

	 std::cout << "Creating global array declaration" << std::endl;
	 llvm::Type* elementType = llvm::Type::getInt32Ty(context); // Assuming arrays of int type for simplicity

	 // Define a unique name for initialization function
	 std::string initFuncName = decl->name + "_init";
	 llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
	 llvm::Function* initFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, initFuncName, module);
	 llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", initFunc);
	 llvm::IRBuilder<> builder(entry);
	 std::cout << "Initialized function for array declaration" << std::endl;

	 // Create the dynamic array
	 llvm::Value* arrayPtr = createDynamicArray(elementType, decl->size);

	 // Register the global array in the module
	 llvm::GlobalVariable* globalArray = new llvm::GlobalVariable(
		 *module,
		 llvm::PointerType::getUnqual(elementType), // Pointer to the array type
		 false, // isConstant
		 llvm::GlobalValue::ExternalLinkage,
		 nullptr, // No initializer here, initializer is created dynamically
		 decl->name
	 );

	 // Store the array pointer in the global variable
	 builder.CreateStore(arrayPtr, globalArray);

	 // Finish the function
	 builder.CreateRetVoid();

	 std::cout << "Global array " << decl->name << " created and registered with initializer.\n";

	 // Save the global variable in globals for later reference
	 globals[decl->name] = globalArray;


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
		 llvm::Value* castedValueToPrint = valueToPrint;
		 formatStr = builder.CreateGlobalStringPtr("%f\n", "printedFloatInt");
		 castedValueToPrint = builder.CreateFPExt(valueToPrint, llvm::Type::getDoubleTy(context), "floatToDouble");

		 std::vector<llvm::Value*> printfArgs = { formatStr, castedValueToPrint };
		 builder.CreateCall(printfFunc, printfArgs);
		 std::cout << "Finished float print statement" << std::endl;
		 return;
	 }

	 else if (valueToPrint->getType()->isIntegerTy(1)) { //boolean
		 // Boolean
		 std::cout << "Boolean value to print" << std::endl;
		 formatStr = builder.CreateGlobalStringPtr("%s\n", "printedFormatBool");
		 // Convert the boolean to a string for printing
		 llvm::Value* trueStr = builder.CreateGlobalStringPtr("true", "trueStr");
		 llvm::Value* falseStr = builder.CreateGlobalStringPtr("false", "falseStr");
		 valueToPrint = builder.CreateSelect(valueToPrint, trueStr, falseStr);
	 }
	 else {
		 llvm::GlobalVariable* gVar = llvm::dyn_cast<llvm::GlobalVariable>(valueToPrint);

		 if (gVar) { //handling special cases for strings as they are global variables

			 std::cout << "String value to print" << std::endl;
			 formatStr = builder.CreateGlobalStringPtr("%s\n", "printedFormatString");
			 std::vector<llvm::Value*> printfArgs = { formatStr, valueToPrint };
			 builder.CreateCall(printfFunc, printfArgs);
		 }

		 else {
			 std::cerr << "Unsupported type for print statement" << std::endl;
			 valueToPrint->getType()->print(llvm::errs());
			 llvm::errs() << "\n"; // Print the type to standard error
			 return;
		 }
	 }

	 std::vector<llvm::Value*> printfArgs = { formatStr, valueToPrint};
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

	 std::cout << "Expression name for primary expression: " << expr->name << "\n";

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

 void LLVMCodeGen::visit(const MethodCall* expr) {
	 std::cout << "Method call: " << expr->name << std::endl;
	 
	if (expr->name == "add") {
		llvm::Value* arrayPtr = globals[expr->object->getName()]; // Get the array pointer
		std::cout << "Processed the array pointer" << std::endl;
		llvm::Value* elementToAdd = evaluateExpression(expr->arguments[0].get());
		std::cout << "Processed the elementToAdd" << std::endl;
		llvm::Value* currentSize = getCurrentSize(arrayPtr); // You need to manage size separately
		std::cout << "Processed the currentSize" << std::endl;
		llvm::Value* capacity = getCapacity(arrayPtr); // Same for capacity
		std::cout << "Processed the capacity" << std::endl;
		addElementToArray(arrayPtr, elementToAdd, currentSize, capacity);
		std::cout << "Processed addElementToArray" << std::endl;
	}
	else if (expr->name == "remove") {
		llvm::Value* arrayPtr = globals[expr->object->getName()]; // Get the array pointer
		llvm::Value* currentSize = getCurrentSize(arrayPtr); // You need to manage size separately
		removeLastElementFromArray(arrayPtr, currentSize);
	}
	else {
		std::cout << "The object name is: " << expr->object->getName() << std::endl;
		std::cerr << "Unsupported method call: " << expr->name << std::endl;
	}
}


 void LLVMCodeGen::visit(const FunctionDefinition* funcDef) {

	 std::cout << "We are visiting a function definition\n";

	 std::cout << funcDef->toString() << std::endl;

	 std::cout << "Function name: " << funcDef->name << std::endl;

	 std::vector<llvm::Type*> paramTypes;

	 std::cout << "Size of parameters: " << funcDef->parameters.size() << "\n";

	 for (const auto& param : funcDef->parameters) {

		 llvm::Type* type = nullptr;
		 
		 std::cout << "Parameter name: " << param.name << "\n";

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
			 std::cout << "String parameter\n";
			 type = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context));
		 }
		 else if (param.type == "void") { 
			 type = llvm::Type::getVoidTy(context);
		 }
		 // Add more type mappings as necessary
		 if (type) {
			 std::cout << "There is a type\n";
			 paramTypes.push_back(type);
		 }
		 else {
			 std::cerr << "Unsupported parameter type: " << param.type << std::endl;
			 lastValue = nullptr;
			 return; // Skip unsupported types
		 }
	 }
	 std::cout << "last value for function definition: " << lastValue << std::endl;
	 llvm::Type* returnType = nullptr;

	 std::cout << "This is the function return type: " << funcDef->returnType << "\n";

	 if (funcDef->returnType == "int") {
		 returnType = llvm::Type::getInt32Ty(context);
	 }
	 else if (funcDef->returnType == "flt") {
		 std::cout << "Function return type is float\n";
		 returnType = llvm::Type::getFloatTy(context);
	 }
	 else if (funcDef->returnType == "bool") {
		 returnType = llvm::Type::getInt1Ty(context);
	 }
	 else if (funcDef->returnType == "str") { //return type for string not working atm
		 std::cout << "String return type\n";
		 returnType = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context));
		 std::cout << "String return type set\n";
	 }
	 else if (funcDef->returnType == "void") {
		 returnType = llvm::Type::getVoidTy(context);
	 }

	 llvm::FunctionType* functionType = llvm::FunctionType::get(returnType, paramTypes, false);
	 llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, funcDef->name, module);
	 std::cout << "Function created" << std::endl;

	 currentFunction = function; // Track the current function
	 std::cout << "Tracking current function" << std::endl;

	 // Create a new basic block to start insertion into.
	 llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(context, "entry", function);
	 builder.SetInsertPoint(entryBB);

	 builder.CreateBr(entryBB);// Branch to itself to ensure the block has a terminator
	 
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

	 //auto currentBB = builder.GetInsertBlock();
	 //After visiting all statements, ensure the current block has a terminator
	 if (!entryBB->getTerminator()) {
		 if (returnType->isVoidTy()) {
			 builder.SetInsertPoint(entryBB);
			 builder.CreateRetVoid(); // Proper terminator for void functions
		 }
		 else {
			 // You should handle non-void return types appropriately, potentially with an unreachable instruction or default return
			 llvm::Value* returnValue = llvm::Constant::getNullValue(returnType);
			 builder.SetInsertPoint(entryBB);
			 builder.CreateRet(returnValue);
		 }
	 }

	 currentFunction = nullptr; // Clear the current function

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

 void LLVMCodeGen::initializeExternalFunctions() {
	llvm::FunctionType* mallocType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), { llvm::Type::getInt64Ty(context) }, false);
	mallocFunction = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "malloc", module);

	llvm::FunctionType* reallocType = llvm::FunctionType::get(llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), { llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0), llvm::Type::getInt64Ty(context) }, false);
	reallocFunction = llvm::Function::Create(reallocType, llvm::Function::ExternalLinkage, "realloc", module);

	llvm::FunctionType* freeType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), { llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0) }, false);
	freeFunction = llvm::Function::Create(freeType, llvm::Function::ExternalLinkage, "free", module);
	 
 }

 llvm::Value* LLVMCodeGen::createDynamicArray(llvm::Type* elemType, size_t initialCapacity) {

	 //if (currentFunction && !currentFunction->getEntryBlock().empty()) {
		// auto* terminator = currentFunction->getEntryBlock().getTerminator();
		// if (terminator) {
		//	 std::cout << "Terminator is present in createDynamicArray." << std::endl;
		//	 llvm::IRBuilder<> builder(terminator);

		// }
		// else {
		//	 std::cerr << "No terminator in entry block createDynamicArray." << std::endl;
		//	 return nullptr; // or handle this scenario more gracefully
		// }
	 //}
	 //else {
		// std::cerr << "Invalid function context or missing entry block createDynamicArray." << std::endl;
		// return nullptr;
	 //}
	 ////llvm::IRBuilder<> builder(currentFunction->getEntryBlock().getTerminator());
	 //std::cout << "After creating IRBuilder" << std::endl;
	 //llvm::Value* elemSize = llvm::ConstantInt::get(context, llvm::APInt(32, module->getDataLayout().getTypeAllocSize(elemType)));
	 //std::cout << "After getting elemSize" << std::endl;
	 //llvm::Value* allocSize = builder.CreateMul(elemSize, llvm::ConstantInt::get(context, llvm::APInt(32, initialCapacity)), "allocSize");
	 //std::cout << "After getting allocSize" << std::endl;
	 //llvm::Value* arrayMem = builder.CreateCall(mallocFunction, allocSize, "arrayMem");
	 //std::cout << "After creating arrayMem" << std::endl;
	 //return builder.CreateBitCast(arrayMem, llvm::PointerType::getUnqual(elemType), "arrayPtr");\

	 //std::cout << "Creating dynamic array in global scope" << std::endl;
	 //llvm::IRBuilder<> builder(context);
	 //std::cout << "After creating IRBuilder in createDynamicArray" << std::endl;
	 //llvm::BasicBlock* tempBlock = llvm::BasicBlock::Create(context, "init", nullptr);
	 //std::cout << "After creating tempBlock in createDynamicArray" << std::endl;
	 //builder.SetInsertPoint(tempBlock);
	 //std::cout << "After setting insert point in createDynamicArray" << std::endl;
	 //llvm::Value* elemSize = llvm::ConstantInt::get(context, llvm::APInt(32, module->getDataLayout().getTypeAllocSize(elemType)));
	 //std::cout << "After getting elemSize in createDynamicArray" << std::endl;
	 //llvm::Value* allocSize = builder.CreateMul(elemSize, llvm::ConstantInt::get(context, llvm::APInt(32, initialCapacity)), "allocSize");
	 //std::cout << "After getting allocSize in createDynamicArray" << std::endl;
	 //llvm::Value* arrayMem = builder.CreateCall(mallocFunction, allocSize, "arrayMem");
	 //std::cout << "After creating arrayMem in createDynamicArray" << std::endl;

	 //// Cleanup: Remove the temporary block after use
	 //tempBlock->eraseFromParent();
	 //std::cout << "After erasing tempBlock in createDynamicArray" << std::endl;

	 //return builder.CreateBitCast(arrayMem, llvm::PointerType::getUnqual(elemType), "arrayPtr");
	 //______________________________________________________________________
	 std::cout << "Creating dynamic array in global scope" << std::endl;

	 // Create a dummy function to host the block if not already within a function
	 llvm::FunctionType* funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), false);
	 llvm::Function* dummyFunc = llvm::Function::Create(funcType, llvm::Function::InternalLinkage, "dummyFunc", module);
	 llvm::BasicBlock* tempBlock = llvm::BasicBlock::Create(context, "init", dummyFunc);

	 llvm::IRBuilder<> builder(tempBlock);
	 std::cout << "IRBuilder and tempBlock initialized." << std::endl;

	 llvm::Value* elemSize = llvm::ConstantInt::get(context, llvm::APInt(32, module->getDataLayout().getTypeAllocSize(elemType)));
	 llvm::Value* allocSize = builder.CreateMul(elemSize, llvm::ConstantInt::get(context, llvm::APInt(32, initialCapacity)), "allocSize");
	 llvm::Value* arrayMem = builder.CreateCall(mallocFunction, allocSize, "arrayMem");
	 std::cout << "Initialized sizes and allocated memory for array." << std::endl;
	 // Now you can safely use the temporary block, knowing it is properly contained.
	 llvm::Value* arrayPtr = builder.CreateBitCast(arrayMem, llvm::PointerType::getUnqual(elemType), "arrayPtr");
	 std::cout << "Bitcasted array memory to array pointer." << std::endl;
	 // Cleanup
	 tempBlock->eraseFromParent();  // Safe to remove now; all uses are complete
	 dummyFunc->eraseFromParent();  // Remove the dummy function as well
	 std::cout << "Erased temporary block and dummy function. Returning array pointer" << std::endl;
	 return arrayPtr;

 }

 llvm::Value* LLVMCodeGen::getCurrentSize(llvm::Value* arrayPtr) {
	 //std::cout << "Getting current size of array" << std::endl;

	 //if (currentFunction && !currentFunction->getEntryBlock().empty()) {
		// auto* terminator = currentFunction->getEntryBlock().getTerminator();
		// if (terminator) {
		//	 std::cout << "Terminator is present in getCurrentSize." << std::endl;
		//	 llvm::IRBuilder<> builder(terminator);
		//	 
		// }
		// else {
		//	 std::cerr << "No terminator in entry block getCurrentSize." << std::endl;
		//	 return nullptr; // or handle this scenario more gracefully
		// }
	 //}
	 //else {
		// std::cerr << "Invalid function context or missing entry block getCurrentSize." << std::endl;
		// return nullptr;
	 //}
	 //std::cout << "After creating IRBuilder in getCurrentSize." << std::endl;
	 //// Adjusting for the actual type of size in your array structure
	 //llvm::PointerType* sizePtrType = llvm::PointerType::get(llvm::Type::getInt32Ty(context), 0);
	 //std::cout << "Pointer type for sizePtrType created." << std::endl;
	 //llvm::Value* sizePtr = builder.CreatePointerCast(arrayPtr, sizePtrType, "sizePtrCast");
	 //std::cout << "Pointer cast to size pointer completed." << std::endl;

	 //// Load should match the type that sizePtr points to; assuming it is indeed int32
	 //return builder.CreateLoad(llvm::Type::getInt32Ty(context), sizePtr, "currentSize");

	 std::cout << "Getting current size of array" << std::endl;

	 if (!currentFunction || currentFunction->getEntryBlock().empty()) {
		 std::cerr << "Invalid function context or missing entry block in getCurrentSize." << std::endl;
		 return nullptr;
	 }

	 // Ensure there is a terminator to attach the builder to
	 llvm::Instruction* terminator = currentFunction->getEntryBlock().getTerminator();
	 if (!terminator) {
		 std::cerr << "No terminator in entry block of getCurrentSize." << std::endl;
		 return nullptr; // Or handle this scenario more gracefully
	 }

	 llvm::IRBuilder<> builder(terminator);
	 std::cout << "IRBuilder successfully created in getCurrentSize." << std::endl;

	 // Adjusting for the actual type of size in your array structure
	 llvm::PointerType* sizePtrType = llvm::PointerType::get(llvm::Type::getInt32Ty(context), 0);
	 std::cout << "Pointer type for size pointer created." << std::endl;

	 llvm::Value* sizePtr = builder.CreatePointerCast(arrayPtr, sizePtrType, "sizePtrCast");
	 std::cout << "Pointer cast to size pointer completed." << std::endl;

	 // Load should match the type that sizePtr points to; assuming it is indeed int32
	 return builder.CreateLoad(llvm::Type::getInt32Ty(context), sizePtr, "currentSize");

 }


llvm::Value* LLVMCodeGen::getCapacity(llvm::Value* arrayPtr) {
	llvm::IRBuilder<> builder(currentFunction->getEntryBlock().getTerminator());
	std::cout << "Getting capacity of array" << std::endl;
	llvm::Type* intType = llvm::Type::getInt32Ty(context);  // Assuming the capacity is stored as an int
	std::cout << "Created intType" << std::endl;
	// Assuming arrayPtr points to a structure where capacity is the first element,
	// and that the structure's type is properly set up in your LLVM context
	std::vector<llvm::Value*> indices{
		llvm::ConstantInt::get(intType, 0),  // index for the first element if it's a simple struct/array
		llvm::ConstantInt::get(intType, 1)   // assuming capacity is at index 1
	};
	std::cout << "Created indices" << std::endl;

	// Now using CreateGEP with the correct number of arguments
	llvm::Value* capacityPtr = builder.CreateGEP(intType, arrayPtr, indices, "capacityPtr");
	std::cout << "Created capacityPtr" << std::endl;
	llvm::Value* capacity = builder.CreateLoad(intType, capacityPtr, "loadCapacity");
	std::cout << "Created capacity" << std::endl;
	 return builder.CreateLoad(llvm::Type::getInt64Ty(context), capacityPtr, "capacity");
 }


 void LLVMCodeGen::addElementToArray(llvm::Value* arrayPtr, llvm::Value* element, llvm::Value* currentSize, llvm::Value* capacity) {
	 llvm::IRBuilder<> builder(currentFunction->getEntryBlock().getTerminator());

	 // Check if resize is needed
	 llvm::Value* needResize = builder.CreateICmpUGE(currentSize, capacity, "needResize");
	 llvm::BasicBlock* resizeBlock = llvm::BasicBlock::Create(context, "resize", currentFunction);
	 llvm::BasicBlock* continueBlock = llvm::BasicBlock::Create(context, "continue", currentFunction);
	 builder.CreateCondBr(needResize, resizeBlock, continueBlock);

	 // Resize the array
	 builder.SetInsertPoint(resizeBlock);
	 llvm::Value* newCapacity = builder.CreateMul(capacity, llvm::ConstantInt::get(context, llvm::APInt(32, 2)), "newCapacity");
	 llvm::Value* newSize = builder.CreateMul(newCapacity, llvm::ConstantInt::get(context, llvm::APInt(32, module->getDataLayout().getTypeAllocSize(element->getType()))), "newSize");
	 llvm::Value* newArrayMem = builder.CreateCall(reallocFunction, { arrayPtr, newSize }, "newArrayMem");
	 llvm::Value* newArrayPtr = builder.CreateBitCast(newArrayMem, llvm::PointerType::getUnqual(element->getType()), "newArrayPtr");
	 builder.CreateBr(continueBlock);

	 // Continue adding element
	 builder.SetInsertPoint(continueBlock);
	 llvm::PHINode* finalArrayPtr = builder.CreatePHI(arrayPtr->getType(), 2, "finalArrayPtr");
	 finalArrayPtr->addIncoming(arrayPtr, currentFunction->getEntryBlock().getTerminator()->getParent());
	 finalArrayPtr->addIncoming(newArrayPtr, resizeBlock);

	 // Add the element
	 llvm::Value* index[] = { currentSize };
	 llvm::Value* elemPtr = builder.CreateInBoundsGEP(element->getType(), finalArrayPtr, index);
	 builder.CreateStore(element, elemPtr);

	 // Update size
	 llvm::Value* updatedSize = builder.CreateAdd(currentSize, llvm::ConstantInt::get(context, llvm::APInt(32, 1)), "updatedSize");
	 builder.CreateStore(updatedSize, currentSize);
 }

 void LLVMCodeGen::removeLastElementFromArray(llvm::Value* arrayPtr, llvm::Value* currentSize) {
	 llvm::IRBuilder<> builder(currentFunction->getEntryBlock().getTerminator());
	 llvm::Value* updatedSize = builder.CreateSub(currentSize, llvm::ConstantInt::get(context, llvm::APInt(32, 1)), "updatedSize");
	 builder.CreateStore(updatedSize, currentSize);
 }