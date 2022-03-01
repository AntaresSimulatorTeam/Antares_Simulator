
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
}
}