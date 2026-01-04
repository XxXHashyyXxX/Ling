#include "Tokens.hpp"
#include <stdexcept>
#include <cctype>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <sstream>

using namespace Tokenization;

std::unordered_map<std::string, Token::Type> types = {{
    {"=", Token::Type::OperatorAssign},
    {"+", Token::Type::OperatorPlus},
    {"-", Token::Type::OperatorMinus},
    {"*", Token::Type::OperatorStar},
    {"/", Token::Type::OperatorSlash},
    {"%", Token::Type::OperatorPercent},
    {"(", Token::Type::ParenthesisLeft},
    {")", Token::Type::ParenthesisRight},
    {"if", Token::Type::KeywordIf},
    {"let", Token::Type::KeywordLet},
    {"while", Token::Type::KeywordWhile},
    {"display", Token::Type::KeywordDisplay},
    {";", Token::Type::EndOfLine},
    {"{", Token::Type::BraceLeft},
    {"}", Token::Type::BraceRight},
    {"and", Token::Type::OperatorAND},
    {"or", Token::Type::OperatorOR},
    {"not", Token::Type::OperatorNOT},
    {"==", Token::Type::ComparatorEquals},
    {"!=", Token::Type::ComparatorNotEquals},
    {">", Token::Type::ComparatorGreaterThan},
    {">=", Token::Type::ComparatorGreaterEqual},
    {"<", Token::Type::ComparatorLessThan},
    {"<=", Token::Type::ComparatorLessEqual}
}};

static bool matchKeyword(const char* it, const char* end, const std::string& key) {
    for(auto c : key) {
        if(it == end || *it != c) return false;
        ++it;
    }
    return true;
}

static std::optional<Token::Type> matchLongestTokenType(const char* &it, const char* end) {
    struct Match {
        Token::Type type;
        size_t length;

        Match(Token::Type type, size_t length) : type(type), length(length) {}
    };

    std::optional<Match> best;

    for(const auto& [key, type] : types) {
        if(matchKeyword(it, end, key)) {
            if(!best.has_value() || key.size() > best->length) {
                best = Match(type, key.size());
            }
        }
    }

    if(!best.has_value()) return std::optional<Token::Type>();
    
    it += best->length;
    return best->type;
}

std::vector<Token> Tokenization::tokenize(std::string_view source)
{
    std::vector<Token> out;
    auto sourceEnd = source.end();

    unsigned lineCount = 1;
    unsigned charsCountAtLineStart = 0;

    for(auto it = source.begin(); it != sourceEnd; ++it) {
        if(*it == '\n') {
            lineCount++;
            charsCountAtLineStart = (it - source.begin());
            continue;
        }

        unsigned position = (it - source.begin() - charsCountAtLineStart);
        std::ostringstream errorLineOss;

        auto previousNewLine = it;
        auto begin = source.begin();
        while(previousNewLine != begin)
        {
            --previousNewLine;
            if(*previousNewLine == '\n')
                break;
        }
        auto lineBegin = *previousNewLine == '\n' ? previousNewLine+1 : previousNewLine;
        auto lineEnd = it;
        auto end = source.end();
        while(lineEnd != end)
        {
            if(*lineEnd == '\n')
                break;
            ++lineEnd;
        }
        if(lineBegin < it) errorLineOss << std::string(lineBegin, it);
        errorLineOss << "\033[31m" << *it << "\033[0m";
        if(it + 1 < lineEnd) errorLineOss << std::string(it + 1, lineEnd);
        errorLineOss << "\n";
        errorLineOss << std::string(it - lineBegin, ' ') << "\033[31m~\033[0m";

        if(std::isspace(*it)) continue;
        if(*it == '\0') break;

        if(std::isdigit(*it)) {
            auto start = it;
            while(it != sourceEnd && std::isdigit(*it)) ++it;
            auto end = it--;

            out.emplace_back(Token::Type::Literal, lineCount, position, errorLineOss.str(), std::string(start, end));
            continue;
        }

        auto tokenType = matchLongestTokenType(it, source.end());
        if(tokenType) {
            --it;
            out.emplace_back(*tokenType, lineCount, position, errorLineOss.str());
            continue;
        }

        if(std::isalpha(*it)) {
            auto start = it;
            while(it != sourceEnd && (std::isalnum(*it) || *it == '_')) ++it;
            auto end = it--;

            out.emplace_back(Token::Type::Identificator, lineCount, position, errorLineOss.str(), std::string(start, end));
            continue;
        }

        std::ostringstream oss;
        oss << "Could not tokenize character " << *it << " at line " << lineCount << ", position " << position << ":\n" << errorLineOss.str();

        throw std::runtime_error(oss.str());
    }
    return out;
}

std::ostream &Tokenization::operator<<(std::ostream &os, const Token &token)
{
    os << token.type;
    if(token.type == Token::Type::Identificator || token.type == Token::Type::Literal) os << token.value << '\'';
    return os << " in line " << token.line << ", position " << token.position << "\n" << token.errorLine; 
}

std::ostream &Tokenization::operator<<(std::ostream &os, const Token::Type &type)
{
    os << '\'';

    switch(type)
    {
        case Token::Type::EndOfLine:
            os << ';';
            break;
        case Token::Type::Identificator:
            os << "Identificator: ";
            break;
        case Token::Type::KeywordIf:
            os << "if";
            break;
        case Token::Type::KeywordLet:
            os << "let";
            break;
        case Token::Type::KeywordWhile:
            os << "while";
            break;
        case Token::Type::KeywordDisplay:
            os << "display";
            break;
        case Token::Type::Literal:
            os << "literal: ";
            break;
        case Token::Type::OperatorAssign:
            os << '=';
            break;
        case Token::Type::OperatorMinus:
            os << '-';
            break;
        case Token::Type::OperatorPlus:
            os << '+';
            break;
        case Token::Type::OperatorSlash:
            os << '/';
            break;
        case Token::Type::OperatorStar:
            os << '*';
            break;
        case Token::Type::OperatorPercent:
            os << "%";
            break;
        case Token::Type::ParenthesisLeft:
            os << '(';
            break;
        case Token::Type::ParenthesisRight:
            os << ')';
            break;
        case Token::Type::BraceLeft:
            os << "{";
            break;
        case Token::Type::BraceRight:
            os << "}";
            break;
        case Token::Type::OperatorAND:
            os << "and";
            break;
        case Token::Type::OperatorOR:
            os << "or";
            break;
        case Token::Type::OperatorNOT:
            os << "not";
            break;
        case Token::Type::ComparatorEquals:
            os << "==";
            break;
        case Token::Type::ComparatorNotEquals:
            os << "!=";
            break;
        case Token::Type::ComparatorGreaterEqual:
            os << ">=";
            break;
        case Token::Type::ComparatorGreaterThan:
            os << ">";
            break;
        case Token::Type::ComparatorLessEqual:
            os << "<=";
            break;
        case Token::Type::ComparatorLessThan:
            os << "<";
            break;
        default:
            throw std::runtime_error("Invalid type");
    }

    if(type != Token::Type::Identificator && type != Token::Type::Literal) return os << '\'';
    return os;
}

Tokenization::Token::Token(Type type, unsigned line, unsigned position, std::string errorLine, const std::string &value) 
    : type(type), value(value), line(line), position(position), errorLine(errorLine)
{
    if(value.empty() && (type == Token::Type::Identificator || type == Token::Type::Literal))
    {
        std::cerr << "Token type: " << type << "\nValue: " << value << "\n";
        throw std::invalid_argument("Cannot create empty token of provided type");
    }
}