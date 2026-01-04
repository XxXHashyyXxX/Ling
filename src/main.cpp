#include <iostream>
#include <fstream>
#include <optional>
#include "frontend/Tokens.hpp"
#include "frontend/Parser.hpp"
#include "backend/SymbolTable.hpp"
#include "backend/IR.hpp"
#include "backend/CodeGen.hpp"

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [fileToCompile]\n";
        return 1;
    }

    std::string src;
    bool fullCompile = true;

    for(int i = 1; i < argc; ++i)
    {
        if(argv[i][0] != '-')
        {
            src = argv[i];
            continue;
        }

        if(argv[i] == std::string("-s"))
            fullCompile = false;
    }

    std::string srcPath = src + ".ling";
    std::ifstream source(srcPath, std::ios::binary);
    source.seekg(0, std::ios::end);
    std::size_t size = source.tellg();
    source.seekg(0);

    std::string buffer(size, '\0');
    source.read(buffer.data(), size);

    std::vector<Tokenization::Token> tokens;
    try 
    {
        tokens = Tokenization::tokenize(buffer);
    }
    catch(std::exception& e)
    {
        std::cerr << "\n\tCompilation error during tokenization:\n" << e.what() << "\n";
        return -1;
    }

    std::vector<Token>::const_iterator it = tokens.begin();
    std::vector<std::unique_ptr<AST::Statement>> result;
    try
    {
        result = Parser::parseTokens(tokens, it);
    }
    catch(std::exception& e)
    {
        std::cerr << "\n\tCompilation error during parsing:\n" << e.what() << "\n";
        return -1;
    }

    std::optional<SymbolTable> optionalTable;

    try
    {
        optionalTable.emplace(result);
    }
    catch(std::exception& e)
    {
        std::cerr << "\n\tCompilation error during variable resolution:\n" << e.what() << "\n";
        return -1;
    }

    SymbolTable table = *optionalTable;
    BuilderIR ir(result);

    CodeGen gen(ir, table);

    if(fullCompile) gen.generateExecutable(src);
    else gen.generateAssembly(src);
    
    return 0;
}