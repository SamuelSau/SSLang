#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <stack>
#include <optional>
#include <iostream>

struct SymbolInfo {
    std::string type;
    int scopeId;
};

class SymbolTable {
public:
    SymbolTable() : currentScopeId(0) {}

    void enterScope();
    void leaveScope();

    bool addVariable(const std::string& name, const std::string& type);
    bool isDeclared(const std::string& name);
    
    std::optional<SymbolInfo> getSymbolInfo(const std::string& name);

private:
    std::stack<std::unordered_map<std::string, SymbolInfo>> scopes;
    int currentScopeId;
};

#endif // SYMBOL_TABLE_H