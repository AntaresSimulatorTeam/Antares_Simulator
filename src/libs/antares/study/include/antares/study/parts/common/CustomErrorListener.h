/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "antlr4-runtime.h"

namespace Antares::Data
{
template<typename ErrorType>
requires std::is_base_of_v<std::invalid_argument, ErrorType>
class CustomErrorListener final: public antlr4::BaseErrorListener
{
public:
    void syntaxError(antlr4::Recognizer* recognizer,
                     antlr4::Token* offendingSymbol,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string& msg,
                     std::exception_ptr e) override
    {
        std::ostringstream os;
        os << "Syntax error at line " << line << ":" << charPositionInLine << " - " << msg
           << std::endl;
        if (offendingSymbol)
        {
            os << "Offending symbol: " << offendingSymbol->getText() << std::endl;
        }
        throw ErrorType(os.str());
    }
};

} // namespace Antares::Data
