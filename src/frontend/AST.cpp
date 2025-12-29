#include "AST.hpp"
#include <stdexcept>
#include <charconv>

AST::VariableDeclaration::VariableDeclaration(std::string_view identificator, std::unique_ptr<Expression> value)
    : identificator(identificator), value(std::move(value)) 
{
    if(identificator.empty())
        throw std::invalid_argument("Cannot declare a variable with empty identificator");
}

AST::VariableAssignment::VariableAssignment(std::string_view identificator, std::unique_ptr<Expression> value)
    : identificator(identificator), value(std::move(value))
{
    if(identificator.empty())
        throw std::invalid_argument("Cannot assign to a variable with empty identificator");
}

AST::IfStatement::IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
    : condition(std::move(condition)), body(std::move(body))
{
}

AST::WhileStatement::WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
    : condition(std::move(condition)), body(std::move(body))
{
}

AST::DisplayStatement::DisplayStatement(std::string_view identificator)
    : identificator(identificator)
{
    if(identificator.empty())
        throw std::invalid_argument("Cannot display a variable with empty identificator");
}

AST::LiteralValue::LiteralValue(std::string_view value)
{
    std::from_chars(value.data(), value.data() + value.size(), this->value);
}

std::optional<int> AST::LiteralValue::getValue() const
{
    return value;
}

AST::VariableValue::VariableValue(std::string_view identificator)
    : identificator(identificator)
{}

std::optional<int> AST::VariableValue::getValue() const
{
    return std::optional<int>();
}

AST::BinaryOperation::BinaryOperation(OperationType operation, std::unique_ptr<Expression> leftOperand, std::unique_ptr<Expression> rightOperand)
    : operation(operation), leftOperand(std::move(leftOperand)), rightOperand(std::move(rightOperand))
{}

std::optional<int> AST::BinaryOperation::getValue() const
{
    auto leftValue = leftOperand->getValue();
    auto rightValue = rightOperand->getValue();

    if(!leftValue.has_value() || !rightValue.has_value()) return std::optional<int>();
    if(!operations.contains(operation)) throw std::runtime_error("Invalid binary operation type");

    auto operationLambda = operations.at(operation);
    return operationLambda(leftValue.value(), rightValue.value());
}

AST::UnaryOperation::UnaryOperation(OperationType operation, std::unique_ptr<Expression> operand)
    : operation(operation), operand(std::move(operand))
{}

std::optional<int> AST::UnaryOperation::getValue() const
{
    auto operandValue = operand->getValue();
    if(!operandValue.has_value()) return std::optional<int>();
    if(!operations.contains(operation)) throw std::runtime_error("Invalid unary operation type");

    auto operationLambda = operations.at(operation);
    return operationLambda(operandValue.value());
}
