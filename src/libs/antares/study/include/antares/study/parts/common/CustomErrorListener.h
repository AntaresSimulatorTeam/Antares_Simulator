// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    void syntaxError([[maybe_unused]] antlr4::Recognizer* recognizer,
                     antlr4::Token* offendingSymbol,
                     size_t line,
                     size_t charPositionInLine,
                     const std::string& msg,
                     [[maybe_unused]] std::exception_ptr e) override
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
