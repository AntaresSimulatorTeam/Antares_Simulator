/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_VARIABLE_PRINT_H__
#define __SOLVER_VARIABLE_PRINT_H__

#include <yuni/yuni.h>

namespace Antares
{
namespace Solver
{
namespace Variable
{
class PrintInfosStdCout final
{
public:
    PrintInfosStdCout():
        pIndent(0)
    {
    }

    template<class VCardT>
    void beginNode()
    {
        printVCard<VCardT, true>();
        ++pIndent;
    }

    template<class VCardT>
    void addVCard()
    {
        printVCard<VCardT, false>();
    }

    void endNode()
    {
        --pIndent;
    }

private:
    template<class VCardT, bool IsNodeT>
    void printVCard()
    {
        pBuffer.clear();
        pBuffer.resize(1 + pBuffer.size() + pIndent * 4, " ");
        pBuffer += (IsNodeT ? "+ " : "  ");
        pBuffer += VCardT::Caption();
        pBuffer.resize(29, " ");
        pBuffer += VCardT::Unit();
        pBuffer.resize(37, " ");
        pBuffer += VCardT::Description();
        Antares::logs.info() << pBuffer;
    }

    void printIndent()
    {
        if (pIndent)
        {
            for (uint i = 0; i != pIndent; ++i)
            {
                pBuffer += "    ";
            }
        }
    }

private:
    uint pIndent;
    Yuni::String pBuffer;
};

} // namespace Variable
} // namespace Solver
} // namespace Antares

#endif // __SOLVER_VARIABLE_PRINT_H__
