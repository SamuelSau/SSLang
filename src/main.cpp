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
    std::cout << "Lexer worked" << std::endl;
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

        LLVMCodeGen llvmCodeGen;

        program->accept(&llvmCodeGen); 
        
        llvm::Module* module = llvmCodeGen.getModule();
        if (module) {
            module->print(llvm::outs(), nullptr);
        }
        else {
            std::cerr << "Module is null. No IR generated." << std::endl;
        }

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
