// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0
#include "antares/study/parts/reserves/makeGroupsOfSymmetriesFromString.h"

#include <boost/algorithm/string.hpp>

#include "antares/study/parts/common/CustomErrorListener.h"
#include "antares/study/parts/reserves/SymmetryCollectorVisitor.h"

#include "SymmetryFieldLexer.h"

namespace Antares::Data::Symmetries
{

class GroupsSymmetries
{
public:
    explicit GroupsSymmetries(const std::string& symmetriesField):
        symmetriesField_(symmetriesField),
        stream_(symmetriesField_),
        lexer_(&stream_),
        tokens_(&lexer_),
        parser_(&tokens_)
    {
        lexer_.removeErrorListeners();
        lexer_.addErrorListener(&customErrorListener_);
        parser_.removeErrorListeners();
        parser_.addErrorListener(&customErrorListener_);
    }

    auto* symmetriesField()
    {
        return parser_.symmetryField();
    }

private:
    std::string symmetriesField_;
    CustomErrorListener<SymmetriesError> customErrorListener_;
    antlr4::ANTLRInputStream stream_;
    SymmetryFieldLexer lexer_;
    antlr4::CommonTokenStream tokens_;
    SymmetryFieldParser parser_;
};

std::vector<std::set<std::string>> makeGroupsOfSymmetries(const std::string& symmetriesField)
{
    GroupsSymmetries groupsSymmetries(symmetriesField);
    auto* tree = groupsSymmetries.symmetriesField();
    try
    {
        SymmetryCollectorVisitor visitor;
        auto result = std::any_cast<std::vector<std::set<std::string>>>(visitor.visit(tree));

        return result;
    }
    catch (std::exception& e)
    {
        std::ostringstream os;
        os << "Exception thrown while parsing '" << symmetriesField << "' : " << e.what()
           << std::endl;
        throw SymmetriesError(os.str());
    }
}
} // namespace Antares::Data::Symmetries
