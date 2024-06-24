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
#ifndef __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADVANCED_PARAMETERS_H__
#define __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADVANCED_PARAMETERS_H__

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

    void onNumericQuality(Component::Button&, wxMenu&, void*, Data::TimeSeriesType ts);
    void onSelectNumericQualityStandard(wxCommandEvent& evt);
    void onSelectNumericQualityHigh(wxCommandEvent& evt);
    
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

    void onSelectUCMode(Antares::Data::UnitCommitmentMode mode);
    void onSelectUCHeuristicFast(wxCommandEvent& evt);
    void onSelectUCHeuristicAccurate(wxCommandEvent& evt);
    void onSelectUCMILP(wxCommandEvent& evt);

    void onNumberOfCores(Component::Button&, wxMenu& menu, void*);
    template<Antares::Data::NumberOfCoresMode>
    void onSelectNumberOfCoresLevel(wxCommandEvent& evt);

private:
    void onSelectNumberOfCoresLevel(Data::NumberOfCoresMode ncMode);

public:
    void onRenewableGenerationModelling(Component::Button&, wxMenu& menu, void*);
    void onSelectRGMaggregated(wxCommandEvent& evt);
    void onSelectRGMrenewableClusters(wxCommandEvent& evt);

private:
    Component::Button* pBtnNumericQualityLoad;
    Component::Button* pBtnNumericQualityWind;
    Component::Button* pBtnNumericQualitySolar;
    Component::Button* pBtnPowerFluctuations;
    Component::Button* pBtnHydroHeuristicPolicy;
    Component::Button* pBtnHydroPricing;
    Component::Button* pBtnSheddingPolicy;
    Component::Button* pBtnUnitCommitment;
    Component::Button* pBtnRenewableGenModelling;
    Component::Button* pBtnNumberOfCores;
    wxTextCtrl* pEditSeeds[Data::seedMax];
    // Only used for menus
    Data::TimeSeriesType pCurrentTS;

}; // class AdvancedParameters

} // namespace Options
} // namespace Window
} // namespace Antares

#endif // __ANTARES_APPLICATION_WINDOWS_OPTIONS_ADVANCED_PARAMETERS_H__
