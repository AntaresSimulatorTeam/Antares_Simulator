
#include "../application/menus.h"
#include "../application/study.h"
#include "link-property-buttons.h"


namespace Antares
{
namespace Window
{

    Yuni::Event<void(Antares::Data::AreaLink*)> NTCusage::onTransmissionCapacitiesUsageChanges;
    
    NTCusage::NTCusage(wxWindow* parent,
                       Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)>& onPopup,
                       wxFlexGridSizer* sizer_flex_grid)
    {
        onTransmissionCapacitiesUsageChanges.connect(this, &NTCusage::update);
        button_ = new Component::Button(parent, wxT("Transmission capacities"), "images/16x16/light_green.png");
        button_->menu(true);
        onPopup.bind(this, &NTCusage::onPopupMenu);
        button_->onPopupMenu(onPopup);
        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void NTCusage::update(Data::AreaLink* link)
    {
        currentLink_ = link;

        switch (link->transmissionCapacities)
        {
        case Data::tncEnabled:
            button_->caption(wxT("Use transmission capacities"));
            button_->image("images/16x16/light_green.png");
            break;
        case Data::tncIgnore:
            button_->caption(wxT("Set transmission capacities to null"));
            button_->image("images/16x16/light_orange.png");
            break;
        case Data::tncInfinite:
            button_->caption(wxT("Set transmission capacities to infinite"));
            button_->image("images/16x16/infinity.png");
            break;
        }
    }

    bool NTCusage::isEmpty()
    {
        return !button_;
    }

    void NTCusage::onPopupMenu(Component::Button&, wxMenu& menu, void*)
    {
        wxMenuItem* it;

        it = Menu::CreateItem(&menu,
            wxID_ANY,
            wxT("Use transmission capacities"),
            "images/16x16/light_green.png",
            wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(NTCusage::onSelectUseNTC),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to null"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(NTCusage::onSelectSetToNull),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to infinite"), "images/16x16/infinity.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(NTCusage::onSelectSetToInfinite),
            nullptr,
            this);
    }

    void NTCusage::onSelectUseNTC(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncEnabled;
        broadCastChange();
    }

    void NTCusage::onSelectSetToNull(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncIgnore;
        broadCastChange();
    }

    void NTCusage::onSelectSetToInfinite(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncInfinite;
        broadCastChange();
    }

    void NTCusage::broadCastChange()
    {
        onTransmissionCapacitiesUsageChanges(currentLink_);
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
    }
}
}