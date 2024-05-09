# SSLang Documentation
Welcome to the official documentation for SSLang, a simple programming language designed with my first and last initials. SSLang is built on top of LLVM IR, which right now targets my local machine, x_86_64 windows msvc. This document covers the basics of SSLang syntax, its core features, and examples.

## Introduction
SSLang (Samuel Sau Language) is a statically typed, imperative programming language that was built in mind from 4 years ago when I started learning to program. It supports basic data types, control flow statements, functions, and more.

## Getting Started
Before diving into SSLang, ensure you have the SSLang compiler installed on your system. This compiler translates SSLang code into LLVM IR, which is then compiled to machine code for execution.

## Syntax and Examples
### Data Types and Declarations
SSLang supports four primary data types: int, float, string, and boolean. Declarations are straightforward:
```
int w = 2;
flt x = 3.3;
str y = "Hello World";
bool z = true;
```
### Printing to the Console
To print a value to the console, use the log statement:
```
log(x); // Prints the value of x
```
### Functions
Defining functions in SSLang is done using the function keyword, followed by the function name, arguments, and return type:
```
function validateInt(arg1, arg2) -> returnType {
    // Function body
}
```
### Calling a function is equally simple:
```
call validateInt(arg1, arg2);
```
## Control Flow
SSLang provides if statements, while loops, and for loops for control flow.

### If statement:
```
if (x > y) {
    // Block of code
}
```
### While loop:
```
loop(x > 2) {
    // Block of code
}
```
### For loop:
```
loop range(2,5) {
    // Block of code
}
```
## Expressions
SSLang supports a variety of expressions, including binary expressions, assignment expressions, and unary expressions:

### Binary Expressions:
```
x > y;
x / y;
x * y;
x notEquals y;
x equals y;
```

### Assignment Expressions:
```
x = x + 1;
```

### Unary Expressions:
```
not x;
-x;
```
### Comments
To comment a line of code in SSLang, use //:
```
// This is a comment
```

## Initialize Arrays
```
int ARRAY numberList = {1,2,3,4,5,6,7,8,9,10};
```

## Conclusion
This documentation provides a basic overview of SSLang's syntax and features. I will try to add more features and fix more bugs for this.
