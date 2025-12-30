#include "Parser.hpp"
#include <stack>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <iostream>

/**
 *  Possible statements:
 * 
 *  - Variable declaration:
 *      let [identificator] = [expression];
 * 
 *  - Variable assignment:
 *      [identificator] = [expression];
 * 
 *  - If Statement:
 *      if( [Expression] ) [Statement]
 * 
 *  - While Statement:
 *      while( [Expression] ) [Statement]
 * 
 *  - Display Statement:
 *      display [identificator];    
 */

std::vector<std::unique_ptr<Statement>> Parser::parseTokens(const std::vector<Token> &tokens)
{
    return std::vector<std::unique_ptr<Statement>>();
}

inline static const std::unordered_map<Token::Type, unsigned> binaryOperatorsPrecedence = {{
    {Token::Type::OperatorPlus, 0},
    {Token::Type::OperatorMinus, 0},
    {Token::Type::OperatorSlash, 1},
    {Token::Type::OperatorStar, 1}
}};

inline static const std::unordered_map<Token::Type, unsigned> unaryOperatorsPrecedence = {{
    {Token::Type::OperatorPlus, 2},
    {Token::Type::OperatorMinus, 2}
}};

static unsigned getPrecedence(Token::Type type, Parser::OperatorArity arity)
{
    switch(arity)
    {
        case Parser::OperatorArity::Unary: {
            if(!unaryOperatorsPrecedence.contains(type))
                throw std::invalid_argument("Token is not an unary operator");
            return unaryOperatorsPrecedence.at(type);
        }
        case Parser::OperatorArity::Binary: {
            if(!binaryOperatorsPrecedence.contains(type))
                throw std::invalid_argument("Token is not a binary operator");
            return binaryOperatorsPrecedence.at(type);
        }
        default:
            throw std::invalid_argument("Invalid arity for precedence calculation");
    }
}

static std::deque<std::pair<std::vector<Token>::const_iterator, std::optional<Parser::OperatorArity>>> convertToOnp(const std::vector<Token> &tokens, std::vector<Token>::const_iterator& it, Token::Type terminationToken)
{
    std::deque<std::pair<std::vector<Token>::const_iterator, std::optional<Parser::OperatorArity>>> onp;
    std::stack<std::pair<std::vector<Token>::const_iterator, Parser::OperatorArity>> operatorsStack;

    auto start = it;

    while(it != tokens.end() && it->type != terminationToken)
    {
        bool isTerminated = false;
        switch(it->type)
        {
            case Token::Type::Identificator:
            case Token::Type::Literal: {
                onp.push_back({it, {}});
            } break;
            case Token::Type::OperatorMinus:
            case Token::Type::OperatorPlus:
            case Token::Type::OperatorSlash:
            case Token::Type::OperatorStar: {
                auto arity = Parser::getOperatorArity(it, start);
                auto canPlaceOperator = [&operatorsStack, &it, &arity]() -> bool {
                    if(operatorsStack.empty()) return true;
                    
                    auto top = operatorsStack.top();

                    auto topPrecedence = getPrecedence(top.first->type, top.second);
                    auto currentPrecedence = getPrecedence(it->type, arity);

                    switch(arity)
                    {
                        case Parser::OperatorArity::Unary:
                            return topPrecedence <= currentPrecedence;
                        case Parser::OperatorArity::Binary:
                            return topPrecedence < currentPrecedence;
                        default:
                            throw std::invalid_argument("Invalid operator arity");
                    }
                };

                while(!canPlaceOperator())
                {
                    auto top = operatorsStack.top();
                    operatorsStack.pop();
                    onp.push_back(top);
                }
                operatorsStack.push({it, arity});
            } break;
            case Token::Type::ParenthesisLeft: {
                ++it;
                auto subExpression = convertToOnp(tokens, it, Token::Type::ParenthesisRight);
                onp.insert(onp.end(), subExpression.begin(), subExpression.end());
            } break;
            default: {
                if(it->type == terminationToken)
                {
                    isTerminated = true;
                    break;
                }
                
                throw std::runtime_error("Unexpected token");
            } break;
        }

        if(isTerminated) break;
        ++it;
    }

    while(!operatorsStack.empty())
    {
        auto top = operatorsStack.top();
        operatorsStack.pop();
        onp.push_back(top);
    }

    return onp;
}

inline static const std::unordered_map<Token::Type, BinaryOperation::OperationType> binaryOperationTypes = {{
    {Token::Type::OperatorPlus, BinaryOperation::OperationType::Addition},
    {Token::Type::OperatorMinus, BinaryOperation::OperationType::Subtraction},
    {Token::Type::OperatorStar, BinaryOperation::OperationType::Multiplication},
    {Token::Type::OperatorSlash, BinaryOperation::OperationType::Division}
}};

inline static const std::unordered_map<Token::Type, UnaryOperation::OperationType> unaryOperationTypes = {{
    {Token::Type::OperatorPlus, UnaryOperation::OperationType::Identity},
    {Token::Type::OperatorMinus, UnaryOperation::OperationType::Negation}
}};

std::unique_ptr<Expression> Parser::parseExpression(const std::vector<Token> &tokens, std::vector<Token>::const_iterator& it, Token::Type terminationToken)
{
    std::stack<std::unique_ptr<Expression>> values;

    auto onpDeque = convertToOnp(tokens, it, terminationToken);

    std::cout << "[Log] Tokens in ONP" << std::endl;
    for(const auto& pair : onpDeque)
    {
        std::cout << "[Log] Token " << *(pair.first) << std::endl;
    }

    for(auto pair : onpDeque)
    {
        auto type = pair.first->type;
        switch(type)
        {
            case Token::Type::Identificator: {
                values.push(std::make_unique<VariableValue>(pair.first->value));
            } break;
            case Token::Type::Literal: {
                values.push(std::make_unique<LiteralValue>(pair.first->value));
            } break;
            default: {
                switch(pair.second.value())
                {
                    case OperatorArity::Binary: {
                        auto rightOperand = std::move(values.top());
                        values.pop();
                        auto leftOperand = std::move(values.top());
                        values.pop();

                        if(!binaryOperationTypes.contains(pair.first->type)) throw std::runtime_error("Invalid binary operator");
                        values.push(std::make_unique<BinaryOperation>(binaryOperationTypes.at(pair.first->type), 
                            std::move(leftOperand), std::move(rightOperand)));
                    } break;
                    case OperatorArity::Unary: {
                        auto operand = std::move(values.top());
                        values.pop();

                        if(!unaryOperationTypes.contains(pair.first->type)) throw std::runtime_error("Invalid unary operator");
                        values.push(std::make_unique<UnaryOperation>(unaryOperationTypes.at(pair.first->type), std::move(operand)));
                    } break;
                }
            } break;
        }
    }

    auto result = std::move(values.top());
    values.pop();
    if(!values.empty()) 
        throw std::runtime_error("Invalid onp convertion");
    
    return std::move(result);
}

inline static const std::unordered_set<Token::Type> operators = {{
    Token::Type::OperatorMinus,
    Token::Type::OperatorPlus,
    Token::Type::OperatorSlash,
    Token::Type::OperatorStar
}};

Parser::OperatorArity Parser::getOperatorArity(std::vector<Token>::const_iterator &it, const std::vector<Token>::const_iterator &start)
{
    if(!operators.contains(it->type)) throw std::invalid_argument("Given token is not an operator and has no arity");

    if(it == start) return OperatorArity::Unary;
    if(operators.contains((it-1)->type)) return OperatorArity::Unary;

    return OperatorArity::Binary;
}