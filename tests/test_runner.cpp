#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>

#include "lexer/Lexer.h"
#include "parser/Parser.h"

void runTest(const std::string& filePath) {
    std::ifstream testFile(filePath);
    if (!testFile) {
        std::cerr << "Failed to open test file: " << filePath << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << testFile.rdbuf();
    std::string code = buffer.str();

    Lexer lexer(code.c_str());
    Parser parser(lexer);

    std::filesystem::path testPath = filePath;
    std::string filename = testPath.filename().string();

    try {
        auto declaration = parser.parseDeclaration();
        std::cout << "Test passed:" << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << filename << " with error: " << "| " << e.what()  << " |" << std::endl;
    }
}

int main() {
    // List of test files
    std::vector<std::string> testFiles = {
        "../../../../tests/test_int_declaration_success.ssl",
        "../../../../tests/test_int_declaration_fail.ssl",
    };

    for (const auto& filePath : testFiles) {
        runTest(filePath);
    }

    return 0;
}
