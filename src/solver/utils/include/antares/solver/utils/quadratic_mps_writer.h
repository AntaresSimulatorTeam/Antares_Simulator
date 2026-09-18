// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <string>

#include <antares/writer/i_writer.h>

class PROBLEME_ANTARES_A_RESOUDRE;

void writeQuadraticMps(const PROBLEME_ANTARES_A_RESOUDRE& problem,
                       Antares::Solver::IResultWriter& writer,
                       const std::string& filename,
                       bool keepNames = true);
