#pragma once

#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include "../frontend/AST.hpp"
#include <memory>

class SymbolTable {
public:
    SymbolTable(const std::vector<std::unique_ptr<AST::Statement>>& statements);
    ~SymbolTable() = default;

    unsigned getOffset() const;

private:
    struct Scope {
        struct DeclarationInfo {
            DeclarationInfo(unsigned offset, const Tokenization::Token& declarationToken);

            const Tokenization::Token& declarationToken;
            unsigned offset;
        };

        std::unordered_map<std::string, DeclarationInfo> symbols;
        unsigned savedOffset = 0;
    };

    std::vector<Scope> scopes;

    void enterScope();
    void leaveScope();

    void declare(AST::VariableData& variable);
    void resolve(AST::VariableData& variable);

    bool validateStatement(const std::unique_ptr<AST::Statement>& statement);
    bool validateExpression(const std::unique_ptr<AST::Expression>& expression);

    const Scope::DeclarationInfo& getDeclarationInfo(const AST::VariableData& variable);
    
    unsigned currentOffset = 0;
    unsigned maxOffset = 0;
};