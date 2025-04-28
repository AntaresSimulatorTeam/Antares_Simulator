#include "antares/study/parts/short-term-storage/makeGroupsOfHoursFromString.h"

#include <regex>

#include <boost/algorithm/string.hpp>

#include "antares/study/parts/short-term-storage/HoursCollectorVisitor.h"

#include "HoursFieldLexer.h"

namespace Antares::Data::ShortTermStorage
{

void CustomErrorListener::syntaxError(antlr4::Recognizer* recognizer,
                                      antlr4::Token* offendingSymbol,
                                      size_t line,
                                      size_t charPositionInLine,
                                      const std::string& msg,
                                      std::exception_ptr e)
{
    std::ostringstream os;
    os << "Syntax error at line " << line << ":" << charPositionInLine << " - " << msg << std::endl;
    if (offendingSymbol)
    {
        os << "Offending symbol: " << offendingSymbol->getText() << std::endl;
    }
    throw ShortTermStorageAdditionalConstraintsError(os.str());
}

class GroupsHours
{
public:
    explicit GroupsHours(const std::string& hoursField):
        hoursField_(hoursField),
        stream_(hoursField_),
        lexer_(&stream_),
        tokens_(&lexer_),
        parser_(&tokens_)
    {
        lexer_.removeErrorListeners();
        lexer_.addErrorListener(&customErrorListener_);
        parser_.removeErrorListeners();
        parser_.addErrorListener(&customErrorListener_);
    }

    auto* hoursField()
    {
        return parser_.hoursField();
    }

private:
    std::string hoursField_;
    CustomErrorListener customErrorListener_;
    antlr4::ANTLRInputStream stream_;
    HoursFieldLexer lexer_;
    antlr4::CommonTokenStream tokens_;
    HoursFieldParser parser_;
};

std::vector<std::set<int>> makeGroupsOfHours(const std::string& hoursField)
{
    GroupsHours groupsHours(hoursField);
    auto* tree = groupsHours.hoursField();
    try
    {
        HoursCollectorVisitor visitor;
        return std::any_cast<std::vector<std::set<int>>>(visitor.visit(tree));
    }
    catch (std::exception& e)
    {
        std::ostringstream os;
        os << "Exception thrown while parsing '" << hoursField << "' : " << e.what() << std::endl;
        throw ShortTermStorageAdditionalConstraintsError(os.str());
    }
}
} // namespace Antares::Data::ShortTermStorage
