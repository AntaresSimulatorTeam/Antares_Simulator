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
    virtual Component::Button* makeButton(wxWindow* parent) = 0;

    Component::Button* getButton() { return button_; }
    void setButton(Component::Button* button) { button_ = button; }

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

public:
    menuLinkButton();
    virtual ~menuLinkButton();

    bool hasNoButton() { return !getButton(); }

protected:
    Data::AreaLink* getCurrentLink() { return currentLink_; }
    void setCurrentLink(Data::AreaLink* link) { currentLink_ = link; }

    virtual void onPopupMenu(Component::Button&, wxMenu& menu, void*) = 0;
    void bindButtonToPopupMenu();

    void broadCastChange();
    void broadCastChangeOutside();

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
    ntcUsageButton(wxWindow* parent,
                   wxFlexGridSizer* sizer_flex_grid);

    ~ntcUsageButton() = default;

    void update(Data::AreaLink* link) override;

private:
    Component::Button* makeButton(wxWindow* parent) override;
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
    hurdleCostsUsageButton(wxWindow* parent,
        wxFlexGridSizer* sizer_flex_grid);

    ~hurdleCostsUsageButton() = default;

    void update(Data::AreaLink* link) override;

private:
    Component::Button* makeButton(wxWindow* parent) override;
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
    assetTypeButton(wxWindow* parent,
        wxFlexGridSizer* sizer_flex_grid);

    ~assetTypeButton() = default;

    void update(Data::AreaLink* link) override;

private:
    Component::Button* makeButton(wxWindow* parent) override;
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
    captionButton(wxWindow* parent,
                  wxFlexGridSizer* sizer_flex_grid);

    ~captionButton() = default;

    void update(Data::AreaLink* link) override;
    void setCaption(const wxString caption) { getButton()->caption(caption); }

private:
    Component::Button* makeButton(wxWindow* parent) override;
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onEditCaption(wxCommandEvent&);
    void onButtonEditCaption(void*);

private:
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

    ~loopFlowUsageButton() = default;

    void update(Data::AreaLink* link) override;

private:
    Component::Button* makeButton(wxWindow* parent) override;
};

// ============================
// Phase shifter usage button
// ============================
class phaseShifterUsageButton : public linkButton
{
public:
    phaseShifterUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    ~phaseShifterUsageButton() = default;

    void update(Data::AreaLink* link) override;
private:
    Component::Button* makeButton(wxWindow* parent) override;
};

}
}