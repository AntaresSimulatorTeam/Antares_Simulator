
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

std::shared_ptr<Antares::Data::Study> FileTreeStudyLoader::load() {
    Antares::Solver::Application application;
    using namespace std::literals::string_literals;
    char* argv[] = {"api", "", "-s"};
    auto argc = 3;
    auto arg1 = study_path_.string().data();
    auto arg2 = "-s"s;
    argv[1] = arg1;
    argv[2] = arg2.data();

    application.prepare(argc, argv);

    return application.study();
}

} // namespace Antares