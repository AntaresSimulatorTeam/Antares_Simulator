// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <yuni/yuni.h>

#include "antares/study/fwd.h"

#include "ortools_utils.h"

namespace Antares::LinearProblem::Api
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
    virtual void runIfNeeded(Antares::Solver::IResultWriter& writer, const std::string& filename)
      = 0;

protected:
    uint current_optim_number_ = 0;
};

class MPSwriter: public I_MPS_writer
{
public:
    ~MPSwriter() override = default;
    MPSwriter(const Antares::LinearProblem::Api::ILinearProblem& linearProblem,
              uint currentOptimNumber,
              bool keepNames);
    void runIfNeeded(Antares::Solver::IResultWriter& writer, const std::string& filename) override;

private:
    const Antares::LinearProblem::Api::ILinearProblem& linearProblem_;
    bool keepNames_;
};

class nullMPSwriter: public I_MPS_writer
{
public:
    ~nullMPSwriter() override = default;
    using I_MPS_writer::I_MPS_writer;

    void runIfNeeded(Antares::Solver::IResultWriter& /*writer*/,
                     const std::string& /*filename*/) override
    {
        // Does nothing
    }
};

class mpsWriterFactory
{
public:
    virtual ~mpsWriterFactory() = default;
    mpsWriterFactory(Antares::Data::mpsExportStatus exportMPS,
                     bool exportMPSOnError,
                     int current_optim_number,
                     const Antares::LinearProblem::Api::ILinearProblem& linearProblem);

    std::unique_ptr<I_MPS_writer> create(bool keepNames);
    std::unique_ptr<I_MPS_writer> createOnOptimizationError();

private:
    // Member functions...
    std::unique_ptr<I_MPS_writer> createFullmpsWriter(bool keepNames);
    bool doWeExportMPS();

    // Member data...
    Antares::Data::mpsExportStatus export_mps_;
    bool export_mps_on_error_;
    const Antares::LinearProblem::Api::ILinearProblem& linearProblem_;
    uint current_optim_number_;
};
