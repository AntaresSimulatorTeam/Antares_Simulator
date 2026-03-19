
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <filesystem>

#include "antares/study-loader/IStudyLoader.h"

namespace Antares
{
namespace Data
{
class Study;
}

/**
 * @class FileTreeStudyLoader
 * @brief A class to load studies from the file tree.
 *
 * This class is responsible for loading studies from files in file system.
 *
 * @note This class inherits from the IStudyLoader interface.
 */
class FileTreeStudyLoader: public IStudyLoader
{
public:
    explicit FileTreeStudyLoader(std::filesystem::path study_path);
    ~FileTreeStudyLoader() override = default;

    /**
     * @brief Loads a study from the file tree.
     *
     * This function prepares the arguments required by the Antares Solver application and then
     * calls the application's prepare method.
     * It then returns the study loaded by the application.
     *
     * @return std::shared_ptr<Antares::Data::Study> A shared_ptr to the loaded Study object.
     */
    [[nodiscard]] std::pair<std::unique_ptr<Data::Study>, std::shared_ptr<Solver::IResultWriter>>
    load() const override;

private:
    std::string study_path_;
};
} // namespace Antares
