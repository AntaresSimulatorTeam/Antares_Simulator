/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <wx/frame.h>
#include <wx/sizer.h>
#include "yuni/core/event/interfaces.h"
#include "../toolbox/components/button.h"
#include "libs/antares/study/area/links.h"

namespace Antares
{
namespace Window
{
// =========================
// Abstract link button
// =========================
class linkButton : public wxFrame
{
public:
    virtual void update(Data::AreaLink* link) = 0;

protected:
    Component::Button* getButton() const
    {
        return button_;
    }
    void setButton(Component::Button* button)
    {
        button_ = button;
    }

private:
    Component::Button* button_ = nullptr;
};

// ==================================
// Abstract menu link button
// ==================================
class menuLinkButton : public linkButton, public Yuni::IEventObserver<menuLinkButton>
{
public:
    static Yuni::Event<void(Antares::Data::AreaLink*)> onSelectionChanges;

    menuLinkButton();
    ~menuLinkButton() override;

    bool hasNoButton() const
    {
        return !getButton();
    }

protected:
    Data::AreaLink* getCurrentLink() const
    {
        return currentLink_;
    }
    void setCurrentLink(Data::AreaLink* link)
    {
        currentLink_ = link;
    }

    virtual void onPopupMenu(Component::Button&, wxMenu& menu, void*) = 0;
    void bindButtonToPopupMenu() const;

    void broadCastChange() const;
    void broadCastChangeOutside() const;

private:
    Data::AreaLink* currentLink_ = nullptr;
    Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)> onPopup_;
};

// =========================
// NTC usage button
// =========================
class ntcUsageButton : public menuLinkButton
{
public:
    ntcUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    void update(Data::AreaLink* link) override;

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onSelectUseNTC(wxCommandEvent&);
    void onSelectSetToNull(wxCommandEvent&);
    void onSelectSetToInfinite(wxCommandEvent&);
};

// ============================
// Hurdle costs usage button
// ============================
class hurdleCostsUsageButton : public menuLinkButton
{
public:
    hurdleCostsUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    void update(Data::AreaLink* link) override;

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onSelectUse(wxCommandEvent&);
    void onSelectIgnore(wxCommandEvent&);
};

// =========================
// Asset type button
// =========================
class assetTypeButton : public menuLinkButton
{
public:
    assetTypeButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    void update(Data::AreaLink* link) override;

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onSelectAC(wxCommandEvent&);
    void onSelectDC(wxCommandEvent&);
    void onSelectGas(wxCommandEvent&);
    void onSelectVirt(wxCommandEvent&);
    void onSelectOther(wxCommandEvent&);
};

// =========================
// Caption button
// =========================
class captionButton : public menuLinkButton
{
public:
    captionButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    void update(Data::AreaLink* link) override;
    void setCaption(const wxString& caption) const
    {
        getButton()->caption(caption);
    }

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onEditCaption(wxCommandEvent&);
    void onButtonEditCaption(void*);

    // Private data members
    Component::Button* alias_button_ = nullptr;
    wxBoxSizer* local_horizontal_sizer_ = nullptr;
    wxStaticText* caption_label_ = nullptr;
    wxWindow* caption_text_ = nullptr;
    wxFlexGridSizer* sizer_flex_grid_;
};

// =========================
// Loop flow usage button
// =========================
class loopFlowUsageButton : public linkButton
{
public:
    loopFlowUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    void update(Data::AreaLink* link) override;
};

// ============================
// Phase shifter usage button
// ============================
class phaseShifterUsageButton : public linkButton
{
public:
    phaseShifterUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    void update(Data::AreaLink* link) override;
};

} // namespace Window
} // namespace Antares