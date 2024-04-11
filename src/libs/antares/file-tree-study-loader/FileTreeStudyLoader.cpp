
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

namespace Antares
{
FileTreeStudyLoader::FileTreeStudyLoader(std::filesystem::path study_path)
: study_path_{std::move(study_path)}
{

}

namespace {
std::unique_ptr<char[]> copy(std::string_view str) {
    auto copy = std::unique_ptr<char[]>(new char[str.size() + 1]);
    std::strcpy(copy.get(), str.data());
    return copy;
}

void prepareArgs(std::array<char*, 3>& argv, std::string_view study_path)
{
    using namespace std::literals::string_literals;
    auto arg0 = copy(""s);
    auto arg1 = copy(study_path);
    auto arg2 = copy("-s"s);
    argv[0] = arg0.get();
    argv[1] = arg1.get();
    argv[2] = arg2.get();
}
} // namespace


std::shared_ptr<Antares::Data::Study> FileTreeStudyLoader::load() {
    Antares::Solver::Application application;
    constexpr unsigned int argc = 3;
    std::array<char*, argc> argv;
    prepareArgs(argv, study_path_.string());
    application.prepare(argc, argv.data());

    return application.study();
}

} // namespace Antares