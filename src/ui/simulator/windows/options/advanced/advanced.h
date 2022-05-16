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
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADVANCED_PARAMETERS_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADVANCED_PARAMETERS_H__

#include <antares/wx-wrapper.h>
#include "../../../toolbox/components/button.h"
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <antares/study/fwd.h>

namespace Antares
{
namespace Window
{
namespace Options
{
extern Yuni::Event<void(bool)> OnRenewableGenerationModellingChanged;

/*!
** \brief Startup Wizard User Interface
*/
class AdvancedParameters final : public wxDialog
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param parent The parent window
    */
    AdvancedParameters(wxWindow* parent);
    //! Destructor
    virtual ~AdvancedParameters();
    //@}

private:
    void refresh();
    void onClose(void*);
    void onResetToDefault(void*);

    void onInternalMotion(wxMouseEvent&);

    wxTextCtrl* insertEdit(wxWindow* parent,
                           wxSizer* sizer,
                           const wxString& text,
                           wxObjectEventFunction);

    void onEditSeedTSDraws(wxCommandEvent&);

    void onNumericQuality(Component::Button&, wxMenu&, void*, Data::TimeSeries ts);
    void onSelectNumericQualityStandard(wxCommandEvent& evt);
    void onSelectNumericQualityHigh(wxCommandEvent& evt);

    void onInitialReservoirLevels(Component::Button&, wxMenu&, void*);
    void onSelectHotStart(wxCommandEvent& evt);
    void onSelectColdStart(wxCommandEvent& evt);

    void onHydroHeuristicPolicy(Component::Button&, wxMenu& menu, void*);
    void onSelectAccomodateRuleCurves(wxCommandEvent& evt);
    void onSelectMaximizeGeneration(wxCommandEvent& evt);

    void onHydroPricingMode(Component::Button&, wxMenu& menu, void*);
    void onSelectHPHeuristic(wxCommandEvent& evt);
    void onSelectHPMixedIntegerLinearProblem(wxCommandEvent& evt);

    void onPowerFluctuations(Component::Button&, wxMenu&, void*);
    void onSelectMinimizeRamping(wxCommandEvent& evt);
    void onSelectMinimizeExcursions(wxCommandEvent& evt);
    void onSelectFreeModulations(wxCommandEvent& evt);

    void onSheddingPolicy(Component::Button&, wxMenu&, void*);
    void onSelectSHPShavePeaks(wxCommandEvent& evt);
    void onSelectSHPMinimizeDuration(wxCommandEvent& evt);

    void onUnitCommitmentMode(Component::Button&, wxMenu& menu, void*);
    void onSelectUCFast(wxCommandEvent& evt);
    void onSelectUCAccurate(wxCommandEvent& evt);
    void onSelectUCMILP(wxCommandEvent& evt);

    void onNumberOfCores(Component::Button&, wxMenu& menu, void*);
    void onSelectNCmin(wxCommandEvent& evt);
    void onSelectNClow(wxCommandEvent& evt);
    void onSelectNCaverage(wxCommandEvent& evt);
    void onSelectNChigh(wxCommandEvent& evt);
    void onSelectNCmax(wxCommandEvent& evt);

    void onRenewableGenerationModelling(Component::Button&, wxMenu& menu, void*);
    void onSelectRGMaggregated(wxCommandEvent& evt);
    void onSelectRGMrenewableClusters(wxCommandEvent& evt);

    void onDAReserveAllocationMode(Component::Button&, wxMenu& menu, void*);
    void onSelectDAGlobal(wxCommandEvent& evt);
    void onSelectDALocal(wxCommandEvent& evt);

private:
    Component::Button* pBtnNumericQualityLoad;
    Component::Button* pBtnNumericQualityWind;
    Component::Button* pBtnNumericQualitySolar;
    Component::Button* pBtnPowerFluctuations;
    Component::Button* pBtnInitialReservoirLevels;
    Component::Button* pBtnHydroHeuristicPolicy;
    Component::Button* pBtnHydroPricing;
    Component::Button* pBtnSheddingPolicy;
    Component::Button* pBtnMultiNodalMarginalPrices;
    Component::Button* pBtnUnitCommitment;
    Component::Button* pBtnRenewableGenModelling;
    Component::Button* pBtnNumberOfCores;
    Component::Button* pBtnDARreserveManagement;
    wxTextCtrl* pEditSeeds[Data::seedMax];
    // Only used for menus
    Data::TimeSeries pCurrentTS;

}; // class AdvancedParameters

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADVANCED_PARAMETERS_H__
