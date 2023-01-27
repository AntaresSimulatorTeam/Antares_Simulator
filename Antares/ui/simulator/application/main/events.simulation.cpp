/*
** Copyright 2007-2018 RTE
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

#include "../main.h"
#include "../study.h"
#include "../../windows/simulation/run.h"
#include "../../windows/analyzer/analyzer.h"
#include "../../windows/constraints-builder/constraintsbuilder.h"
#include "internal-data.h"
#include "../../config.h"
#include <yuni/datetime/timestamp.h>
#include "../../toolbox/dispatcher/study.h"
#include <antares/study/version.h>

using namespace Yuni;

namespace Antares
{
namespace Forms
{
static bool SimulationCheck(const Data::Study& study)
{
    if (study.parameters.readonly)
    {
        logs.error() << "The study is read-only."
                        "\n"
                        "Please save the study into another folder first.";
        return false;
    }
    if (study.areas.empty())
    {
        logs.error() << "The study must contain at least one area";
        return false;
    }
    return true;
}

template<class ParentT>
static void ShowSimulationPanel(ParentT* parent, bool preproOnly)
{
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        if (SimulationCheck(*study))
        {
            // making sure that the parent window has been refreshed
            Antares::Dispatcher::GUI::Refresh(parent);

            // disable the main form
            Forms::Disabler<ApplWnd> disabler(*parent);

            // run the simulation input dialog
            auto* runS = new Window::Simulation::Run(parent, preproOnly);
            runS->ShowModal();
            runS->Destroy();
        }
    }
}

void MainFormData::onToolbarRunSimulation(void*)
{
    // the call to this window must be delayed to avoid issues with other main loop
    // events, from the output viewer for example
    // (already delayed by the toolbar button)
    ShowSimulationPanel(&pMainForm, false);
}

void ApplWnd::evtOnRunSimulation(wxCommandEvent&)
{
    // the call to this window must be delayed to avoid issues with other main loop
    // events, from the output viewer for example
    Yuni::Bind<void()> callback;
    callback.bind(pData, &MainFormData::onToolbarRunSimulation, nullptr);
    Dispatcher::GUI::Post(callback);
}

void ApplWnd::evtOnRunTSGenerators(wxCommandEvent&)
{
    Dispatcher::GUI::Post(this, &ApplWnd::evtOnRunTSGeneratorsDelayed);
}

void ApplWnd::evtOnRunTSGeneratorsDelayed()
{
    ShowSimulationPanel(this, true);
}

void ApplWnd::evtOnRunTSAnalyzer(wxCommandEvent&)
{
    Dispatcher::GUI::Post(this, &ApplWnd::evtOnRunTSAnalyzerDelayed);
}

void ApplWnd::evtOnRunTSAnalyzerDelayed()
{
    if (not Data::Study::Current::Valid())
    {
        logs.error() << "No study opened";
        return;
    }
    auto& study = *Data::Study::Current::Get();

    if (SimulationCheck(study))
    {
        if (study.folder.empty() || study.folderInput.empty())
        {
            logs.error() << "The study must be saved before launching the analyzer";
            return;
        }
        if (study.header.version != Data::versionLatest)
        {
            logs.error() << "The study must be upgraded to the v"
                         << Data::VersionToCStr((Data::Version)Data::versionLatest)
                         << " format before launching the analyzer";
            return;
        }

        Forms::Disabler<ApplWnd> disabler(*this);

        auto* form = new Window::AnalyzerWizard(nullptr);
        form->ShowModal();
        String filename = form->analyzerInfoFile();
        form->Destroy();

        launchAnalyzer(filename);
    }
}

void ApplWnd::evtOnRunConstraintsBuilder(wxCommandEvent&)
{
    Dispatcher::GUI::Post(this, &ApplWnd::evtOnRunConstraintsBuilderDelayed);
}

void ApplWnd::evtOnRunConstraintsBuilderDelayed()
{
    if (not Data::Study::Current::Valid())
    {
        logs.error() << "No study opened";
        return;
    }
    auto& study = *Data::Study::Current::Get();

    if (SimulationCheck(study))
    {
        if (study.folder.empty() || study.folderInput.empty())
        {
            logs.error() << "The study must be saved before launching the constraints builder";
            return;
        }
        if (study.header.version != Data::versionLatest)
        {
            logs.error() << "The study must be upgraded to the v"
                         << Data::VersionToCStr((Data::Version)Data::versionLatest)
                         << " format before launching the constraints builder";
            return;
        }

        Forms::Disabler<ApplWnd> disabler(*this);
        size_t beforeRun = study.bindingConstraints.size();
        auto* form = new Window::ConstraintsBuilderWizard(nullptr);
        form->ShowModal();
        form->Destroy();

        if (beforeRun != study.bindingConstraints.size())
            MarkTheStudyAsModified();

        // TO DO : create job
        // String filename = "D:/settings.ini";
        // launchConstraintsBuilder(filename);
    }
}

} // namespace Forms
} // namespace Antares
