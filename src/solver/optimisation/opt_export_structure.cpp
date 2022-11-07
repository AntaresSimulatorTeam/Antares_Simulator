/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <sstream>

#include <antares/study.h>

#include "../simulation/sim_structure_probleme_economique.h"

#include "opt_export_structure.h"

#include "../utils/mps_utils.h"
#include "../utils/filename.h"

////////////////////////////////////////////////////////////////////
// Export de la structure des LPs
////////////////////////////////////////////////////////////////////

namespace Antares
{
namespace Data
{
namespace Enum
{
template<>
const std::initializer_list<std::string>& getNames<ExportStructDict>()
{
    static std::initializer_list<std::string> s_exportStructDictNames{
      "ValeurDeNTCOrigineVersExtremite",
      "PalierThermique",
      "ProdHyd",
      "DefaillancePositive",
      "DefaillanceNegative",
      "BilansPays",
      "CoutOrigineVersExtremiteDeLInterconnexion",
      "CoutExtremiteVersOrigineDeLInterconnexion",
      "CorrespondanceVarNativesVarOptim"};
    return s_exportStructDictNames;
}
} // namespace Enum
} // namespace Data
} // namespace Antares

void OPT_ExportInterco(const Antares::Data::Study& study,
                       PROBLEME_HEBDO* ProblemeHebdo,
                       uint numSpace)
{
    // Interco are exported only once for first year
    if (ProblemeHebdo->firstWeekOfSimulation)
    {
        Yuni::Clob Flot;
        for (int i(0); i < ProblemeHebdo->NombreDInterconnexions; ++i)
        {
            Flot.appendFormat("%d %d %d\n",
                              i,
                              ProblemeHebdo->PaysOrigineDeLInterconnexion[i],
                              ProblemeHebdo->PaysExtremiteDeLInterconnexion[i]);
        }
        auto filename = getFilenameWithExtension("interco", "txt", numSpace);
        auto writer = study.resultWriter;
        writer->addEntryFromBuffer(filename, Flot);
    }
}

void OPT_ExportAreaName(const Antares::Data::Study& study,
                        PROBLEME_HEBDO* ProblemeHebdo,
                        uint numSpace)
{
    // Area name are exported only once for first year
    if (ProblemeHebdo->firstWeekOfSimulation)
    {
        auto filename = getFilenameWithExtension("area", "txt", numSpace);
        Yuni::Clob Flot;
        for (uint i = 0; i < study.areas.size(); ++i)
        {
            Flot.appendFormat("%s\n", study.areas[i]->name.c_str());
        }
        auto writer = study.resultWriter;
        writer->addEntryFromBuffer(filename, Flot);
    }
}

void OPT_Export_add_variable(std::vector<std::string>& varname,
                             int Var,
                             Antares::Data::Enum::ExportStructDict structDict,
                             int firstVal,
                             int secondVal,
                             int ts)
{
    if ((int)varname.size() > Var && varname[Var].empty())
    {
        std::stringstream buffer;
        buffer << Var << " ";
        buffer << Antares::Data::Enum::toString(structDict) << " ";
        buffer << firstVal << " ";
        buffer << secondVal << " ";
        buffer << ts << " ";
        varname[Var] = buffer.str();
    }
}

void OPT_Export_add_variable(std::vector<std::string>& varname,
                             int Var,
                             Antares::Data::Enum::ExportStructDict structDict,
                             int firstVal,
                             int ts)
{
    if ((int)varname.size() > Var && varname[Var].empty())
    {
        std::stringstream buffer;
        buffer << Var << " ";
        buffer << Antares::Data::Enum::toString(structDict) << " ";
        buffer << firstVal << " ";
        buffer << ts << " ";
        varname[Var] = buffer.str();
    }
}

void OPT_ExportVariables(const Antares::Data::Study& study,
                         const std::vector<std::string>& varname,
                         const std::string& fileName,
                         const std::string& fileExtension,
                         uint numSpace)
{
    Yuni::Clob Flot;
    auto filename = getFilenameWithExtension(fileName, fileExtension, numSpace);
    for (auto const& line : varname)
    {
        Flot.appendFormat("%s\n", line.c_str());
    }
    auto writer = study.resultWriter;
    writer->addEntryFromBuffer(filename, Flot);
}
