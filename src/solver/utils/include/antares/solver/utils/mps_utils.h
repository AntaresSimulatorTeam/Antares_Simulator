// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/fwd.h"

#include "ortools_utils.h"

namespace Antares::Optimisation::LinearProblemApi
{
class ILinearProblem;
}

// ======================
// MPS files writing
// ======================

class I_MPS_writer
{
public:
    explicit I_MPS_writer(uint currentOptimNumber):
        current_optim_number_(currentOptimNumber)
    {
    }

    I_MPS_writer() = default;
    virtual ~I_MPS_writer() = default;
    virtual void runIfNeeded(Solver::IResultWriter& writer, const std::string& filename) = 0;

protected:
    uint current_optim_number_ = 0;
};

class MPSwriter: public I_MPS_writer
{
public:
    virtual ~MPSwriter() = default;
    MPSwriter(const Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
              uint currentOptimNumber,
              bool keepNames);
    void runIfNeeded(Solver::IResultWriter& writer, const std::string& filename) override;

private:
    const Optimisation::LinearProblemApi::ILinearProblem& linearProblem_;
    bool keepNames_;
};

class nullMPSwriter: public I_MPS_writer
{
public:
    virtual ~nullMPSwriter() = default;
    using I_MPS_writer::I_MPS_writer;

    void runIfNeeded(Solver::IResultWriter& /*writer*/, const std::string& /*filename*/) override
    {
        // Does nothing
    }
};

class mpsWriterFactory
{
public:
    virtual ~mpsWriterFactory() = default;
    mpsWriterFactory(Data::mpsExportStatus exportMPS,
                     bool exportMPSOnError,
                     int current_optim_number,
                     const Optimisation::LinearProblemApi::ILinearProblem& linearProblem);

    std::unique_ptr<I_MPS_writer> create(bool keepNames);
    std::unique_ptr<I_MPS_writer> createOnOptimizationError();

private:
    // Member functions...
    std::unique_ptr<I_MPS_writer> createFullmpsWriter(bool keepNames);
    bool doWeExportMPS();

    // Member data...
    Data::mpsExportStatus export_mps_;
    bool export_mps_on_error_;
    const Optimisation::LinearProblemApi::ILinearProblem& linearProblem_;
    uint current_optim_number_;
};
