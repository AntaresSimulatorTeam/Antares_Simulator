#pragma once

#include "economy_base.h"

namespace Antares::Solver::Variable::Economy
{

struct TestColumnTraits: public LOLD_Base_Traits
{
    static std::string Caption()
    {
        return "TEST";
    }

    static std::string Description()
    {
        return "Test column - always returns 1";
    }

    static double value(const State&)
    {
        return 1.;
    }

    static bool checkCondition(const State&)
    {
        return true;
    }
};

using VCardTestColumn = VCard_Base<TestColumnTraits>;

template<class NextT = Container::EndOfList>
using TestColumn = Economy_Base<TestColumnTraits, NextT>;

} // namespace Antares::Solver::Variable::Economy
