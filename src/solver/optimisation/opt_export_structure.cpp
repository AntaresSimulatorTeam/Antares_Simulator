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

#include "antares/solver/optimisation/opt_export_structure.h"

#include <sstream>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"

////////////////////////////////////////////////////////////////////
// Export de la structure des LPs
////////////////////////////////////////////////////////////////////

void OPT_ExportInterco(Antares::Solver::IResultWriter& writer, PROBLEME_HEBDO* problemeHebdo)
{
    Yuni::Clob Flot;
    for (uint32_t i(0); i < problemeHebdo->NombreDInterconnexions; ++i)
    {
        Flot.appendFormat("%d %d %d\n",
                          i,
                          problemeHebdo->PaysOrigineDeLInterconnexion[i],
                          problemeHebdo->PaysExtremiteDeLInterconnexion[i]);
    }
    // TODO[FOM] "interco.txt"
    std::string filename = "interco-1-1.txt";
    writer.addEntryFromBuffer(filename, Flot);
}

void OPT_ExportAreaName(Antares::Solver::IResultWriter& writer,
                        const std::vector<const char*>& areaNames)
{
    // TODO[FOM] "area.txt"
    std::string filename = "area-1-1.txt";
    Yuni::Clob Flot;
    for (const char* name: areaNames)
    {
        Flot.appendFormat("%s\n", name);
    }
    writer.addEntryFromBuffer(filename, Flot);
}

void OPT_ExportStructures(PROBLEME_HEBDO* problemeHebdo, Antares::Solver::IResultWriter& writer)
{
    OPT_ExportInterco(writer, problemeHebdo);
    OPT_ExportAreaName(writer, problemeHebdo->NomsDesPays);
}
