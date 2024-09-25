/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <string>

#include <antares/locale/locale.h>
#include <antares/logs/logs.h>
#include <antares/resources/resources.h>
#include <antares/study/study.h>
#include <antares/sys/policy.h>
#include "antares/solver/constraints-builder/cbuilder.h"

#include "kirchhoff-cbuilder.h"

using namespace Yuni;
using namespace Antares;

int main(int argc, const char* argv[])
{
    logs.applicationName("k-cbuild");
    if (argc < 2)
    {
        logs.error() << "Not enough arguments, exiting.";
        logs.error() << "args: study_path, option_path";
        return EXIT_FAILURE;
    }

    std::string studyPath(argv[1]);
    std::string kirchhoffOptionPath;
    if (argc > 2)
    {
        kirchhoffOptionPath = argv[2];
    }

    if (!initResources(argc, argv))
    {
        logs.error() << "Init resources failed.";
        return EXIT_FAILURE;
    }

    auto study = std::make_shared<Data::Study>();

    if (!initComponents(study, studyPath))
    {
        logs.error() << "Init components failed.";
        return EXIT_FAILURE;
    }

    if (!runKirchhoffConstraints(study, studyPath, kirchhoffOptionPath))
    {
        return EXIT_FAILURE;
    }

    return 0;
}

bool runKirchhoffConstraints(std::shared_ptr<Data::Study> study,
                             const std::string& studyPath,
                             const std::string& kirchhoffOptionPath)
{
    study->areas.ensureDataIsInitialized(study->parameters, false);

    CBuilder constraintBuilder(*study);
    logs.info() << "CBuilder created";

    if (!constraintBuilder.completeFromStudy())
    {
        logs.error() << "CBuilder complete from study went wrong, aborting.";
        return false;
    }

    if (kirchhoffOptionPath != "")
    {
        if (!constraintBuilder.completeCBuilderFromFile(kirchhoffOptionPath))
        {
            logs.error() << "CBuilder complete from option file went wrong, aborting.";
            return false;
        }
    }

    logs.info() << "CBuilder completed study and option file.";

    if (!constraintBuilder.runConstraintsBuilder())
    {
        logs.error() << "Run constraints failed.";
        return false;
    }

    auto bindingPath = studyPath + Yuni::IO::Separator + "input" + Yuni::IO::Separator
                       + "bindingconstraints";

    if (!study->bindingConstraints.saveToFolder(bindingPath))
    {
        logs.error() << "Save to folder failed";
        return false;
    }

    return true;
}

static void NotEnoughMemory()
{
    logs.fatal() << "Not enough memory. aborting.";
}

bool initResources(int argc, const char* argv[])
{
    std::set_new_handler(&NotEnoughMemory);

    InitializeDefaultLocale();
    if (!LocalPolicy::Open())
    {
        return false;
    }

    LocalPolicy::CheckRootPrefix(argv[0]);

    Resources::Initialize(argc, argv, true);
    return true;
}

bool initComponents(std::shared_ptr<Data::Study> study, const std::string& studyPath)
{
    study->header.version = Data::StudyHeader::tryToFindTheVersion(studyPath);
    if (study->header.version == Data::StudyVersion::unknown())
    {
        return false;
    }
    study->folder = studyPath;
    study->folderInput = studyPath + Yuni::IO::Separator + "input";

    logs.info() << "Study version: " << study->header.version.toString();

    Data::StudyLoadOptions options;
    options.loadOnlyNeeded = false;

    if (!study->areas.loadFromFolder(options))
    {
        logs.error() << "Areas loading failed";
        return false;
    }
    logs.info() << "Areas loaded.";

    auto bindingPath = studyPath + Yuni::IO::Separator + "input" + Yuni::IO::Separator
                       + "bindingconstraints";

    if (!study->bindingConstraints.loadFromFolder(*study, options, bindingPath))
    {
        logs.error() << "Binding constraints loading failed";
        return false;
    }
    logs.info() << "Binding constraints loaded.";
    logs.notice() << studyPath << " is loaded.";

    return true;
}
