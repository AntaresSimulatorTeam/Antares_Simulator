// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "singleProblemGetterImpl.h"

#include <map>
#include <stdexcept>
#include <string>

#include <antares/optimisation/linear-problem-api/StructuredLinearProblem.h>
#include <antares/writer/null_result_writer.h>
#include "antares/application/ScenarioBuilderOwner.h"
#include "antares/benchmarking/DurationCollector.h"
#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/io/outputs/MPSGenerator.h"
#include "antares/modeler-optimisation-container/OptimEntityContainer.h"
#include "antares/solver/hydro/management/HydroInputsChecker.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/solver/optimisation/LinearProblemMatrix.h"
#include "antares/solver/optimisation/opt_export_structure.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/regenerate_timeseries.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/writer/i_writer.h"

using namespace Optimisation::LinearProblemApi;

namespace
{
constexpr int optimizationNumber = 1;  // the 1st optim is available for now
constexpr int numeroDeLIntervalle = 0; // simplex-range = week
constexpr int numSpace = 0;            // full sequential
constexpr int PremierPdtDeLIntervalle = 0;
constexpr int DernierPdtDeLIntervalle = 168; // 1 week = 7*24 hours
Antares::Solver::NullResultWriter gResultWriter;
Benchmarking::DurationCollector gDurationCollector;

std::pair<std::unique_ptr<Data::Study>, Solver::IResultWriter::Ptr> loadStudy(
  const std::filesystem::path& studyPath)
{
    Antares::FileTreeStudyLoader loader(studyPath);
    return loader.load();
}

} // namespace

namespace Antares::Solver::Implementation
{

SingleProblemGetter::SingleProblemGetter(const std::filesystem::path& studyPath):
    SingleProblemGetter(loadStudy(studyPath))
{
}

SingleProblemGetter::SingleProblemGetter(
  std::pair<std::unique_ptr<Data::Study>, Solver::IResultWriter::Ptr>&& loadedPair):
    study_(std::move(loadedPair.first)),
    resultWriter_(std::move(loadedPair.second))
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
    initConstantData();
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
    nbWeeks_ = static_cast<int>(study_->parameters.simulationDays.numberOfWeeks());
    std::map<unsigned int, bool> isYearPerformed; // TODO check year number
    for (uint year = 0; year < study_->parameters.nbYears; ++year)
    {
        isYearPerformed[year] = study_->parameters.yearsFilter[year];
        if (study_->parameters.yearsFilter[year])
        {
            ++nbYears;
            playedYears_.insert(static_cast<unsigned>(year));
        }
    }

    randomForParallelYears_.emplace(nbYears, study_->parameters.power.fluctuations);
    randomForParallelYears_->allocate(*study_);

    MersenneTwister randomHydroGenerator;
    randomHydroGenerator.reset(study_->parameters.seed[Data::seedHydroManagement]);
    const unsigned int finalYear = 1 + study_->runtime.rangeLimits.year[Data::rangeEnd];
    randomForParallelYears_->compute(*study_, finalYear, isYearPerformed, randomHydroGenerator);
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
    // Ensure study metadata is exported (about-the-study directory)
    study_->saveAboutTheStudy(*writer);
}

void fillLinksProperties(PROBLEME_HEBDO& pb, const Antares::Data::Study& study)
{
    const auto& studyruntime = study.runtime;
    for (uint k = 0; k < studyruntime.interconnectionsCount(); ++k)
    {
        const auto* lnk = studyruntime.areaLink[k];
        pb.CoutDeTransport[k].IntercoGereeAvecDesCouts = lnk->useHurdlesCost;
    }
}

struct Tag
{
    std::string_view name;
    unsigned base;
};

constexpr std::array<Tag, 3> tags = {{{"::hour<", 168}, {"::day<", 7}, {"::week<", 1}}};

std::vector<NameMemo> buildMemo(const std::vector<std::string>& names)
{
    std::vector<NameMemo> mem;
    // Build memo once
    for (std::size_t i = 0; i < names.size(); ++i)
    {
        const std::string& s = names[i];
        for (const auto& [tag, base]: tags)
        {
            const size_t tagLen = tag.size();
            std::size_t tagPos = s.find(tag);
            if (tagPos == std::string::npos)
            {
                continue;
            }

            std::size_t numStart = tagPos + tagLen;
            std::size_t end = s.find('>', numStart);
            if (end == std::string::npos)
            {
                continue;
            }

            mem.emplace_back(numStart, end, std::stoi(s.substr(numStart, end - numStart)), i, base);
            break;
        }
    }

    return mem;
}

void SingleProblemGetter::initConstantData()
{
    // IntercoGereeAvecDesCouts needs to be initialized
    // before building variable list and the common matrix
    fillLinksProperties(pb_, *study_);

    OPT_NumeroDeJourDuPasDeTemps(&pb_);

    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeLineaire(&pb_);

    auto builder_data = NewGetConstraintBuilderFromProblemHebdo(&pb_);
    ConstraintBuilder builder(builder_data);
    LinearProblemMatrix linearProblemMatrix(&pb_, builder);
    linearProblemMatrix.Run();
    auto* PbAResoudre = pb_.ProblemeAResoudre.get();
    variablesMemo_ = buildMemo(PbAResoudre->NomDesVariables);
    constraintsMemo_ = buildMemo(PbAResoudre->NomDesContraintes);
    variablesName_ = PbAResoudre->NomDesVariables;
    constraintsName_ = PbAResoudre->NomDesContraintes;
}

ConstantDataFromAntares SingleProblemGetter::getConstantData() const
{
    return translator_.commonProblemData(pb_.ProblemeAResoudre.get());
}

std::vector<std::string> applyTimeOffset(const std::vector<std::string>& in,
                                         const std::vector<NameMemo>& mem,
                                         unsigned week)
{
    auto names = in;
    for (const auto& [left_end, right_begin, baseTime, index, base]: mem)
    {
        std::string& s = names[index];
        s = s.substr(0, left_end) + std::to_string(baseTime + week * base)
            + s.substr(static_cast<size_t>(right_begin));
    }
    return names;
}

void updateWeekId(WeeklyProblemId& id)
{
    auto& [year, week] = id;
    // by convention, weeks start at 1 from the caller's POV, but at 0 in Simulator
    if (week == 0)
    {
        throw std::out_of_range("Invalid week number 0 detected, week number must be >=1");
    }
    --week;
}

void SingleProblemGetter::setWeeklyData(WeeklyProblemId& id)
{
    updateWeekId(id);
    pb_.year = id.year;
    pb_.weekInTheYear = id.week;

    const auto [hydroLevels, ventilationResults] = getYearlyData(id.year);

    uint indexYear = randomForParallelYears_->yearNumberToIndex[id.year];
    auto& randomForCurrentYear = randomForParallelYears_->pYears[indexYear];
    // TODO
    if (auto [_, unseen] = randomPrepared_.insert(static_cast<int>(id.year)); unseen)
    {
        // TODO once per year, not every week
        Antares::Solver::Simulation::PrepareRandomNumbers(*study_, pb_, randomForCurrentYear);
    }

    const auto hourInTheYear = 168 * id.week; // TODO
    pb_.HeureDansLAnnee = hourInTheYear;
    // Apply hydro levels
    for (uint areaIndex = 0; areaIndex < study_->areas.size(); ++areaIndex)
    {
        const auto* area = study_->areas.byIndex[areaIndex];
        if (area->hydro.reservoirManagement)
        {
            double initialLevel = hydroLevels.at(area)[id.week];
            pb_.previousSimulationFinalLevel[areaIndex] = initialLevel;
        }
    }

    SIM_RenseignementProblemeHebdo(*study_,
                                   pb_,
                                   id.week,
                                   hourInTheYear,
                                   ventilationResults,
                                   scratchmap_);

    // required at least for OPT_SommeDesPminThermiques (RHS)
    Antares::Solver::Simulation::BuildThermalPartOfWeeklyProblem(
      *study_,
      pb_,
      hourInTheYear,
      randomForCurrentYear.pThermalNoisesByArea,
      id.year);

    OPT_VerifierPresenceReserveJmoins1(&pb_);

    OPT_InitialiserLesPminHebdo(&pb_);

    OPT_InitialiserLesContrainteDEnergieHydrauliqueParIntervalleOptimise(&pb_);

    OPT_MaxDesPmaxHydrauliques(&pb_);

    if (pb_.OptimisationAvecCoutsDeDemarrage)
    {
        OPT_InitialiserNombreMinEtMaxDeGroupesCoutsDeDemarrage(&pb_);
    }

    OPT_RestaurerLesDonnees(&pb_);

    OPT_InitialiserLesBornesDesVariablesDuProblemeLineaire(&pb_,
                                                           PremierPdtDeLIntervalle,
                                                           DernierPdtDeLIntervalle);

    OPT_InitialiserLeSecondMembreDuProblemeLineaire(&pb_,
                                                    PremierPdtDeLIntervalle,
                                                    DernierPdtDeLIntervalle,
                                                    numeroDeLIntervalle,
                                                    optimizationNumber);

    OPT_InitialiserLesCoutsLineaire(&pb_, PremierPdtDeLIntervalle, DernierPdtDeLIntervalle);
}

WeeklyDataFromAntares SingleProblemGetter::getWeeklyData(WeeklyProblemId id)
{
    setWeeklyData(id);
    // by convention, weeks start at 1 from the caller's POV, but at 0 in Simulator
    return translator_.translate(pb_.ProblemeAResoudre.get(), problemName({id.year, id.week + 1}));
}

std::unique_ptr<ILinearProblem> SingleProblemGetter::getWeeklyProblem(WeeklyProblemId id)
{
    setWeeklyData(id);
    auto& ProblemeAResoudre = pb_.ProblemeAResoudre;

    if (id.week == 0 && id.year != 0)
    {
        ProblemeAResoudre->NomDesVariables = variablesName_;
        ProblemeAResoudre->NomDesContraintes = constraintsName_;
    }
    if (id.week != 0)
    {
        ProblemeAResoudre->NomDesVariables = applyTimeOffset(variablesName_,
                                                             variablesMemo_,
                                                             id.week);
        ProblemeAResoudre->NomDesContraintes = applyTimeOffset(constraintsName_,
                                                               constraintsMemo_,
                                                               id.week);
    }

    std::unique_ptr<ILinearProblem> linearProblem = std::make_unique<
      Antares::Optimisation::LinearProblemApi::StructuredLinearProblem>();
    fillProblem(*linearProblem, id);

    return linearProblem;
}

void SingleProblemGetter::fillProblem(ILinearProblem& problem, const WeeklyProblemId& id)
{
    const int opt = optimizationNumber - 1;
    assert(opt >= 0 && opt < 2);
    Optimisation::LinearProblemApi::FillContext fillCtx = buildFillContext(&pb_,
                                                                           numeroDeLIntervalle);
    const auto modelerData = pb_.modelerData;
    bool hasModelerData = modelerData != nullptr;
    const ILinearProblemData* modelerDataSeries = hasModelerData ? modelerData->dataSeries.get()
                                                                 : nullptr;
    const Optimisation::ScenarioGroupRepository* modelerScenarioGroupRepository
      = hasModelerData ? &modelerData->scenarioGroupRepository : nullptr;

    Optimisation::OptimEntityContainer optimEntityContainer(problem);
    if (hasModelerData)
    {
        modelerData->bendersDecomposition.setCurrentProblemId(problemName({id.year, id.week + 1}));
    }

    fillLinearProblem(fillCtx,
                      &pb_,
                      optimEntityContainer,
                      true,
                      &modelerData->bendersDecomposition);
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
    for (const auto& area: study_->areas | std::views::values)
    {
        if (!area->hydro.reservoirManagement)
        {
            areaIndex++;
            continue;
        }
        auto inflows = area->hydro.series->storage.getColumn(year);
        auto& level = hydroLevels[area.get()];

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

bool SingleProblemGetter::areWeeksIndependent() const
{
    return std::ranges::all_of(study_->areas | std::views::values,
                               [&](const auto& area)
                               {
                                   const auto& hydro = area->hydro;
                                   return !hydro.reservoirManagement
                                          || (hydro.useHeuristicTarget && !hydro.useLeeway);
                               });
}

void writeWeekMPS(const std::unique_ptr<ILinearProblem>& weekly,
                  const WeeklyProblemId& id,
                  IResultWriter::Ptr& resultWriter)
{
    auto name = problemName(id);

    IO::Outputs::MPSGenerator mpsGenerator(*weekly, name + ".mps");
    std::string mps = mpsGenerator.run();

    logs.info() << "Printing problem: " << name << '\n';

    resultWriter->addEntryFromBuffer(name + ".mps", mps);
}

Solver::ProblemEntity SingleProblemGetter::getMasterProblem() const
{
    using namespace Antares::Solver;
    using namespace Antares::Optimisation;
    using namespace Antares::Optimisation::LinearProblemApi;

    logs.info() << "Building master problem and Benders decomposition...";

    FillContext fillContext = {0, 167, 0, 167, 0};
    return buildProblem(*pb_.modelerData,
                        Config::Location::MASTER,
                        "master",
                        &pb_.modelerData->bendersDecomposition,
                        fillContext,
                        ResolutionMode::BENDERS_DECOMPOSITION,
                        std::nullopt);
}

void SingleProblemGetter::writeMasterAndStructure() const
{
    using namespace Antares::Solver;
    using namespace Antares::Optimisation;
    using namespace Antares::Optimisation::LinearProblemApi;

    logs.info() << "Building master problem and Benders decomposition...";

    FillContext fillContext = {0, 167, 0, 167, 0};

    auto [masterProblem, _] = getMasterProblem();

    if (!masterProblem)
    {
        logs.warning() << "Master problem is empty - not writing master.mps or structure.txt";
        return;
    }

    auto mps = IO::Outputs::MPSGenerator(*masterProblem, "master").run();
    resultWriter_->addEntryFromBuffer("master.mps", mps);
    logs.info() << "Written: " << "master.mps";

    BendersDecompositionWriter writer(pb_.modelerData->bendersDecomposition);
    auto structureFile = writer.getBendersDecomposition();
    resultWriter_->addEntryFromBuffer("structure.txt", structureFile);
    logs.info() << "Written: " << "structure.txt";
}

void SingleProblemGetter::printProblems()
{
    if (areWeeksIndependent())
    {
        logs.info() << "Weeks are independent";
    }
    else
    {
        logs.warning() << "Weeks are dependent, the printed MPS files may differ from those "
                          "produced by antares-solver because of hydro levels";
    }

    logs.info() << " * Number of years: " << playedYears_.size();
    logs.info() << " * Number of weeks per year: " << nbWeeks_;

    auto problemIds = getProblemIds();
    logs.info() << " * Number of problems to process: " << problemIds.size();

    for (const auto& id: problemIds)
    {
        logs.info() << " year: " << id.year << ", week: " << id.week;
        auto weekly = getWeeklyProblem(id);
        writeWeekMPS(weekly, id, resultWriter_);
    }

    if (pb_.modelerData)
    {
        writeMasterAndStructure();
    }
}
} // namespace Antares::Solver::Implementation
