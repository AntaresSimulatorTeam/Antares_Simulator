/*
** Copyright 2007-2023 RTE
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
#ifndef __EXPORT_STRUCTURE__
#define __EXPORT_STRUCTURE__

#include <vector>
#include <optional>
#include <string>

#include <yuni/yuni.h>

#include <antares/Enum.hpp>
#include "../writer/i_writer.h"
#include <antares/study.h>

namespace Antares
{
namespace Data
{
namespace Enum
{
/*! Enum class to define export structure dictionnary */
enum class ExportStructDict : unsigned char
{
    ValeurDeNTCOrigineVersExtremite,
    PalierThermique,
    ProdHyd,
    DefaillancePositive,
    DefaillanceNegative,
    BilansPays,
    CoutOrigineVersExtremiteDeLInterconnexion,
    CoutExtremiteVersOrigineDeLInterconnexion,
    CorrespondanceVarNativesVarOptim,
    DispatchableProduction
};
enum class ExportStructConstraintsDict : unsigned char
{
    FlowDissociation,
    AreaBalance,
    FictiveLoads,
    HydroPower,
    HydroPowerSmoothingUsingVariationSum,
    HydroPowerSmoothingUsingVariationMaxDown,
    HydroPowerSmoothingUsingVariationMaxUp,
    MinHydroPower,
    MaxHydroPower,
    MaxPumping,
    AreaHydroLevel,
    FinalStockEquivalent,
    FinalStockExpression,
    MinRunTime,
    MinStopTime,
    PMaxOfRunningThermalClusterGroups,
    PMinOfRunningThermalClusterGroups,
    StartingAndStopingThermalClusterGroups,
    ShortTermStorageLevel
};
enum class ExportStructTimeStepDict : unsigned char
{
    hour,
    day,
    week
};
enum class ExportStructLocationDict : unsigned char
{
    area,
    link
};
enum class ExportStructBindingConstraintType : unsigned char
{
    hourly,
    daily,
    weekly
};
} // namespace Enum

class Study;
} // namespace Data
} // namespace Antares

struct PROBLEME_HEBDO;

void OPT_Export_add_variable(std::vector<std::string>& varname,
                             int var,
                             Antares::Data::Enum::ExportStructDict structDict,
                             int ts, // TODO remove
                             int firstVal,
                             std::optional<int> secondVal = std::nullopt);
void OPT_ExportInterco(const Antares::Solver::IResultWriter::Ptr writer,
                       PROBLEME_HEBDO* problemeHebdo);
void OPT_ExportAreaName(Antares::Solver::IResultWriter::Ptr writer,
                        const Antares::Data::AreaList& areas);
void OPT_ExportVariables(const Antares::Solver::IResultWriter::Ptr writer,
                         const std::vector<std::string>& varname,
                         const std::string& filename);

#endif
