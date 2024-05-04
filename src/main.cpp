#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LLVMContext.h"

#include "../include/lexer/Lexer.h"
#include "../include/parser/Parser.h"
#include "../include/symbolTable/SymbolTable.h"
#include "../include/semanticAnalyzer/SemanticAnalyzer.h"
#include "../include/llvmGen/LLVMCodeGen.h"
#include "llvmOptimize/LLVMOptimizer.h" 
#include "generateMachineCode/genObjFile.h"


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
    std::string unoptimizedFilename = "llvmGenerated/unoptimized_" + testPath.filename().replace_extension(".ll").string();
    std::string optimizedFilename = "llvmGenerated/optimized_" + testPath.filename().replace_extension(".ll").string();

    try {
        auto program = parser.parseProgram();
        std::cout << "Parsed program as " << program->toString() << std::endl;
        std::cout << "Parsed program successfully\n";
        SymbolTable symbolTable;
        std::cout << "Created symbol table\n";
        SemanticAnalyzer semanticAnalyzer(symbolTable);
        std::cout << "Visiting program for semantic analysis\n";
        semanticAnalyzer.visit(program.get());
        std::cout << "Semantic analysis successful\n";
        LLVMCodeGen llvmCodeGen;
        std::cout << "LLVMCodeGen object created successfully\n";
        program->accept(&llvmCodeGen);
        std::cout << "Visited program for llvm codegen successfully\n";
        
        llvm::Module* module = llvmCodeGen.getModule();
        if (module) {

            {
                std::error_code EC;
                llvm::raw_fd_ostream dest(unoptimizedFilename, EC);

                if (EC) {
                    std::cerr << "Could not open file: " << EC.message() << ", for writing the LLVM IR to " << unoptimizedFilename << std::endl;
                    return;
                }

                module->print(dest, nullptr);
                std::cout << "LLVM IR was written to " << unoptimizedFilename << std::endl;
                std::string unoptimizedObjFilename = "genObjectFile/unoptimized_" + testPath.filename().replace_extension(".o").string();
                GenerateOBJ::generateObjectFile(module, unoptimizedObjFilename);
            }

            LLVMOptimizer::optimize(module);
            
            {
                std::error_code EC;
                llvm::raw_fd_ostream dest(optimizedFilename, EC);

                if (EC) {
                    std::cerr << "Could not open file: " << EC.message() << ", for writing the LLVM IR to " << optimizedFilename << std::endl;
                    return;
                }

                module->print(dest, nullptr);
                std::cout << "LLVM IR was written to " << optimizedFilename << std::endl;
                std::string optimizedObjFileName = "genObjectFile/optimized_" + testPath.filename().replace_extension(".o").string();
                GenerateOBJ::generateObjectFile(module, optimizedObjFileName);
            }
            //assume you have another compiler installed like clang to generate the executable
        }

        else {
            std::cerr << "Module is null. No IR generated, no optimization, or no object file created." << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "\033[31mTest Failed\033[0m" << " with error: " << e.what() << std::endl;
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
