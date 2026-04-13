// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "antares/solver/lps/LpsFromAntares.h"
#include "antares/solver/modeler/Modeler.h"
#include "antares/solver/optim-model-filler/BendersDecomposition.h"
#include "antares/study/study.h"
#include "antares/writer/i_writer.h"

namespace Antares::Solver::Implementation
{

class SingleProblemGetterModeler final
{
public:
    explicit SingleProblemGetterModeler(const std::filesystem::path& modelerPath);

    void printProblems() const;

private:
    std::unique_ptr<Modeler> modeler_;
    std::unique_ptr<Antares::Data::Study> study_;
    Solver::IResultWriter::Ptr resultWriter_;
};

} // namespace Antares::Solver::Implementation
