
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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
    [[nodiscard]] std::unique_ptr<Antares::Data::Study> load() const override;

private:
    std::string study_path_;
};
} // namespace Antares
