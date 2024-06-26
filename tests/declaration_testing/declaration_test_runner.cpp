#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

//#include "lexer/Lexer.h"
//#include "parser/Parser.h"
#include "../../include/lexer/Lexer.h"
#include "../../include/parser/Parser.h"

void runTestForLine(const std::string& line, int lineNumber, const std::string& filePath) {
    // Convert the string line to const char* when passing to the Lexer
    Lexer lexer(line.c_str());
    Parser parser(lexer);

    std::filesystem::path testPath = filePath;
    std::string filename = testPath.filename().string();

    try {
        auto declaration = parser.parseDeclaration();
        std::cout << "\033[32mTest Passed\033[0m" << " Line: " << lineNumber << " in " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "\033[31mTest Failed\033[0m" << " Line: " << lineNumber << " in " << filename << " with error: " << e.what() << std::endl;
    }
}

void runTestsInFile(const std::string& filePath) {
    std::ifstream testFile(filePath);
    if (!testFile) {
        std::cerr << "Failed to open test file: " << filePath << std::endl;
        return;
    }

    std::string line;
    int lineNumber = 1;
    while (getline(testFile, line)) {
        if (!line.empty()) { // Skip empty lines
            runTestForLine(line, lineNumber, filePath);
        }
        lineNumber++;
    }
}

int main() {
    // List of test files
    std::vector<std::string> testFiles = {
        "../../tests/declaration_testing/test_declarations.ssl",
    };

    for (const auto& filePath : testFiles) {
        runTestsInFile(filePath);
    }

    return 0;
}