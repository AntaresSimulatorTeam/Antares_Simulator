// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include "../main.h"
#include "antares/study/study.h"
#include "../../windows/options/temp-folder/temp-folder.h"
#include "../../windows/options/geographic-trimming/geographic-trimming.h"
#include "../../windows/options/select-output/select-output.h"
#include "../../windows/options/playlist/playlist.h"
#include "../../windows/options/optimization/optimization.h"
#include "../../windows/options/adequacy-patch/adequacy-patch-options.h"
#include "../../windows/options/adequacy-patch/adequacy-patch-areas.h"
#include "../../windows/options/advanced/advanced.h"
#include "../../windows/message.h"

namespace Antares::Forms
{
void ApplWnd::evtOnOptionsTempFolder(wxCommandEvent&)
{
    Dispatcher::GUI::CreateAndShowModal<Window::Options::ConfigureTempFolder>(this);
}

void ApplWnd::evtOnOptionsSelectOutput(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::SelectOutput>(this);
    }
}

void ApplWnd::evtOnOptionsSelectAreasTrimming(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::areasTrimming>(this);
    }
}

void ApplWnd::evtOnOptionsSelectLinksTrimming(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::linksTrimming>(this);
    }
}

void ApplWnd::evtOnOptionsMCPlaylist(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::MCPlaylist>(this);
    }
}

void ApplWnd::evtOnOptionsDistricts(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        assert(pNotebook);
        pNotebook->select("sets");
    }
}

void ApplWnd::evtOnOptionsMCScenarioBuilder(wxCommandEvent&)
{
    auto study = GetCurrentStudy();
    if (!study)
    {
        return;
    }

    Forms::Disabler<ApplWnd> disabler(*this);

    // Checking the minimum requirements
    if (study->areas.empty())
    {
        // No area, it is meaningless to go further
        Window::Message message(this,
                                wxT("Configure MC Scenario Builder"),
                                wxEmptyString,
                                wxT("The study is empty. Please add one or more areas before using "
                                    "the MC Scenario builder"),
                                "images/misc/warning.png");
        message.add(Window::Message::btnCancel, true);
        message.showModal();
    }
    else
    {
        // Open the MC Scenario Builder
        pSectionNotebook->select(wxT("scenariobuilder"), true);
    }
}

void ApplWnd::evtOnOptionsOptimizationPrefs(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::Optimization>(this);
    }
}

void ApplWnd::evtOnOptionsAdvanced(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::AdvancedParameters>(this);
    }
}

void ApplWnd::evtOnOptionsAdequacyPatchOptions(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::AdequacyPatchOptions>(this);
    }
}

void ApplWnd::evtOnOptionsAdequacyPatchAreas(wxCommandEvent&)
{
    Forms::Disabler<ApplWnd> disabler(*this);
    if (CurrentStudyIsValid())
    {
        Dispatcher::GUI::CreateAndShowModal<Window::Options::AdequacyPatchAreas>(this);
    }
}

} // namespace Antares::Forms
