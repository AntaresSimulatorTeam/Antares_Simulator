// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include "spx_fonctions.h"

struct LibererProbleme
{
    void operator()(PROBLEME_SPX* p) const;
};

using PROBLEME_SPX_WRAPPER = std::unique_ptr<PROBLEME_SPX, LibererProbleme>;
