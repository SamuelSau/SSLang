#include "symbolTable/SymbolTable.h"
#include <iostream>

void SymbolTable::enterScope() {
    currentScopeId++;
    scopes.push({}); // Push a new scope onto the stack
}

void SymbolTable::leaveScope() {
    if (!scopes.empty()) {
        scopes.pop(); // Pop the current scope from the stack
        currentScopeId--;
    }
}

bool SymbolTable::addVariable(const std::string& name, const std::string& type) {
    if (scopes.empty()) {
        return false; // No scope to add a variable to
    }

    auto& currentScope = scopes.top();
    if (currentScope.find(name) != currentScope.end()) {
        return false; // Variable already declared in this scope
    }

    // Add the new variable to the current scope
    SymbolInfo info = {type, currentScopeId};
    currentScope[name] = info;
    return true;
}

bool SymbolTable::isDeclared(const std::string& name) {
    auto tempScopes = scopes; // Copy the stack for iteration
    while (!tempScopes.empty()) {
        const auto& scope = tempScopes.top();
        if (scope.find(name) != scope.end()) {
            return true; // Found the variable in a scope
        }
        tempScopes.pop();
    }
    return false; // Variable not found in any scope
}

std::optional<SymbolInfo> SymbolTable::getSymbolInfo(const std::string& name) {
    auto tempScopes = scopes; // Copy the stack for iteration
    while (!tempScopes.empty()) {
        const auto& scope = tempScopes.top();
        auto it = scope.find(name);
        if (it != scope.end()) {
            return it->second; // Found the variable, return its info
        }
        tempScopes.pop();
    }
    return std::nullopt; // Variable not found
}

bool SymbolTable::addFunction(const std::string& name, const FunctionInfo& info) {
    if (functions.find(name) != functions.end()) {
        return false; // Function already declared
    }
    functions[name] = info;
    return true;
}

std::optional<FunctionInfo> SymbolTable::getFunctionInfo(const std::string& name) const {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return it->second; // Found the function, return its info
    }
    return std::nullopt; // Function not found
}