#pragma once

#include "AST.hpp"
#include "Tokens.hpp"
#include <vector>

using namespace Tokenization;
using namespace AST;

namespace Parser {
    std::vector<std::unique_ptr<Statement>> parseTokens(const std::vector<Token>& tokens);
    std::unique_ptr<Expression> parseExpression(const std::vector<Token>& tokens, std::vector<Token>::const_iterator& it, Token::Type terminationToken = Token::Type::ParenthesisRight);

    enum class OperatorArity {
        Unary,
        Binary
    };

    OperatorArity getOperatorArity(std::vector<Token>::const_iterator& it, const std::vector<Token>::const_iterator& start);
};