#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include "llvm/IR/IRBuilder.h"

#include "../include/lexer/Lexer.h"
#include "../include/parser/Parser.h"
#include "../include/symbolTable/SymbolTable.h"
#include "../include/semanticAnalyzer/SemanticAnalyzer.h"
#include "../include/llvmGen/LLVMCodeGen.h"

static void runTestForFile(const std::string& filePath) {
    std::ifstream testFile(filePath);
    if (!testFile) {
        std::cerr << "Failed to open test file: " << filePath << std::endl;

    }

    // Read the entire file into a string
    std::string fileContent((std::istreambuf_iterator<char>(testFile)),
        (std::istreambuf_iterator<char>()));

    // Initialize the lexer and parser with the file content
    Lexer lexer(fileContent.c_str());
    std::cout << "Lexer passed???" << std::endl;
    Parser parser(lexer);
    std::filesystem::path testPath = filePath;
    std::string filename = testPath.filename().string();

    try
    {
        auto program = parser.parseProgram();

        std::cout << "Parsed program as this: " << program->toString() << std::endl;

        SymbolTable symbolTable;

        SemanticAnalyzer semanticAnalyzer(symbolTable);

        semanticAnalyzer.visit(program.get());

        std::cout << "Program is semantically correct!" << std::endl;

        // Instantiate LLVMCodeGen and generate LLVM IR
        LLVMCodeGen llvmCodeGen;

        //program->accept(&llvmCodeGen); //when we are all done with implementation of LLVMCodeGen visitor functions
        
        std::cout << "Creating declarations...\n";

        // Create an declarations for global testing
        IntDeclaration* intDecl = new IntDeclaration("x", "5");
        FloatDeclaration* floatDecl = new FloatDeclaration("y", "5.0");
        StringDeclaration* stringDecl = new StringDeclaration("z", "Hello, World!");
        BoolDeclaration* boolDecl = new BoolDeclaration("b", "true");

        std::cout << "Creating condition from binary expression...\n";

        // Create an example condition (e.g., x < 10)
        BinaryExpression* condition = new BinaryExpression(std::make_unique<PrimaryExpression>("5"), std::make_unique<PrimaryExpression>("10"), "<");

        std::cout << "Creating AssignmentStatement...\n";

        // Create a simple body for the if statement (e.g., x = 20;)
        AssignmentStatement* assignmentStmt = new AssignmentStatement("x", std::make_unique<PrimaryExpression>("20"));
        
        std::vector<std::unique_ptr<Statement>> ifBody;
        ifBody.push_back(std::unique_ptr<Statement>(assignmentStmt));

        // Define a simple function that includes the IfStatement
        std::vector<std::unique_ptr<Statement>> functionBody;

        // Create and add the IfStatement to the function body as shown previously
        // Assume condition and assignmentStmt are defined as before

        std::cout << "Creating IfStatement...\n";
        functionBody.push_back(std::make_unique<IfStatement>(
            std::unique_ptr<Expression>(condition),
            std::make_unique<BlockStatement>(std::move(ifBody))
        ));

        std::cout << "Creating ReturnStatement...\n";  

        // Add a ReturnStatement to the function body
        functionBody.push_back(std::make_unique<ReturnStatement>(
            std::make_unique<PrimaryExpression>("5") // Return an integer value directly
        ));

        // Define the function's parameters (empty for this example)
        std::vector<ParamInfo> parameters;

        // Create the function definition
        FunctionDefinition* functionDef = new FunctionDefinition(
            "testFunction", // Function name
            parameters, // Parameters
            "int", // Return type
            std::move(functionBody) // Function body
        );

        // Visit the declarations to test the LLVM IR generation
        llvmCodeGen.visit(intDecl);
        llvmCodeGen.visit(floatDecl);
        llvmCodeGen.visit(stringDecl);
        llvmCodeGen.visit(boolDecl);

        // Visit the function definition to generate LLVM IR
        llvmCodeGen.visit(functionDef);

        // Retrieve the LLVM module and dump the IR
        llvm::Module* module = llvmCodeGen.getModule();
        if (module) {
            module->print(llvm::outs(), nullptr);
        }
        else {
            std::cerr << "Module is null. No IR generated." << std::endl;
        }

        // Cleanup
        delete intDecl;
        delete floatDecl;
        delete stringDecl;
        delete boolDecl;
        delete condition;
        delete assignmentStmt;
        delete functionDef;

        //// Output LLVM IR
        //llvm::raw_os_ostream ostream(std::cout);
        //llvmCodeGen.getModule()->print(ostream, nullptr);

        //std::cout << "\033[32mLLVM IR generation successful\033[0m" << std::endl;

        // If the parsing succeeds, it means the entire program (file content) is valid.
        std::cout << "\033[32mTest Passed\033[0m" << " in " << filename << std::endl;
    }
    catch (const std::exception& e){
        //std::cerr << "Compilation finished with errors." << std::endl;
        std::cerr << "\033[31mTest Failed\033[0m" << " in " << filename << " with error: " << e.what() << std::endl;
    }
}

int main() {
    // Adjusted for testing entire files rather than line-by-line

    std::vector<std::string> testFiles = {
        "tests/program_testing/test_programs.ssl"
    };

    for (const auto& filePath : testFiles) {
        runTestForFile(filePath); // Adjusted function call
    }

    return 0;
}
