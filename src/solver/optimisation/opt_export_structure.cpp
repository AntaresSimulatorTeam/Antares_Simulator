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

void OPT_ExportInterco(const Antares::Solver::IResultWriter::Ptr writer,
                       PROBLEME_HEBDO* ProblemeHebdo)
{
    Yuni::Clob Flot;
    for (int i(0); i < ProblemeHebdo->NombreDInterconnexions; ++i)
    {
        Flot.appendFormat("%d %d %d\n",
                          i,
                          ProblemeHebdo->PaysOrigineDeLInterconnexion[i],
                          ProblemeHebdo->PaysExtremiteDeLInterconnexion[i]);
    }
    // TODO[FOM] "interco.txt"
    std::string filename = "interco-1-1.txt";
    writer->addEntryFromBuffer(filename, Flot);
}

void OPT_ExportAreaName(Antares::Solver::IResultWriter::Ptr writer,
                        const Antares::Data::AreaList& areas)
{
    // TODO[FOM] "area.txt"
    std::string filename = "area-1-1.txt";
    Yuni::Clob Flot;
    for (uint i = 0; i < areas.size(); ++i)
    {
        Flot.appendFormat("%s\n", areas[i]->name.c_str());
    }
    writer->addEntryFromBuffer(filename, Flot);
}

void OPT_Export_add_variable(std::vector<std::string>& varname,
                             int Var,
                             Antares::Data::Enum::ExportStructDict structDict,
                             int ts, // TODO remove
                             int firstVal,
                             std::optional<int> secondVal)
{
    if ((int)varname.size() > Var && varname[Var].empty())
    {
        std::stringstream buffer;
        buffer << Var << " ";
        buffer << Antares::Data::Enum::toString(structDict) << " ";
        buffer << firstVal << " ";
        if (secondVal.has_value())
        {
            buffer << secondVal.value() << " ";
        }
        buffer << ts << " ";
        varname[Var] = buffer.str();
    }
}

void OPT_ExportVariables(const Antares::Solver::IResultWriter::Ptr writer,
                         const std::vector<std::string>& varname,
                         const std::string& filename)
{
    Yuni::Clob Flot;
    for (auto const& line : varname)
    {
        Flot.appendFormat("%s\n", line.c_str());
    }
    writer->addEntryFromBuffer(filename, Flot);
}
