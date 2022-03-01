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
    Component::Button* button_ = nullptr;
};


// ==================================
// Abstract modifiable link button
// ==================================
class modifiableLinkButton : public linkButton, public Yuni::IEventObserver<modifiableLinkButton>
{
public:
    static Yuni::Event<void(Antares::Data::AreaLink*)> onSelectionChanges;

public:
    modifiableLinkButton();
    ~modifiableLinkButton() = default;

    bool isEmpty() { return !button_; }

protected:
    virtual void onPopupMenu(Component::Button&, wxMenu& menu, void*) = 0;

protected:
    Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)> onPopup_;
    Data::AreaLink* currentLink_ = nullptr;
};


// =========================
// NTC usage button
// =========================
class ntcUsageButton : public modifiableLinkButton
{
public:
    ntcUsageButton(wxWindow* parent,
                   wxFlexGridSizer* sizer_flex_grid);

    ~ntcUsageButton();

    void update(Data::AreaLink* link) override;

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onSelectUseNTC(wxCommandEvent&);
    void onSelectSetToNull(wxCommandEvent&);
    void onSelectSetToInfinite(wxCommandEvent&);

    void broadCastChange();
    void broadCastChangeOutside();

};


// =========================
// Caption button
// =========================
class captionButton : public modifiableLinkButton
{
public:
    captionButton(wxWindow* parent,
                  wxFlexGridSizer* sizer_flex_grid);

    ~captionButton();

    void update(Data::AreaLink* link) override;
    void setCaption(const wxString caption) { button_->caption(caption); }

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*) override;

    void onEditCaption(wxCommandEvent&);
    void onButtonEditCaption(void*);

    void broadCastChange();
    void broadCastChangeOutside();

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
};
}
}