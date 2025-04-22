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

std::vector<std::set<int>> makeGroupsOfHours(std::string& hoursField)
{
    CustomErrorListener customErrorListener;
    antlr4::ANTLRInputStream stream(hoursField);
    HoursFieldLexer lexer(&stream);
    lexer.removeErrorListeners();
    lexer.addErrorListener(&customErrorListener);
    antlr4::CommonTokenStream tokens(&lexer);
    HoursFieldParser parser(&tokens);

    parser.removeErrorListeners();
    parser.addErrorListener(&customErrorListener);

    auto* tree = parser.hoursField();
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
