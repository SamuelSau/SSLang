# SSLang

SSLang is my programming language using my initials.

## Features

SSLang tries to be unique and simple. Key features include:

- **LLVM Backend:** By targeting LLVM IR, SSLang benefits from LLVM's optimizations and support for multiple target architectures.
- **C++17 Lexer and Parser:** The language's lexer and parser are built using modern C++ standards.

## Getting Started

To start using SSLang, you'll need to clone the repository and build the compiler. The following steps will guide you through the process:

1. **Clone the Repository:**
 ```
 git clone https://github.com/SamuelSau/SSLang.git
```
2. **Build the Compiler:**

Ensure you have CMake and a C++17 compatible compiler installed. I used Ninja for my build system. Then run:
```
 ninja
```
3. **Run Clang:**

After building, the LLVM IR will be generated in .ll files and .o files. There will be 2 files, optimized and unoptimized generated LLVM IR for testing purposes. To link the object files, you can use a compiler like Clang:

```
 clang unoptimized_test_programs.o -o unoptimized_test_programs.exe
```

This will compile your SSLang source file to an executable based on the target architecture. For now, it supports x86_64. 

## Documentation

For more detailed information on SSLang's syntax and features, refer to the [Documentation](https://github.com/SamuelSau/SSLang/blob/main/Documentation.md) directory.

## License

SSLang is open-source software licensed under the MIT license. See the [LICENSE](LICENSE) file for more details.
