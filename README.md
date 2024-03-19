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

Ensure you have CMake and a C++17 compatible compiler installed. Then run:
```
 mkdir build && cd build
 cmake ..
 cmake --build .
```
3. **Run SSLang:**

After building, you can start using the SSLang compiler with:

```
./sslang <your-source-file>.ssl
```

This will compile your SSLang source file to LLVM IR, ready for further compilation to your target architecture.

## Documentation

For more detailed information on SSLang's syntax and features, refer to the [Documentation](docs/) directory.

## License

SSLang is open-source software licensed under the MIT license. See the [LICENSE](LICENSE) file for more details.
