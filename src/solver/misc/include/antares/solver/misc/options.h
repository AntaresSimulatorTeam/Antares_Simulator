// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <memory>

#include <yuni/yuni.h>
#include <yuni/core/getopt.h>
#include <yuni/core/string.h>

#include <antares/optimization-options/options.h>
#include <antares/study/study.h>

//! Selects which output families to write

class OutputSelection
{
public:
    enum Value
    {
        All,            // --output=all
        None,           // --output=none
        MonteCarlo,     // --output=monte-carlo
        SimulationTable // --output=simulation-table
    };

    OutputSelection() = default;

    explicit OutputSelection(Value v):
        value_(v)
    {
    }

    bool shouldExportMonteCarloResults() const
    {
        return value_ == All || value_ == MonteCarlo;
    }

    bool shouldExportSimulationTable() const
    {
        return value_ == All || value_ == SimulationTable;
    }

    bool operator==(Value v) const
    {
        return value_ == v;
    }

private:
    Value value_ = MonteCarlo;
};

/*!
** \brief Command line settings for launching the simulation
*/
class Settings final
{
public:
    void reset();

    //! Where is my study
    YString studyFolder;
    //! Name of the simulation
    Yuni::CString<150, false> simulationName;
    //! Comment file
    std::string commentFile;

    //! Simplex optimizatio range
    Yuni::CString<32, false> simplexOptimRange;

    //! Ignore loading errors
    bool ignoreLoadingErrors = false;
    //! Ignore constraints
    bool ignoreConstraints = false;

    //! Run the TS generator only
    bool tsGeneratorsOnly = false;

    //! Override the study's output selection
    OutputSelection outputSelection;

    //! Raw string from CLI, converted to outputSelection after parsing
    std::string outputSelectionStr;

    //! Convert outputSelectionStr to outputSelection (call after parsing)
    void resolveOutputSelection();

    // In case we print simulation tables, do we print it in csv or parquet ?
    bool parquetFmtForSimuTables = false;

    Yuni::String PID;
    bool forceZipOutput = false;
    Antares::Solver::Optimization::CmdLineOptimOptions solverOptions;
}; // class Settings

/*!
** \brief Interprete options given in command line
*/
std::unique_ptr<Yuni::GetOpt::Parser> CreateParser(Settings& settings,
                                                   Antares::Data::StudyLoadOptions& options);

void checkStudyFolder(const std::string& studyFolder);
std::string fixStudyFolder(const std::string& studyFolder);
void printPIDtoDisk(const Settings& settings);
void checkAndCorrectSettingsAndOptions(Settings& settings,
                                       Antares::Data::StudyLoadOptions& options);
