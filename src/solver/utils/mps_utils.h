#pragma once

extern "C"
{
#include "spx_definition_arguments.h"
#include "spx_fonctions.h"

#include "srs_api.h"
}

#include "named_problem.h"
#include "ortools_utils.h"

using namespace Antares;
using namespace Antares::Data;
using namespace Antares::Optimization;
using namespace operations_research;

// ======================
// MPS files writing
// ======================

class I_MPS_writer
{
public:
    explicit I_MPS_writer(uint currentOptimNumber) : current_optim_number_(currentOptimNumber)
    {}
    I_MPS_writer() = default;
    virtual void runIfNeeded(Solver::IResultWriter::Ptr writer, const std::string & filename) = 0;

protected:
    uint current_optim_number_ = 0;
};

class fullMPSwriter final : public I_MPS_writer
{
public:
    fullMPSwriter(PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                  uint currentOptimNumber);
    void runIfNeeded(Solver::IResultWriter::Ptr writer, const std::string & filename) override;

private:
    PROBLEME_SIMPLEXE_NOMME* named_splx_problem_ = nullptr;
};

class fullOrToolsMPSwriter : public I_MPS_writer
{
public:
    fullOrToolsMPSwriter(MPSolver* solver, 
                         uint currentOptimNumber);
    void runIfNeeded(Solver::IResultWriter::Ptr writer, const std::string & filename) override;

private:
    MPSolver* solver_ = nullptr;
};

class nullMPSwriter : public I_MPS_writer
{
public:
    using I_MPS_writer::I_MPS_writer;
    void runIfNeeded(Solver::IResultWriter::Ptr /*writer*/, const std::string& /*filename*/) override
    {
        // Does nothing
    }
};

class mpsWriterFactory
{
public:
    mpsWriterFactory(PROBLEME_HEBDO* ProblemeHebdo,
                     const int current_optim_number,
                     PROBLEME_SIMPLEXE_NOMME* named_splx_problem,
                     bool ortoolsUsed,
                     MPSolver* solver);

    std::unique_ptr<I_MPS_writer> create();
    std::unique_ptr<I_MPS_writer> createOnOptimizationError();

private:
    // Member functions...
    std::unique_ptr<I_MPS_writer> createFullmpsWriter();
    bool doWeExportMPS();

    // Member data...
    PROBLEME_HEBDO* pb_hebdo_ = nullptr;
    PROBLEME_SIMPLEXE_NOMME* named_splx_problem_ = nullptr;
    bool ortools_used_;
    MPSolver* solver_ = nullptr;
    uint current_optim_number_;
    Data::mpsExportStatus export_mps_;
    bool export_mps_on_error_;
};
