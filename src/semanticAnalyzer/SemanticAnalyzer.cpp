#include "../../include/semanticAnalyzer/SemanticAnalyzer.h"
#include "../../include/symbolTable/SymbolTable.h"
#include "../../include/ast/ASTNodes.h"

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& symbolTable) : symbolTable(symbolTable) {}

//Implementation of the visit methods

void SemanticAnalyzer::visit(const IntDeclaration* decl) {
    if (symbolTable.isDeclared(decl->name)) {
       throw std::runtime_error("int '" + decl->name + "' is already declared in this scope.");
    }
    symbolTable.addVariable(decl->name, "int");
    
}

void SemanticAnalyzer::visit(const FloatDeclaration* decl) {
    if (symbolTable.isDeclared(decl->name)) {
       throw std::runtime_error("flt '" + decl->name + "' is already declared in this scope.");
    }
    symbolTable.addVariable(decl->name, "float");
}

void SemanticAnalyzer::visit(const StringDeclaration* decl) {
    if (symbolTable.isDeclared(decl->name)) {
       throw std::runtime_error("str '" + decl->name + "' is already declared in this scope.");
    }
    symbolTable.addVariable(decl->name, "string");
}

void SemanticAnalyzer::visit(const BoolDeclaration* decl) {
    if (symbolTable.isDeclared(decl->name)) {
       throw std::runtime_error("bool '" + decl->name + "' is already declared in this scope.");
    }
    symbolTable.addVariable(decl->name, "bool");
}

void SemanticAnalyzer::visit(const AssignmentExpression* expr) {
    if (!insideFunction) {
        throw std::runtime_error("Assignment expressions must be inside a function definition.");
    }
    auto varInfo = symbolTable.getSymbolInfo(expr->name);
    
    if (!varInfo.has_value()) {
       //symbolTable.printContents();
       throw std::runtime_error("Variable " + expr->name + " not declared in assignment expression");
    }
    auto exprType = expr->expression->getType(symbolTable);
    if (varInfo->type != exprType) {
       //std::cout << "Variable type: " << varInfo->type << " Expression type: " << exprType << "\n";
       throw std::runtime_error("Type mismatch in assignment to " + expr->name);
    }
}

 void SemanticAnalyzer::visit(const BinaryExpression* expr) {
     if (!insideFunction) {
         throw std::runtime_error("Binary expressions must be inside a function definition.");
     }
    auto leftType = expr->left->getType(symbolTable);
    auto rightType = expr->right->getType(symbolTable);

    std::cout << "We are visiting the binary expression\n";
    std::cout << "Binary expressions: Left type: " << leftType << " Right type: " << rightType << "\n";

    if ((leftType != "int" || rightType != "int") || (leftType != "float" || rightType != "float")) {
       throw std::runtime_error("Binary expressions only supports on integers and floats.");
    }
 }

void SemanticAnalyzer::visit(const PrimaryExpression* expr) {
   
    auto type= expr->getType(symbolTable);
        
        if (!(type == "int" || type == "float" || type == "string" || type == "bool")) {
           throw std::runtime_error("Primary expressions support integers, floats, strings, and booleans.");
        }
}

void SemanticAnalyzer::visit(const UnaryExpression* expr) {

    auto exprType = expr->expr->getType(symbolTable);
    if (exprType != "int" || exprType != "float") {
       throw std::runtime_error("Unary operations only supports integers and floats.");
    }

}


void SemanticAnalyzer::visit(const PrintStatement* stmt) {
    if (!insideFunction) {
        throw std::runtime_error("Print statement must be inside a function definition.");
    }
    auto exprType = stmt->expr->getType(symbolTable);

    std::cout << "We are visiting the print statement" << std::endl;
    //std::cout << "Expression type: " << exprType << std::endl;

    if (exprType != "int" && exprType != "float" && exprType != "string" && exprType != "bool") {
       throw std::runtime_error("Print statement only supports int, float, string, and bool types");
    }
}

void SemanticAnalyzer::visit(const WhileLoopStatement* stmt) {
    if (!insideFunction) {
       throw std::runtime_error("While loops must be inside a function definition.");
    }
    symbolTable.enterScope();
    auto conditionType = stmt->condition->getType(symbolTable);
    if (conditionType != "bool") {
       throw std::runtime_error("While condition must be boolean");
    }
    
    stmt->body->accept(this);
    symbolTable.leaveScope();
}

void SemanticAnalyzer::visit(const ForLoopStatement* stmt) {
    if (!insideFunction) {
        throw std::runtime_error("For loops must be inside a function definition.");
    }
    symbolTable.enterScope();
    auto start = stmt->start->getType(symbolTable);
    auto end = stmt->end->getType(symbolTable);
    if (start != "int" || end != "int") {
       throw std::runtime_error("\"For loop\" start and end values must be integers.");
    }
   
    stmt->body->accept(this);
    symbolTable.leaveScope();
}

void SemanticAnalyzer::visit(const AssignmentStatement* stmt) {
    if (!insideFunction) {
        throw std::runtime_error("Assignment expressions must be inside a function definition.");
    }
    std::cout << "We are visiting the assignment statement\n";
    auto varInfo = symbolTable.getSymbolInfo(stmt->name);
    if (!varInfo) {
       throw std::runtime_error("Variable " + stmt->name + " not declared?!?!");
    }
    auto exprType = stmt->expression->getType(symbolTable);
    if (varInfo->type != exprType) {
       throw std::runtime_error("Type mismatch for " + stmt->name + " in assignment statement.");
    }
}

void SemanticAnalyzer::visit(const IfStatement* stmt) {
    if (!insideFunction) {
        throw std::runtime_error("If Statements must be inside a function definition");
    }
    // Check the condition's type
    auto conditionType = stmt->condition->getType(symbolTable);
    if (conditionType != "bool") {
        throw std::runtime_error("If condition must be boolean");
    }

    // Then body
    symbolTable.enterScope();
    stmt->thenBody->accept(this);
    symbolTable.leaveScope();

    // Else body, if it exists
    if (stmt->elseBody) {
        symbolTable.enterScope();
        stmt->elseBody->accept(this);
        symbolTable.leaveScope();
    }
}

void SemanticAnalyzer::visit(const ReturnStatement* stmt) {
    if (!insideFunction) {
       throw std::runtime_error("Return statement must be inside a function definition.");
    }
    auto exprType = stmt->expression->getType(symbolTable);

    if (!(exprType == "int" || exprType == "float" || exprType == "string" || exprType == "bool")) {
       throw std::runtime_error("Return statement only supports int, float, string, and bool types");
    }
    if (exprType != currentFunctionReturnType) {
       throw std::runtime_error("Return type does not match function return type.");
    }
}

void SemanticAnalyzer::visit(const BlockStatement* stmt) {
    if (!insideFunction) {
        throw std::runtime_error("Block statement must be inside a function definition.");
    }
    symbolTable.enterScope();
    
    for (const auto& statement : stmt->statements) {
        statement->accept(this);
    }

    symbolTable.leaveScope();
}

void SemanticAnalyzer::visit(const ExpressionStatement* stmt) {
    if (!insideFunction) {
        throw std::runtime_error("Expression statements must be inside a function definition.");
    }
     stmt->expression->accept(this);
}

void SemanticAnalyzer::visit(const FunctionDefinition* funcDef) {
    
    FunctionInfo info;
    info.name = funcDef->name;
    info.parameterInfo = funcDef->parameters;
    currentFunctionReturnType = funcDef->returnType;

    if (!symbolTable.addFunction(funcDef->name, info)) {
       throw std::runtime_error("Function " + funcDef->name + " is already declared.");
    }

    symbolTable.enterScope();
    insideFunction = true;

    for (const auto& param : funcDef->parameters) {
        if (!symbolTable.addVariable(param.name, param.type)) {
           throw std::runtime_error("Parameter " + param.name + " is already declared.");
        }
    }

    for (const auto& stmt : funcDef->body) {
        stmt->accept(this);
    }

    symbolTable.leaveScope();
    insideFunction = false;
}

void SemanticAnalyzer::visit(const FunctionCall* call) {
    std::cout << "Function call: <<" << call->name << ">>\n";
    auto funcInfo = symbolTable.getFunctionInfo(call->name);
    if (!funcInfo) {
       throw std::runtime_error("Function " + call->name + " not declared.");
    }

    // Check if the number of arguments in the call matches the number of parameters in the function definition
    if (call->arguments.size() != funcInfo->parameterInfo.size()) {
        throw std::runtime_error("Function '" + call->name + "' called with incorrect number of arguments. Expected " + std::to_string(funcInfo->parameterInfo.size()) + ", got " + std::to_string(call->arguments.size()) + ".");
    }

}

void SemanticAnalyzer::visit(const Program* program) {
    
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

}