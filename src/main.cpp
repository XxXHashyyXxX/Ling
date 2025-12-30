#include <iostream>
#include <fstream>
#include "frontend/Tokens.hpp"
#include "frontend/Parser.hpp"

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
    std::vector<Token>::const_iterator it = tokens.begin();
    auto result = Parser::parseExpression(tokens, it);
    auto resultValue = result->getValue();
    if(!resultValue.has_value()) 
        std::cout << "Expression has no clear value" << std::endl;
    else
        std::cout << "Expression value: " << resultValue.value() << std::endl;
    
    return 0;
}