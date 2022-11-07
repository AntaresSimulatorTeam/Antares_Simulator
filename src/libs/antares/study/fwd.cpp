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

#include "fwd.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
const char* SeedToCString(SeedIndex seed)
{
    switch (seed)
    {
    case seedTsGenLoad:
        return "Load time-series generation";
    case seedTsGenHydro:
        return "Hydro time-series generation";
    case seedTsGenWind:
        return "Wind time-series generation";
    case seedTsGenThermal:
        return "Thermal time-series generation";
    case seedTsGenSolar:
        return "Solar time-series generation";
    case seedTimeseriesNumbers:
        return "Time-series draws (MC scenario builder)";
    case seedUnsuppliedEnergyCosts:
        return "Noise on Unsupplied energy costs";
    case seedSpilledEnergyCosts:
        return "Noise on Spilled energy costs";
    case seedThermalCosts:
        return "Noise on Thermal plants costs";
    case seedHydroCosts:
        return "Noise on virtual Hydro costs";
    case seedHydroManagement:
        return "Initial reservoir levels";
    case seedMax:
        return "";
    }
    return "";
}

const char* SeedToID(SeedIndex seed)
{
    switch (seed)
    {
    case seedTsGenLoad:
        return "seed-tsgen-load";
    case seedTsGenHydro:
        return "seed-tsgen-hydro";
    case seedTsGenWind:
        return "seed-tsgen-wind";
    case seedTsGenThermal:
        return "seed-tsgen-thermal";
    case seedTsGenSolar:
        return "seed-tsgen-solar";
    case seedTimeseriesNumbers:
        return "seed-tsnumbers";
    case seedUnsuppliedEnergyCosts:
        return "seed-unsupplied-energy-costs";
    case seedSpilledEnergyCosts:
        return "seed-spilled-energy-costs";
    case seedThermalCosts:
        return "seed-thermal-costs";
    case seedHydroCosts:
        return "seed-hydro-costs";
    case seedHydroManagement:
        return "seed-initial-reservoir-levels";
    case seedMax:
        return "";
    }
    return "";
}

// ... Initial reservoir levels ...
InitialReservoirLevels StringToInitialReservoirLevels(const AnyString& text)
{
    if (!text)
        return irlUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "cold start")
        return irlColdStart;
    if (s == "hot start")
        return irlHotStart;

    return irlUnknown;
}

const char* InitialReservoirLevelsToCString(InitialReservoirLevels iniLevels)
{
    switch (iniLevels)
    {
    case irlColdStart:
        return "cold start";
    case irlHotStart:
        return "hot start";
    case irlUnknown:
        return "";
    }
    return "";
}

// ... Hydro heuristic policy ...
HydroHeuristicPolicy StringToHydroHeuristicPolicy(const AnyString& text)
{
    if (!text)
        return hhpUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "accommodate rule curves")
        return hhpAccommodateRuleCurves;
    if (s == "maximize generation")
        return hhpMaximizeGeneration;

    return hhpUnknown;
}

const char* HydroHeuristicPolicyToCString(HydroHeuristicPolicy hhPolicy)
{
    switch (hhPolicy)
    {
    case hhpAccommodateRuleCurves:
        return "accommodate rule curves";
    case hhpMaximizeGeneration:
        return "maximize generation";
    case hhpUnknown:
        return "";
    }
    return "";
}

// ... Hydro pricing ...
HydroPricingMode StringToHydroPricingMode(const AnyString& text)
{
    if (!text)
        return hpUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "fast")
        return hpHeuristic;
    if (s == "accurate") // mixed integer linear problem
        return hpMILP;

    return hpUnknown;
}

const char* HydroPricingModeToCString(HydroPricingMode hpm)
{
    switch (hpm)
    {
    case hpHeuristic:
        return "fast";
    case hpMILP:
        return "accurate"; // (slow)
    case hpUnknown:
        return "";
    }
    return "";
}

PowerFluctuations StringToPowerFluctuations(const AnyString& text)
{
    if (!text)
        return lssUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "minimize ramping")
        return lssMinimizeRamping;
    if (s == "free modulations")
        return lssFreeModulations;
    if (s == "minimize excursions")
        return lssMinimizeExcursions;

    return lssUnknown;
}

const char* PowerFluctuationsToCString(PowerFluctuations fluctuations)
{
    switch (fluctuations)
    {
    case lssFreeModulations:
        return "free modulations";
    case lssMinimizeRamping:
        return "minimize ramping";
    case lssMinimizeExcursions:
        return "minimize excursions";
    case lssUnknown:
        return "";
    }
    return "";
}

SheddingPolicy StringToSheddingPolicy(const AnyString& text)
{
    if (!text)
        return shpUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "shave peaks")
        return shpShavePeaks;
    if (s == "minimize duration")
        return shpMinimizeDuration;

    return shpUnknown;
}

const char* SheddingPolicyToCString(SheddingPolicy strategy)
{
    switch (strategy)
    {
    case shpShavePeaks:
        return "shave peaks";
    case shpMinimizeDuration:
        return "minimize duration";
    case shpUnknown:
        return "";
    }
    return "";
}

UnitCommitmentMode StringToUnitCommitmentMode(const AnyString& text)
{
    if (!text)
        return ucUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "fast")
        return ucHeuristic;
    if (s == "accurate") // mixed integer linear problem
        return ucMILP;

    return ucUnknown;
}

const char* UnitCommitmentModeToCString(UnitCommitmentMode ucommitment)
{
    switch (ucommitment)
    {
    case ucHeuristic:
        return "fast";
    case ucMILP:
        return "accurate"; // (slow)
    case ucUnknown:
        return "";
    }
    return "";
}

NumberOfCoresMode StringToNumberOfCoresMode(const AnyString& text)
{
    if (!text)
        return ncUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "minimum")
        return ncMin;
    if (s == "low")
        return ncLow;
    if (s == "medium")
        return ncAvg;
    if (s == "high")
        return ncHigh;
    if (s == "maximum")
        return ncMax;

    return ncUnknown;
}

const char* NumberOfCoresModeToCString(NumberOfCoresMode ucores)
{
    switch (ucores)
    {
    case ncMin:
        return "minimum";
    case ncLow:
        return "low";
    case ncAvg:
        return "medium";
    case ncHigh:
        return "high";
    case ncMax:
        return "maximum";
    case ncUnknown:
        return "";
    }
    return "";
}

const char* RenewableGenerationModellingToCString(RenewableGenerationModelling rgModelling)
{
    switch (rgModelling)
    {
    case rgAggregated:
        return "aggregated";
    case rgClusters:
        return "clusters"; // using renewable clusters
    case rgUnknown:
        return "";
    }
    return "";
}

DayAheadReserveManagement StringToDayAheadReserveManagementMode(const AnyString& text)
{
    if (!text)
        return daReserveUnknown;

    CString<24, false> s = text;
    s.trim();
    s.toLower();
    if (s == "global")
        return daGlobal;
    if (s == "local")
        return daLocal;

    return daReserveUnknown;
}

const char* DayAheadReserveManagementModeToCString(DayAheadReserveManagement daReserveMode)
{
    switch (daReserveMode)
    {
    case daGlobal:
        return "global";
    case daLocal:
        return "local";
    case daReserveUnknown:
        return "";
    }
    return "";
}


std::string mpsExportStatusToString(const mpsExportStatus& mps_export_status)
{
    switch (mps_export_status)
    {
    case mpsExportStatus::NO_EXPORT:
        return "none";
    case mpsExportStatus::EXPORT_FIRST_OPIM:
        return "optim-1";
    case mpsExportStatus::EXPORT_SECOND_OPIM:
        return "optim-2";
    case mpsExportStatus::EXPORT_BOTH_OPTIMS:
        return "both-optims";
    default:
        return "unknown status";
    }
}

mpsExportStatus stringToMPSexportStatus(const AnyString& value)
{
    if (!value)
    {
        return mpsExportStatus::UNKNOWN_EXPORT;
    }

    CString<24, false> v = value;
    v.trim();
    v.toLower();
    if (v == "both-optims" || v == "true")   // Case "true" : for compatibily with older study versions
        return mpsExportStatus::EXPORT_BOTH_OPTIMS;
    if (v == "none" || v == "false")   // Case "false" : for compatibily with older study versions
        return mpsExportStatus::NO_EXPORT;
    if (v == "optim-1")
        return mpsExportStatus::EXPORT_FIRST_OPIM;
    if (v == "optim-2")
        return mpsExportStatus::EXPORT_SECOND_OPIM;

    return mpsExportStatus::UNKNOWN_EXPORT;
}

std::string GlobalTransmissionCapacitiesToString(GlobalTransmissionCapacities transmissionCapacities)
{
        switch (transmissionCapacities)
        {
        using GT = GlobalTransmissionCapacities;
        case GT::enabledForAllLinks: return "local-values";
        case GT::nullForAllLinks: return "null-for-all-links";
        case GT::infiniteForAllLinks: return "infinite-for-all-links";
        case GT::infiniteForPhysicalLinks: return "infinite-for-physical-links";
        case GT::nullForPhysicalLinks: return "null-for-physical-links";
        }
        return "";
}

bool stringToGlobalTransmissionCapacities(const AnyString& value,
                                          GlobalTransmissionCapacities& out)
{
        using GT = GlobalTransmissionCapacities;
        CString<64, false> v = value;
        v.trim();
        v.toLower();
        if (v == "local-values")
            out = GT::enabledForAllLinks;
        else if (v == "null-for-all-links")
            out = GT::nullForAllLinks;
        else if (v == "infinite-for-all-links")
            out = GT::infiniteForAllLinks;
        else if (v == "infinite-for-physical-links")
            out = GT::infiniteForPhysicalLinks;
        else if (v == "null-for-physical-links")
            out = GT::nullForPhysicalLinks;
        else
            out = v.to<bool>() ? GT::enabledForAllLinks : GT::nullForAllLinks;
        return true;
}

} // namespace Data
} // namespace Antares
