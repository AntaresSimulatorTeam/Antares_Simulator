// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <utility>

class IntoUTF8ArgsTranslator final
{
public:
    IntoUTF8ArgsTranslator(int argc, const char** argv);
    std::pair<int, const char**> convert();
    ~IntoUTF8ArgsTranslator();

private:
    int argc_;
    const char** argv_;
};
