
#include <wx/textdlg.h>

#include "../application/menus.h"
#include "../application/study.h"
#include "../toolbox/create.h"
#include "link-property-buttons.h"
#include <antares/memory/new_check.hxx>

using namespace Antares::MemoryUtils;

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

    void menuLinkButton::bindButtonToPopupMenu()
    {
        getButton()->onPopupMenu(onPopup_);
    }

    void menuLinkButton::broadCastChange()
    {
        onSelectionChanges(getCurrentLink());
        broadCastChangeOutside();
    }

    void menuLinkButton::broadCastChangeOutside() const
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
        setButton(makeButton(parent));
        getButton()->menu(true);
        bindButtonToPopupMenu();

        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(getButton(), 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    Component::Button* ntcUsageButton::makeButton(wxWindow* parent)
    {
        return new_check_allocation<Component::Button>(parent, wxT("Transmission capacities"), "images/16x16/light_green.png");
    }

    void ntcUsageButton::update(Data::AreaLink* link)
    {
        setCurrentLink(link);

        switch (link->transmissionCapacities)
        {
        case Data::tncEnabled:
            getButton()->caption(wxT("Use transmission capacities"));
            getButton()->image("images/16x16/light_green.png");
            break;
        case Data::tncIgnore:
            getButton()->caption(wxT("Set transmission capacities to null"));
            getButton()->image("images/16x16/light_orange.png");
            break;
        case Data::tncInfinite:
            getButton()->caption(wxT("Set transmission capacities to infinite"));
            getButton()->image("images/16x16/infinity.png");
            break;
        default:
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
        if (!getCurrentLink())
            return;
        getCurrentLink()->transmissionCapacities = Data::tncEnabled;
        broadCastChange();
    }

    void ntcUsageButton::onSelectSetToNull(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;
        getCurrentLink()->transmissionCapacities = Data::tncIgnore;
        broadCastChange();
    }

    void ntcUsageButton::onSelectSetToInfinite(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;
        getCurrentLink()->transmissionCapacities = Data::tncInfinite;
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

        setButton(makeButton(parent));
        getButton()->menu(true);
        bindButtonToPopupMenu();

        sizer_flex_grid->Add(label, 0, wxRIGHT | wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        sizer_flex_grid->Add(getButton(), 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    Component::Button* hurdleCostsUsageButton::makeButton(wxWindow* parent)
    {
        return new_check_allocation<Component::Button>(parent, wxT("local values"), "images/16x16/light_green.png");
    }

    void hurdleCostsUsageButton::update(Data::AreaLink* link)
    {
        setCurrentLink(link);

        if (link->useHurdlesCost)
        {
            getButton()->caption(wxT("Use hurdles costs"));
            getButton()->image("images/16x16/light_green.png");
        }
        else
        {
            getButton()->caption(wxT("Ignore hurdles costs"));
            getButton()->image("images/16x16/light_orange.png");
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
        if (!getCurrentLink())
            return;

        getCurrentLink()->useHurdlesCost = true;
        broadCastChange();
    }

    void hurdleCostsUsageButton::onSelectIgnore(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;

        getCurrentLink()->useHurdlesCost = false;
        broadCastChange();
    }


    // =========================
    // Asset type button
    // =========================
    assetTypeButton::assetTypeButton(wxWindow* parent,
                                     wxFlexGridSizer* sizer_flex_grid)
        : menuLinkButton()
    {
        setButton(makeButton(parent));
        getButton()->menu(true);
        bindButtonToPopupMenu();

        sizer_flex_grid->AddSpacer(10);
        sizer_flex_grid->Add(getButton(), 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    Component::Button* assetTypeButton::makeButton(wxWindow* parent)
    {
        return new_check_allocation<Component::Button>(parent, wxT("Asset type"), "images/16x16/light_green.png");
    }

    void assetTypeButton::update(Data::AreaLink* link)
    {
        setCurrentLink(link);

        switch (link->assetType)
        {
        case Data::atAC:
            getButton()->caption(wxT("Asset type: AC"));
            getButton()->image("images/16x16/light_green.png");
            break;
        case Data::atDC:
            getButton()->caption(wxT("Asset type: DC"));
            getButton()->image("images/16x16/light_orange.png");
            break;
        case Data::atGas:
            getButton()->caption(wxT("Asset type: Gas"));
            getButton()->image("images/16x16/light_orange.png");
            break;
        case Data::atVirt:
            getButton()->caption(wxT("Asset type: Virtual"));
            getButton()->image("images/16x16/light_orange.png");
            break;
        case Data::atOther:
            getButton()->caption(wxT("Asset type: other"));
            getButton()->image("images/16x16/light_orange.png");
            break;
        default:
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
        if (!getCurrentLink())
            return;

        getCurrentLink()->assetType = Data::atAC;
        broadCastChange();

        getCurrentLink()->color[0] = 112;
        getCurrentLink()->color[1] = 112;
        getCurrentLink()->color[2] = 112;
        getCurrentLink()->style = Data::stPlain;
        getCurrentLink()->linkWidth = 1;
    }

    void assetTypeButton::onSelectDC(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;

        getCurrentLink()->assetType = Data::atDC;
        broadCastChange();

        getCurrentLink()->color[0] = 0;
        getCurrentLink()->color[1] = 255;
        getCurrentLink()->color[2] = 0;
        getCurrentLink()->style = Data::stDash;
        getCurrentLink()->linkWidth = 2;
    }

    void assetTypeButton::onSelectGas(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;

        getCurrentLink()->assetType = Data::atGas;
        broadCastChange();

        getCurrentLink()->color[0] = 0;
        getCurrentLink()->color[1] = 128;
        getCurrentLink()->color[2] = 255;
        getCurrentLink()->style = Data::stPlain;
        getCurrentLink()->linkWidth = 3;
    }

    void assetTypeButton::onSelectVirt(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;

        getCurrentLink()->assetType = Data::atVirt;
        broadCastChange();

        getCurrentLink()->color[0] = 255;
        getCurrentLink()->color[1] = 0;
        getCurrentLink()->color[2] = 128;
        getCurrentLink()->style = Data::stDotDash;
        getCurrentLink()->linkWidth = 2;
    }

    void assetTypeButton::onSelectOther(wxCommandEvent&)
    {
        if (!getCurrentLink())
            return;

        getCurrentLink()->assetType = Data::atOther;
        broadCastChange();

        getCurrentLink()->color[0] = 255;
        getCurrentLink()->color[1] = 128;
        getCurrentLink()->color[2] = 0;
        getCurrentLink()->style = Data::stDot;
        getCurrentLink()->linkWidth = 2;
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
        setButton(makeButton(parent));
        getButton()->menu(true);
        getButton()->bold(true);
        bindButtonToPopupMenu();
        sizer_flex_grid_->Add(getButton(), 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

        // Alias caption
        caption_label_ = Component::CreateLabel(parent, wxT("Caption"), false, true);
        alias_button_ = new_check_allocation<Component::Button>(parent, wxT(""), "images/16x16/document.png",
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

    Component::Button* captionButton::makeButton(wxWindow* parent)
    {
        return new_check_allocation<Component::Button>(parent, wxT("local values"), "images/16x16/link.png");
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
        if (not getCurrentLink())
            return;

        wxTextEntryDialog dialog(this,
            wxT("Please enter the new link's caption :"),
            wxT("Caption"),
            wxStringFromUTF8(getCurrentLink()->comments),
            wxOK | wxCANCEL);

        if (dialog.ShowModal() == wxID_OK)
        {
            String text;
            wxStringToString(dialog.GetValue(), text);
            text.trim();
            if (text != getCurrentLink()->comments)
            {
                getCurrentLink()->comments = text;
                broadCastChange();
            }
        }
    }

    void captionButton::update(Data::AreaLink* link)
    {
        setCurrentLink(link);

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
        setButton(makeButton(parent));
        sizer_flex_grid->Add(getButton(), 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    Component::Button* loopFlowUsageButton::makeButton(wxWindow* parent)
    {
        return new_check_allocation<Component::Button>(parent, wxT("loop flow"), "images/16x16/light_green.png");
    }

    void loopFlowUsageButton::update(Data::AreaLink* link)
    {
        if (link->useLoopFlow)
        {
            getButton()->caption(wxT("Account for loop flows"));
            getButton()->image("images/16x16/light_green.png");
        }
        else
        {
            getButton()->caption(wxT("Ignore loop flows"));
            getButton()->image("images/16x16/light_orange.png");
        }
    }


    // ============================
    // Phase shifter usage button
    // ============================
    phaseShifterUsageButton::phaseShifterUsageButton(wxWindow* parent, wxFlexGridSizer* sizer_flex_grid)
    {
        setButton(makeButton(parent));
        sizer_flex_grid->Add(getButton(), 0, wxLEFT | wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
    }

    Component::Button* phaseShifterUsageButton::makeButton(wxWindow* parent)
    {
        return new_check_allocation<Component::Button>(parent, wxT("phase shifter"), "images/16x16/light_green.png");
    }

    void phaseShifterUsageButton::update(Data::AreaLink* link)
    {
        if (link->usePST)
        {
            getButton()->caption(wxT("Tune PST"));
            getButton()->image("images/16x16/light_green.png");
        }
        else
        {
            getButton()->caption(wxT("Ignore PST "));
            getButton()->image("images/16x16/light_orange.png");
        }
    }

}   // End namespace Window 
}   // End namespace Antares 