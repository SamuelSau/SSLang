#include "symbolTable/SymbolTable.h"
#include <iostream>

void SymbolTable::enterScope() {
    currentScopeId++;
    //std::cout << "Entering new scope, current scope depth: " << currentScopeId << std::endl;
    scopes.push({}); 
}

void SymbolTable::leaveScope() {
    if (!scopes.empty()) {
        //std::cout << "Leaving scope, current scope depth before leaving: " << scopes.size() << std::endl;
        scopes.pop(); 
        currentScopeId--;
    }
}

bool SymbolTable::addVariable(const std::string& name, const std::string& type) {
    //std::cout << "Adding a variable with name: " << name << " and type: " << type << "\n";
    
    if (scopes.empty()) {
        return false;
    }

    auto& currentScope = scopes.top();
    if (currentScope.find(name) != currentScope.end()) {
        return false; 
    }

    SymbolInfo info = {type, currentScopeId};
    currentScope[name] = info;
    //std::cout << "printing info: " << info.type << " " << info.scopeId << "\n";
    return true;
}

bool SymbolTable::isDeclared(const std::string& name) {
    auto tempScopes = scopes; 
    while (!tempScopes.empty()) {
        const auto& scope = tempScopes.top();
        if (scope.find(name) != scope.end()) {
            return true; // Found the variable in a scope
        }
        tempScopes.pop();
    }
    return false;
}

std::optional<SymbolInfo> SymbolTable::getSymbolInfo(const std::string& name) {
    auto tempScopes = scopes; 
    while (!tempScopes.empty()) {
        const auto& scope = tempScopes.top();
        auto it = scope.find(name);
        if (it != scope.end()) {
            // std::cout << "Found variable in scope: " << it->second.scopeId << "\n";
            // std::cout << "The type of the variable is: " << it->second.type << "\n";
            return it->second; 
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
        return it->second; 
    }
    return std::nullopt; // Function not found
}

void SymbolTable::printContents() const {
    
    std::stack<std::unordered_map<std::string, SymbolInfo>> tempStack(scopes);

    size_t scopeLevel = tempStack.size();
    //std::cout << "Size of stack: " << tempStack.size() << "\n";
    while (!tempStack.empty()) {
        const auto& scope = tempStack.top(); 
        //std::cout << "Scope level " << scopeLevel << ":\n";
        
        for (const auto& [name, symbolInfo] : scope) {
            //std::cout << "  Name: " << name << ", Type: " << symbolInfo.type << ", Scope ID: " << symbolInfo.scopeId << "\n";
        }

        tempStack.pop();
        scopeLevel--;
    }
}

