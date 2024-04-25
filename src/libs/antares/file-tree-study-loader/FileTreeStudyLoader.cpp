
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

#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/application/application.h"
#include <memory>
#include <span>
#include <cstring>

namespace Antares
{
FileTreeStudyLoader::FileTreeStudyLoader(std::filesystem::path study_path)
: study_path_{std::move(study_path)}
{

}

namespace {
/**
 * @brief Prepares arguments for the Antares Solver application.
 *
 * This function prepares the arguments required by the Antares Solver application.
 * It takes a span of char pointers and a string_view representing the study path.
 * The function creates copies of the required arguments and stores them in a vector of unique_ptr to char arrays.
 * The original char pointers in the span are updated to point to the newly created copies.
 * Lifetime of values inside argv is determined be the content of the returned vector
 *
 * @param argv A span of char pointers to be filled with the prepared arguments.
 * @param study_path A string_view representing the study path.
 * @return std::vector<std::unique_ptr<char[]>> A vector of unique_ptr to char arrays containing the prepared arguments.
 */
[[nodiscard]] std::vector<std::string> prepareArgs(std::span<char*> argv, std::string_view study_path)
{
    using namespace std::literals::string_literals;
    std::string arg0{""s};
    std::string arg1{study_path};
    std::string arg2{"-s"s};
    argv[0] = arg0.data();
    argv[1] = arg1.data();
    argv[2] = arg2.data();
    return {std::move(arg0), std::move(arg1), std::move(arg2)};
}
} // namespace

std::unique_ptr<Antares::Data::Study> FileTreeStudyLoader::load() const
{
    using namespace std::literals::string_literals;
    Antares::Solver::Application application;
    constexpr unsigned int argc = 3;
    std::array<char*, argc> argv;
    auto keep_alive = prepareArgs(argv, study_path_.string());
    application.prepare(argc, argv.data());

    return application.acquireStudy();
}

} // namespace Antares