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

#include <antares/version.h>
#include <antares/locale.h>
#include <antares/utils.h>
#include <antares/study.h>
#include <antares/logs.h>
#include <string>

#include "../../solver/constraints-builder/cbuilder.h"

using namespace Yuni;
using namespace Antares;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        logs.error() << "Not enough arguments, exiting";
        logs.error() << "args: study_path, option_path";
        return 1;
    }

    std::string studyPath(argv[1]);
    std::string kirchhoffOptionPath(argv[2]);

    auto study = std::make_shared<Data::Study>(true);

    Data::StudyLoadOptions options;
    options.loadOnlyNeeded = false;
    if (!study->loadFromFolder(studyPath, options))
    {
        logs.error() << "Couldn't load study from file, exiting";
        return 1;
    }

    logs.info();
    study->ensureDataAreAllInitialized();

    logs.info() << "The study is loaded.";

    Data::Study::Current::Set(study);
    JIT::enabled = true;

    CBuilder constraintBuilder(study);
    constraintBuilder.completeFromStudy();
    constraintBuilder.completeCBuilderFromFile(kirchhoffOptionPath);

    const bool result = constraintBuilder.runConstraintsBuilder();

    logs.info() << "Result: " << result;

    return 0;
}
