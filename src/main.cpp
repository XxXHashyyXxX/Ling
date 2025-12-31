#include <iostream>
#include <fstream>
#include "frontend/Tokens.hpp"
#include "frontend/Parser.hpp"
#include "backend/SymbolTable.hpp"


int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " [fileToCompile]\n";
        return 1;
    }

    std::ifstream source(argv[1], std::ios::binary);
    source.seekg(0, std::ios::end);
    std::size_t size = source.tellg();
    source.seekg(0);

    std::string buffer(size, '\0');
    source.read(buffer.data(), size);

    auto tokens = Tokenization::tokenize(buffer);
    auto result = Parser::parseTokens(tokens);

    for(const auto& statement : result)
    {
        if(auto* ptr = dynamic_cast<VariableDeclaration*>(statement.get())) {
            std::cout   <<  "Variable declaration:\n"
                            "\tVariable name: " << ptr->identificator << "\n"
                        <<  "\tVariable value: " << ptr->value->getValue().value() << std::endl;
        }
        else if(auto* ptr = dynamic_cast<VariableAssignment*>(statement.get())) {
            std::cout   <<  "Variable assignment:\n"
                            "\tVariable name: " << ptr->identificator << "\n"
                        <<  "\tVariable value: " << ptr->value->getValue().value() << std::endl;
        }
        else if(auto* ptr = dynamic_cast<IfStatement*>(statement.get())) {
            std::cout   <<  "If statement:\n"
                            "\tCondition: " << ptr->condition->getValue().value() << std::endl;
        }
        else if(auto* ptr = dynamic_cast<WhileStatement*>(statement.get())) {
            std::cout   <<  "While statement:\n"
                            "\tCondition: " << ptr->condition->getValue().value() << std::endl;
        }
        else if(auto* ptr = dynamic_cast<DisplayStatement*>(statement.get())) {
            std::cout   <<  "Display statement:\n"
                            "\tVariable name: " << ptr->identificator << std::endl;
        }
    }

    SymbolTable table(result);
    for(auto symbol : table)
    {
        std::cout << "[Symbol table] Declared symbol: " << symbol << "\n";
    }
    
    return 0;
}