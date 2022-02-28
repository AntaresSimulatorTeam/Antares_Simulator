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
// NTC usage button
// =========================
class ntcUsageButton : public wxFrame, public Yuni::IEventObserver<ntcUsageButton>
{
public:
    ntcUsageButton(wxWindow* parent,
             Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)>& onPopup,
             wxFlexGridSizer* sizer_flex_grid);

    ~ntcUsageButton();

    void update(Data::AreaLink* link);
    bool isEmpty() { return !button_; }

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*);

    void onSelectUseNTC(wxCommandEvent&);
    void onSelectSetToNull(wxCommandEvent&);
    void onSelectSetToInfinite(wxCommandEvent&);

    void broadCastChange();
    void broadCastChangeOutside();

public:
    static Yuni::Event<void(Antares::Data::AreaLink*)> onTransmissionCapacitiesUsageChanges;
private:
    Component::Button* button_ = nullptr;
    Data::AreaLink* currentLink_ = nullptr;
};


// =========================
// Caption button
// =========================
class captionButton : public wxFrame, public Yuni::IEventObserver<ntcUsageButton>
{
public:
    captionButton(wxWindow* parent,
        Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)>& onPopup,
        wxFlexGridSizer* sizer_flex_grid);

    ~captionButton();

    void update(Data::AreaLink* link);
    bool isEmpty() { return !button_; }
    void setCaption(const wxString caption) { button_->caption(caption); }

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*);

    void onEditCaption(wxCommandEvent&);
    void onButtonEditCaption(void*);

    void broadCastChange();
    void broadCastChangeOutside();

public:
    static Yuni::Event<void(Antares::Data::AreaLink*)> onLinkCaptionChanges;
private:
    Component::Button* button_ = nullptr;
    Component::Button* alias_button_ = nullptr;
    wxBoxSizer* local_horizontal_sizer_ = nullptr;
    wxStaticText* caption_label_ = nullptr;
    wxWindow* caption_text_ = nullptr;
    wxFlexGridSizer* sizer_flex_grid_;
    Data::AreaLink* currentLink_ = nullptr;
};


// =========================
// Loop flow usage button
// =========================
class loopFlowUsageButton : public wxFrame, public Yuni::IEventObserver<ntcUsageButton>
{
public:
    loopFlowUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid);

    ~loopFlowUsageButton() = default;

    void update(Data::AreaLink* link);

private:
    Component::Button* button_ = nullptr;
};
}
}