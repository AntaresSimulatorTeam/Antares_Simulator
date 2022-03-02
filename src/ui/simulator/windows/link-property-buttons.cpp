
#include <wx/textdlg.h>

#include "../application/menus.h"
#include "../application/study.h"
#include "../toolbox/create.h"
#include "link-property-buttons.h"


namespace Antares
{
namespace Window
{
    
    // ==================================
    // Abstract menu link button
    // ==================================

    Yuni::Event<void(Antares::Data::AreaLink*)> menuLinkButton::onSelectionChanges;

    menuLinkButton::menuLinkButton()
    {
        onSelectionChanges.connect(this, &menuLinkButton::update);
        onPopup_.bind(this, &menuLinkButton::onPopupMenu);
    }

    menuLinkButton::~menuLinkButton()
    {
        currentLink_ = nullptr;
        destroyBoundEvents();
    }

    void menuLinkButton::broadCastChange()
    {
        onSelectionChanges(currentLink_);
        broadCastChangeOutside();
    }

    void menuLinkButton::broadCastChangeOutside()
    {
        MarkTheStudyAsModified();
        OnInspectorRefresh(nullptr);
    }
    

    // =========================
    // NTC usage button
    // =========================    
    ntcUsageButton::ntcUsageButton(wxWindow* parent,
                       wxFlexGridSizer* sizer_flex_grid)
        : menuLinkButton()
    {
        button_ = new Component::Button(parent, wxT("Transmission capacities"), "images/16x16/light_green.png");
        button_->menu(true);
        button_->onPopupMenu(onPopup_);

        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void ntcUsageButton::update(Data::AreaLink* link)
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

    void ntcUsageButton::onPopupMenu(Component::Button&, wxMenu& menu, void*)
    {
        wxMenuItem* it;

        it = Menu::CreateItem(&menu,
            wxID_ANY,
            wxT("Use transmission capacities"),
            "images/16x16/light_green.png",
            wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ntcUsageButton::onSelectUseNTC),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to null"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ntcUsageButton::onSelectSetToNull),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to infinite"), "images/16x16/infinity.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ntcUsageButton::onSelectSetToInfinite),
            nullptr,
            this);
    }

    void ntcUsageButton::onSelectUseNTC(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncEnabled;
        broadCastChange();
    }

    void ntcUsageButton::onSelectSetToNull(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncIgnore;
        broadCastChange();
    }

    void ntcUsageButton::onSelectSetToInfinite(wxCommandEvent&)
    {
        if (!currentLink_)
            return;
        currentLink_->transmissionCapacities = Data::tncInfinite;
        broadCastChange();
    }


    // ============================
    // Hurdle costs usage button
    // ============================
    hurdleCostsUsageButton::hurdleCostsUsageButton(wxWindow* parent,
                                                   wxFlexGridSizer* sizer_flex_grid)
        : menuLinkButton()
    {
        wxStaticText* label = Component::CreateLabel(parent, wxT("Local values"), false, true);

        button_ = new Component::Button(parent, wxT("local values"), "images/16x16/light_green.png");
        button_->menu(true);
        button_->onPopupMenu(onPopup_);

        sizer_flex_grid->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void hurdleCostsUsageButton::update(Data::AreaLink* link)
    {
        currentLink_ = link;

        if (link->useHurdlesCost)
        {
            button_->caption(wxT("Use hurdles costs"));
            button_->image("images/16x16/light_green.png");
        }
        else
        {
            button_->caption(wxT("Ignore hurdles costs"));
            button_->image("images/16x16/light_orange.png");
        }
    }

    void hurdleCostsUsageButton::onPopupMenu(Component::Button&, wxMenu& menu, void*)
    {
        wxMenuItem* it;

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Use hurdles costs"), "images/16x16/light_green.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(hurdleCostsUsageButton::onSelectUse),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Ignore"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(hurdleCostsUsageButton::onSelectIgnore),
            nullptr,
            this);
    }

    void hurdleCostsUsageButton::onSelectUse(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->useHurdlesCost = true;
        broadCastChange();
    }

    void hurdleCostsUsageButton::onSelectIgnore(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->useHurdlesCost = false;
        broadCastChange();
    }


    // =========================
    // Asset type button
    // =========================
    assetTypeButton::assetTypeButton(wxWindow* parent,
                                     wxFlexGridSizer* sizer_flex_grid)
        : menuLinkButton()
    {
        button_ = new Component::Button(parent, wxT("Asset type"), "images/16x16/light_green.png");
        button_->menu(true);
        button_->onPopupMenu(onPopup_);

        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void assetTypeButton::update(Data::AreaLink* link)
    {
        currentLink_ = link;

        switch (link->assetType)
        {
        case Data::atAC:
            button_->caption(wxT("Asset type: AC"));
            button_->image("images/16x16/light_green.png");
            break;
        case Data::atDC:
            button_->caption(wxT("Asset type: DC"));
            button_->image("images/16x16/light_orange.png");
            break;
        case Data::atGas:
            button_->caption(wxT("Asset type: Gas"));
            button_->image("images/16x16/light_orange.png");
            break;
        case Data::atVirt:
            button_->caption(wxT("Asset type: Virtual"));
            button_->image("images/16x16/light_orange.png");
            break;
        case Data::atOther:
            button_->caption(wxT("Asset type: other"));
            button_->image("images/16x16/light_orange.png");
            break;
        }
    }

    void assetTypeButton::onPopupMenu(Component::Button&, wxMenu& menu, void*)
    {
        wxMenuItem* it;

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to AC"), "images/16x16/light_green.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(assetTypeButton::onSelectAC),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to DC"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(assetTypeButton::onSelectDC),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to Gas"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(assetTypeButton::onSelectGas),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to Virt"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(assetTypeButton::onSelectVirt),
            nullptr,
            this);

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Set to other"), "images/16x16/light_orange.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(assetTypeButton::onSelectOther),
            nullptr,
            this);
    }

    void assetTypeButton::onSelectAC(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->assetType = Data::atAC;
        broadCastChange();

        currentLink_->color[0] = 112;
        currentLink_->color[1] = 112;
        currentLink_->color[2] = 112;
        currentLink_->style = Data::stPlain;
        currentLink_->linkWidth = 1;
    }

    void assetTypeButton::onSelectDC(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->assetType = Data::atDC;
        broadCastChange();

        currentLink_->color[0] = 0;
        currentLink_->color[1] = 255;
        currentLink_->color[2] = 0;
        currentLink_->style = Data::stDash;
        currentLink_->linkWidth = 2;
    }

    void assetTypeButton::onSelectGas(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->assetType = Data::atGas;
        broadCastChange();

        currentLink_->color[0] = 0;
        currentLink_->color[1] = 128;
        currentLink_->color[2] = 255;
        currentLink_->style = Data::stPlain;
        currentLink_->linkWidth = 3;
    }

    void assetTypeButton::onSelectVirt(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->assetType = Data::atVirt;
        broadCastChange();

        currentLink_->color[0] = 255;
        currentLink_->color[1] = 0;
        currentLink_->color[2] = 128;
        currentLink_->style = Data::stDotDash;
        currentLink_->linkWidth = 2;
    }

    void assetTypeButton::onSelectOther(wxCommandEvent&)
    {
        if (!currentLink_)
            return;

        currentLink_->assetType = Data::atOther;
        broadCastChange();

        currentLink_->color[0] = 255;
        currentLink_->color[1] = 128;
        currentLink_->color[2] = 0;
        currentLink_->style = Data::stDot;
        currentLink_->linkWidth = 2;
    }



    // =========================
    // Caption button
    // =========================
    using namespace Yuni;

    captionButton::captionButton(wxWindow* parent,                           
        wxFlexGridSizer* sizer_flex_grid)
        : menuLinkButton(), sizer_flex_grid_(sizer_flex_grid)
    {
        // Link caption
        button_ = new Component::Button(parent, wxT("local values"), "images/16x16/link.png");
        button_->menu(true);
        button_->bold(true);
        button_->onPopupMenu(onPopup_);
        sizer_flex_grid_->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

        // Alias caption
        caption_label_ = Component::CreateLabel(parent, wxT("Caption"), false, true);
        alias_button_ = new Component::Button(parent, wxT(""), "images/16x16/document.png",
                                              this,
                                              &captionButton::onButtonEditCaption);
        local_horizontal_sizer_ = new wxBoxSizer(wxHORIZONTAL);
        caption_text_ = Component::CreateLabel(parent, wxEmptyString);
        sizer_flex_grid_->Add(caption_label_, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        local_horizontal_sizer_->Add(alias_button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        local_horizontal_sizer_->AddSpacer(2);
        local_horizontal_sizer_->Add(caption_text_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
        sizer_flex_grid_->Add(local_horizontal_sizer_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void captionButton::onPopupMenu(Component::Button&, wxMenu& menu, void*)
    {
        wxMenuItem* it;

        it = Menu::CreateItem(
            &menu, wxID_ANY, wxT("Edit caption"), "images/16x16/document.png", wxEmptyString);
        menu.Connect(it->GetId(),
            wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(captionButton::onEditCaption),
            nullptr,
            this);
    }

    void captionButton::onEditCaption(wxCommandEvent&)
    {
        onButtonEditCaption(nullptr);
    }

    void captionButton::onButtonEditCaption(void*)
    {
        if (not currentLink_)
            return;

        wxTextEntryDialog dialog(this,
            wxT("Please enter the new link's caption :"),
            wxT("Caption"),
            wxStringFromUTF8(currentLink_->comments),
            wxOK | wxCANCEL);

        if (dialog.ShowModal() == wxID_OK)
        {
            String text;
            wxStringToString(dialog.GetValue(), text);
            text.trim();
            if (text != currentLink_->comments)
            {
                currentLink_->comments = text;
                broadCastChange();
            }
        }
    }

    void captionButton::update(Data::AreaLink* link)
    {
        currentLink_ = link;

        if (link->comments.empty())
        {
            sizer_flex_grid_->Hide(caption_label_);
            sizer_flex_grid_->Hide(local_horizontal_sizer_);
        }
        else
        {
            caption_text_->SetLabel(wxStringFromUTF8(link->comments));
            sizer_flex_grid_->Show(caption_label_);
            sizer_flex_grid_->Show(local_horizontal_sizer_);
        }
    }


    // =========================
    // Loop flow usage button
    // =========================
    loopFlowUsageButton::loopFlowUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid)
    {
        button_ = new Component::Button(parent, wxT("loop flow"), "images/16x16/light_green.png");
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void loopFlowUsageButton::update(Data::AreaLink* link)
    {
        if (link->useLoopFlow)
        {
            button_->caption(wxT("Account for loop flows"));
            button_->image("images/16x16/light_green.png");
        }
        else
        {
            button_->caption(wxT("Ignore loop flows"));
            button_->image("images/16x16/light_orange.png");
        }
    }


    // ============================
    // Phase shifter usage button
    // ============================
    phaseShifterUsageButton::phaseShifterUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid)
    {
        button_ = new Component::Button(parent, wxT("phase shifter"), "images/16x16/light_green.png");
        sizer_flex_grid->Add(button_, 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    void phaseShifterUsageButton::update(Data::AreaLink* link)
    {
        if (link->usePST)
        {
            button_->caption(wxT("Tune PST"));
            button_->image("images/16x16/light_green.png");
        }
        else
        {
            button_->caption(wxT("Ignore PST "));
            button_->image("images/16x16/light_orange.png");
        }
    }

}   // End namespace Window 
}   // End namespace Antares 