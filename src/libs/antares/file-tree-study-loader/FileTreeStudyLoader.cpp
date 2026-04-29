// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"

#include <memory>

#include "antares/application/application.h"

namespace Antares
{
FileTreeStudyLoader::FileTreeStudyLoader(std::filesystem::path study_path):
    study_path_{study_path.string()}
{
}

std::pair<std::unique_ptr<Data::Study>, Solver::IResultWriter::Ptr> FileTreeStudyLoader::load()
  const
{
    using namespace std::literals::string_literals;
    Antares::Solver::Application application;
    constexpr unsigned int argc = 3;
    std::array<const char*, argc> argv{"", study_path_.c_str(), "--parallel"};
    application.prepare(argc, argv.data());

    return application.acquireStudy();
}

} // namespace Antares
