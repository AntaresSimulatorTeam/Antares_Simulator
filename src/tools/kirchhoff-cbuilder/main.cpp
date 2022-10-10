/*
** Copyright 2007-2022 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include <antares/resources/resources.h>
#include <antares/sys/policy.h>
#include <antares/version.h>
#include <antares/locale.h>
#include <antares/utils.h>
#include <antares/study.h>
#include <antares/logs.h>
#include <string>

#include "../../solver/constraints-builder/cbuilder.h"

#include "kirchhoff-cbuilder.h"

using namespace Yuni;
using namespace Antares;


int main(int argc, char* argv[])
{
    logs.applicationName("k-cbuild");
    if (argc < 3)
    {
        logs.error() << "Not enough arguments, exiting.";
        logs.error() << "args: study_path, option_path";
        return EXIT_FAILURE;
    }

    std::string studyPath(argv[1]);
    std::string kirchhoffOptionPath(argv[2]);

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
        return EXIT_FAILURE;

    return 0;
}

bool runKirchhoffConstraints(std::shared_ptr<Data::Study> study,
    std::string & studyPath, std::string & kirchhoffOptionPath)
{
    bool result = true;
    CBuilder constraintBuilder(study);
    logs.info() << "CBuilder created";

    result = constraintBuilder.completeFromStudy() && result;
    result = constraintBuilder.completeCBuilderFromFile(kirchhoffOptionPath) && result;

    if (!result)
    {
        logs.error() << "CBuilder init went wrong, aborting.";
        return false;
    }

    logs.info() << "CBuilder completed study and option file.";

    result = constraintBuilder.runConstraintsBuilder();
    if (!result)
    {
        logs.error() << "Run constraints failed.";
        return false;
    }

    result = study->bindingConstraints.saveToFolder(studyPath + "/input/bindingconstraints");
    if (!result)
        logs.error() << "Save to folder failed";

    return result;
}


static void NotEnoughMemory()
{
    logs.fatal() << "Not enough memory. aborting.";
}

bool initResources(int argc, char* argv[])
{
    std::set_new_handler(&NotEnoughMemory);
    if (!memory.initialize())
    {
        logs.error() << "Failed to initialize memory.";
        return false;
    }

    InitializeDefaultLocale();

    if (!LocalPolicy::Open())
        return false;

    LocalPolicy::CheckRootPrefix(argv[0]);

    Resources::Initialize(argc, argv, true);
    return true;
}

bool initComponents(std::shared_ptr<Data::Study> study, std::string &studyPath)
{
    study->header.version = Data::StudyHeader::ReadVersionFromFile(studyPath + "/study.antares");
    study->folder = studyPath;
    study->folderInput = studyPath + "/input";
    study->inputExtension = "txt";

    Data::StudyLoadOptions options;
    options.loadOnlyNeeded = false;

    if(!study->areas.loadFromFolder(options))
    {
        logs.error() << "Areas loading failed";
        return false;
    }
    logs.info() << "Areas loaded.";

    study->loadLayers(studyPath + "layers/layers.ini");
    logs.debug() << "active layer ID: " << study->activeLayerID;

    if (!study->bindingConstraints.loadFromFolder(*study,
        options, studyPath + "/input/bindingconstraints/"))
    {
        logs.error() << "Binding constraints loading failed";
        return false;
    }
    logs.info() << "Binding constraints loaded.";

    Data::Study::Current::Set(study);

    logs.notice() << studyPath << " is loaded.";

    return true;
}
