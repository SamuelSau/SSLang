#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include "../include/lexer/Lexer.h"
#include "../include/parser/Parser.h"
#include "../include/symbolTable/SymbolTable.h"
#include "../include/semanticAnalyzer/SemanticAnalyzer.h"
#include "../include/llvmGen/LLVMCodeGen.h"

static void runTestForFile(const std::string& filePath) {
    std::ifstream testFile(filePath);
    if (!testFile) {
        std::cerr << "Failed to open test file: " << filePath << std::endl;
        return;
    }

    std::string fileContent((std::istreambuf_iterator<char>(testFile)),
        (std::istreambuf_iterator<char>()));
    Lexer lexer(fileContent.c_str());
    Parser parser(lexer);
    std::filesystem::path testPath = filePath;
    std::string filename = testPath.filename().replace_extension(".ll").string(); // Change extension to .ll

    try {
        auto program = parser.parseProgram();
        SymbolTable symbolTable;
        SemanticAnalyzer semanticAnalyzer(symbolTable);
        semanticAnalyzer.visit(program.get());
        LLVMCodeGen llvmCodeGen;
        program->accept(&llvmCodeGen);

        llvm::Module* module = llvmCodeGen.getModule();
        if (module) {
            std::error_code EC;
            std::string outputFilename = "llvmGenerated/" + filename; // Adjust the output directory as needed
            llvm::raw_fd_ostream dest(outputFilename, EC);

            if (EC) {
                std::cerr << "Could not open file: " << EC.message() << ", for writing the LLVM IR to " << outputFilename << std::endl;
                return;
            }

            module->print(dest, nullptr);
            std::cout << "LLVM IR was written to " << outputFilename << std::endl;
        }
        else {
            std::cerr << "Module is null. No IR generated." << std::endl;
        }
    }
    catch (const std::exception& e) {
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
