/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include "singleProblemGetterImpl.h"

#include <map>
#include <stdexcept>
#include <string>

#include "antares/application/ScenarioBuilderOwner.h"
#include "antares/benchmarking/DurationCollector.h"
#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/solver/hydro/management/HydroInputsChecker.h"
#include "antares/solver/optimisation/LinearProblemMatrix.h"
#include "antares/solver/optimisation/opt_export_structure.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/regenerate_timeseries.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/writer/i_writer.h"

#include "fmt/format.h"

namespace
{
constexpr int optimizationNumber = 1;  // the 1st optim is available for now
constexpr int numeroDeLIntervalle = 0; // simplex-range = week
constexpr int numSpace = 0;            // full sequential
constexpr int PremierPdtDeLIntervalle = 0;
constexpr int DernierPdtDeLIntervalle = 168; // 1 week = 7*24 hours
Antares::Solver::NullResultWriter gResultWriter;
Benchmarking::DurationCollector gDurationCollector;

std::unique_ptr<Antares::Data::Study> loadStudy(const std::filesystem::path& studyPath)
{
    Antares::FileTreeStudyLoader loader(studyPath);
    auto study = loader.load();
    return study;
}

std::string problemName(const WeeklyProblemId& id)
{
    return fmt::format("problem-{}-{}.txt", id.year, id.week);
}
} // namespace

namespace Antares::Solver::Implementation
{

SingleProblemGetter::SingleProblemGetter(const std::filesystem::path& studyPath):
    SingleProblemGetter(loadStudy(studyPath))
{
}

SingleProblemGetter::SingleProblemGetter(std::unique_ptr<Antares::Data::Study>&& study):
    study_(std::move(study))
{
    SIM_InitialisationProblemeHebdo(*study_,
                                    pb_,
                                    /* NombreDePasDeTemps = */ 168,
                                    numSpace);

    Antares::Solver::Simulation::regenerateTimeSeries(*study_, gResultWriter, gDurationCollector);

    study_->computePThetaInfForThermalClusters(); // PthetaInf

    // TODO duplication
    if (!pb_.LeProblemeADejaEteInstancie)
    {
        pb_.NombreDeJours = (int)(pb_.NombreDePasDeTemps / pb_.NombreDePasDeTempsDUneJournee);

        if (!pb_.OptimisationAuPasHebdomadaire)
        {
            pb_.NombreDePasDeTempsPourUneOptimisation = pb_.NombreDePasDeTempsDUneJournee;
        }
        else
        {
            pb_.NombreDePasDeTempsPourUneOptimisation = pb_.NombreDePasDeTemps;
        }

        OPT_AllocDuProblemeAOptimiser(&pb_);

        OPT_ChainagesDesIntercoPartantDUnNoeud(&pb_);

        pb_.LeProblemeADejaEteInstancie = true;
    }

    scratchmap_ = study_->areas.buildScratchMap(numSpace);
    initializeRandomNumbers();
    ScenarioBuilderOwner(*study_).callScenarioBuilder();
}

std::vector<WeeklyProblemId> SingleProblemGetter::getProblemIds() const
{
    std::vector<WeeklyProblemId> ret;

    const auto& p = study_->parameters;
    for (unsigned int year = 0; year < p.nbYears; ++year)
    {
        if (p.yearsFilter[year])
        {
            for (unsigned week = 0; week < p.simulationDays.numberOfWeeks(); ++week)
            {
                // by convention, weeks start at 1
                ret.emplace_back(year, week + 1);
            }
        }
    }
    return ret;
}

void SingleProblemGetter::initializeRandomNumbers()
{
    int nbYears = 0;
    std::map<unsigned int, bool> isYearPerformed; // TODO check year number
    for (uint year = 0; year < study_->parameters.nbYears; ++year)
    {
        isYearPerformed[year] = study_->parameters.yearsFilter[year];
        if (study_->parameters.yearsFilter[year])
        {
            ++nbYears;
        }
    }

    randomForParallelYears_.emplace(nbYears, study_->parameters.power.fluctuations);
    randomForParallelYears_->allocate(*study_);

    MersenneTwister randomHydroGenerator;
    randomHydroGenerator.reset(study_->parameters.seed[Data::seedHydroManagement]);
    randomForParallelYears_->compute(*study_, 1, isYearPerformed, randomHydroGenerator);
}

void SingleProblemGetter::writeNTCTimeSeries(const std::filesystem::path& outputDir)
{
    // TS number have already been loaded/generated, we just need to write them
    auto writer = resultWriterFactory(Antares::Data::ResultFormat::legacyFilesDirectories,
                                      outputDir,
                                      nullptr, // not needed
                                      gDurationCollector);
    study_->storeTimeSeriesNumbers<Antares::Data::TimeSeriesType::timeSeriesTransmissionCapacities>(
      *writer);
}

void SingleProblemGetter::writeStudyDescriptionFiles(const std::filesystem::path& outputDir)
{
    auto writer = resultWriterFactory(Antares::Data::ResultFormat::legacyFilesDirectories,
                                      outputDir,
                                      nullptr, // not needed
                                      gDurationCollector);
    OPT_ExportStructures(&pb_, *writer);
}

ConstantDataFromAntares SingleProblemGetter::getConstantData()
{
    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(&pb_);

    auto builder_data = NewGetConstraintBuilderFromProblemHebdo(&pb_);
    ConstraintBuilder builder(builder_data);
    LinearProblemMatrix linearProblemMatrix(&pb_, builder);
    linearProblemMatrix.Run();

    return translator_.commonProblemData(pb_.ProblemeAResoudre.get());
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    auto [year, week] = id;
    // by convention, weeks start at 1 from the caller's POV, but at 0 in Simulator
    if (week == 0)
    {
        throw std::out_of_range("Invalid week number 0 detected, week number must be >=1");
    }
    week--;

    pb_.year = year;
    pb_.weekInTheYear = week;

    const auto [hydroLevels, ventilationResults] = getYearlyData(year);

    // TODO
    uint indexYear = randomForParallelYears_->yearNumberToIndex[year];
    auto& randomForCurrentYear = randomForParallelYears_->pYears[indexYear];

    // TODO once per year, not every week
    Antares::Solver::Simulation::PrepareRandomNumbers(*study_, pb_, randomForCurrentYear);

    const auto hourInTheYear = 168 * week; // TODO
    SIM_RenseignementProblemeHebdo(*study_,
                                   pb_,
                                   week,
                                   hourInTheYear,
                                   ventilationResults,
                                   scratchmap_);

    // Apply hydro levels
    for (uint areaIndex = 0; areaIndex < study_->areas.size(); ++areaIndex)
    {
        const auto* area = study_->areas.byIndex[areaIndex];
        if (area->hydro.reservoirManagement)
        {
            double initialLevel = hydroLevels.at(area)[week];
            pb_.CaracteristiquesHydrauliques[areaIndex].NiveauInitialReservoir = initialLevel;
        }
    }

    // required at least for OPT_SommeDesPminThermiques (RHS)
    Antares::Solver::Simulation::BuildThermalPartOfWeeklyProblem(
      *study_,
      pb_,
      hourInTheYear,
      randomForCurrentYear.pThermalNoisesByArea,
      year);

    OPT_VerifierPresenceReserveJmoins1(&pb_);

    OPT_InitialiserLesPminHebdo(&pb_);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(&pb_);

    OPT_MaxDesPmaxHydrauliques(&pb_);

    if (pb_.OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(&pb_);
    }

    OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(&pb_,
                                                           PremierPdtDeLIntervalle,
                                                           DernierPdtDeLIntervalle,
                                                           optimizationNumber);

    OPT_InitialiserLeSecondMembreDuProblemeLineaire(&pb_,
                                                    PremierPdtDeLIntervalle,
                                                    DernierPdtDeLIntervalle,
                                                    numeroDeLIntervalle,
                                                    optimizationNumber);

    OPT_InitialiserLesCoutsLineaire(&pb_, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
    return translator_.translate(pb_.ProblemeAResoudre.get(), problemName(id));
}

const YearlyData& SingleProblemGetter::getYearlyData(unsigned year)
{
    if (auto it = allData_.find(year); it != allData_.end()) // We already have data for this year
    {
        return it->second;
    }

    auto& dataForYear = allData_[year];

    /*
      Side effects for HydroInputsChecker are limited to the year scope
      inside the study.
      more specifically, area.hydro.managementData[year]
      So "out-of-order" such as calls "y=0, y=4, y=0" should be fine
    */
    Antares::HydroInputsChecker hydroInputsChecker(*study_);
    hydroInputsChecker.Execute(year);
    hydroInputsChecker.CheckForErrors();

    Antares::Solver::Simulation::prepareClustersInMustRunMode(*study_,
                                                              scratchmap_,
                                                              year,
                                                              study_->parameters.mode);

    uint indexYear = randomForParallelYears_->yearNumberToIndex[year];
    auto& randomForCurrentYear = randomForParallelYears_->pYears[indexYear];

    dataForYear = computeHydroLevels(year, randomForCurrentYear.pReservoirLevels);

    return dataForYear;
}

YearlyData SingleProblemGetter::computeHydroLevels(unsigned year,
                                                   const std::vector<double>& initialLevel)
{
    // For each year
    Antares::HydroManagement hydroManagement(study_->areas,
                                             study_->parameters,
                                             study_->calendar,
                                             gResultWriter);

    HydroLevels hydroLevels;
    hydroManagement.makeVentilation(initialLevel, year, scratchmap_);

    const auto& ventilationResults = hydroManagement.ventilationResults();
    const auto& calendar = study_->calendar;

    int areaIndex = 0;
    for (const auto& [_, area]: study_->areas)
    {
        if (!area->hydro.reservoirManagement)
        {
            areaIndex++;
            continue;
        }
        auto inflows = area->hydro.series->storage.getColumn(year);
        auto& level = hydroLevels[area];

        // Initialize first week level
        uint firstDay = calendar.weeks[0].daysYear.first;
        level[0] = ventilationResults[areaIndex].NiveauxReservoirsDebutJours[firstDay]
                   * area->hydro.reservoirCapacity;

        // Compute sums for first week to use in week 1
        double inflowsPrevWeek = 0.0, turbinedPrevWeek = 0.0;
        for (uint day = calendar.weeks[0].daysYear.first; day < calendar.weeks[0].daysYear.end;
             ++day)
        {
            inflowsPrevWeek += inflows[day];
            turbinedPrevWeek += ventilationResults[areaIndex].HydrauliqueModulableQuotidien[day];
        }

        // Loop over remaining weeks
        for (unsigned week = 1; week < calendar.maxWeeksInYear; ++week)
        {
            // Update level using previous week values
            level[week] = level[week - 1] + inflowsPrevWeek - turbinedPrevWeek;

            // Compute sums for this week to use next iteration
            inflowsPrevWeek = 0.0;
            turbinedPrevWeek = 0.0;
            for (uint day = calendar.weeks[week].daysYear.first;
                 day < calendar.weeks[week].daysYear.end;
                 ++day)
            {
                inflowsPrevWeek += inflows[day];
                turbinedPrevWeek += ventilationResults[areaIndex]
                                      .HydrauliqueModulableQuotidien[day];
            }

            logs.debug() << "week=" << week << " prevInflows=" << inflowsPrevWeek
                         << " prevTurbined=" << turbinedPrevWeek << " level=" << level[week];
        }

        areaIndex++;
    }
    return {hydroLevels, ventilationResults};
}
} // namespace Antares::Solver::Implementation
