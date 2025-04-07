#pragma once

#include <memory>

#include "spx_fonctions.h"

struct LibererProbleme
{
    void operator()(PROBLEME_SPX* p) const;
};

using PROBLEME_SPX_WRAPPER = std::unique_ptr<PROBLEME_SPX, LibererProbleme>;
