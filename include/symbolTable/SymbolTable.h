#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <stack>
#include <optional>
#include <iostream>

struct ParamInfo {
    std::string name;
    std::string type;

    ParamInfo(std::string name, std::string type) : name(std::move(name)), type(std::move(type)) {}
};

struct SymbolInfo {
    std::string type;
    int scopeId;
};

struct FunctionInfo {
    std::string name;
    std::string returnType;
    std::vector<ParamInfo> parameterInfo;
};

class SymbolTable {
    public:
        SymbolTable() : currentScopeId(0) {
            enterScope();
        }
        void enterScope();
        void leaveScope();

        bool addVariable(const std::string& name, const std::string& type);
        bool isDeclared(const std::string& name);
        
        std::optional<SymbolInfo> getSymbolInfo(const std::string& name);

        bool addFunction(const std::string& name, const FunctionInfo& info);
        std::optional<FunctionInfo> getFunctionInfo(const std::string& name) const;
        void printContents() const;

    private:
        std::stack<std::unordered_map<std::string, SymbolInfo>> scopes;
        int currentScopeId;
        std::unordered_map<std::string, FunctionInfo> functions;
};

#endif // SYMBOL_TABLE_H