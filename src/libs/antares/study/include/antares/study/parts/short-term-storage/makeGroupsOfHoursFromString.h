
#pragma once

#include <BaseErrorListener.h>
#include <set>
#include <string>
#include <vector>

namespace antlr4
{
class Recognizer;
}

namespace Antares::Data::ShortTermStorage
{
struct ShortTermStorageAdditionalConstraintsError final: std::invalid_argument
{
    using std::invalid_argument::invalid_argument;
};

class CustomErrorListener: public antlr4::BaseErrorListener
{
public:
    void syntaxError(antlr4::Recognizer* recognizer,
                     antlr4::Token* offendingSymbol,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string& msg,
                     std::exception_ptr e) override;
};

std::vector<std::set<int>> makeGroupsOfHours(std::string& hoursField);

}
