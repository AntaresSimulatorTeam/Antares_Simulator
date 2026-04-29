// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/opt_export_structure.h"

#include <string>
#include <vector>

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
    std::string content = Flot.c_str();
    writer.addEntryFromBuffer(filename, content);
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

    std::string content = Flot.c_str();
    writer.addEntryFromBuffer(filename, content);
}

void OPT_ExportStructures(PROBLEME_HEBDO* problemeHebdo, Antares::Solver::IResultWriter& writer)
{
    OPT_ExportInterco(writer, problemeHebdo);
    OPT_ExportAreaName(writer, problemeHebdo->NomsDesPays);
}
