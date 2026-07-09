// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/writer/i_writer.h"
struct PROBLEME_HEBDO;

void OPT_ExportStructures(PROBLEME_HEBDO* problemeHebdo, Antares::Solver::IResultWriter& writer);
