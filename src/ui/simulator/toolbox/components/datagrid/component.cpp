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

#include <antares/wx-wrapper.h>
#include <antares/array/matrix.h>
#include <yuni/io/file.h>
#ifndef YUNI_OS_WINDOWS
#include <yuni/core/system/environment.h>
#endif

#include "component.h"
#include "wxgrid-renderer.h"
#include "renderer.h"

#include <wx/sizer.h>
#include <wx/dcclient.h>
#include <wx/statline.h>
#include <wx/choice.h>
#include <wx/dirdlg.h>
#include <wx/busyinfo.h>
#include <wx/filedlg.h>
#include <wx/busyinfo.h>
#include <wx/app.h>
#include <wx/msgdlg.h>
#include <wx/wupdlock.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>

#include "gridhelper.h"
#include "../../../application/main.h"
#include "../../../application/menus.h"
#include "../../../application/study.h"
#include "../../../application/wait.h"
#include "../../../toolbox/components/captionpanel.h"
#include "../../../toolbox/components/button.h"
#include "../../../toolbox/clipboard/clipboard.h"
#include "../../../windows/calendar/calendar.h"
#include "../../resources.h"
#include "../../create.h"
#include "../../../toolbox/validator.h"
#include "../../components/datagrid/filter/component.h"
#include "dbgrid.h"
#include <ui/common/lock.h>
#include <ui/common/component/panel/group.h>

#include "wxgrid-renderer.h"
#include "modifiers.hxx"

using namespace Yuni;

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace // anonymous
{
class PopupMenuModifierSet final : public wxObject
{
public:
    PopupMenuModifierSet(ModifierSet set) : modifierSet(set)
    {
    }
    virtual ~PopupMenuModifierSet()
    {
    }

    ModifierSet modifierSet;
};

class PopupMenuModifierOperator final : public wxObject
{
public:
    PopupMenuModifierOperator(uint op) : modifierOperator(op)
    {
    }
    virtual ~PopupMenuModifierOperator()
    {
    }

    uint modifierOperator;
};

class CreateOptions final
{
public:
    bool hasFilter;
    bool copypasteOnly;
    bool readonly;
    bool colorMappingRowLabels;
    bool hasLayerFilter;
};

} // anonymous namespace

class InternalState final : public Yuni::IEventObserver<InternalState>
{
public:
    //! Default constructor
    InternalState();
    //! Destructor
    virtual ~InternalState();

    /*!
    ** \brief Recreate from scratch the filter
    **
    ** The current design flaw of the filter component is to not be
    ** able to dynamicly update its precision when its changes from the
    ** renderer. Consequently we currently have to recreate it from
    ** scratch when it does.
    */
    void recreateFilter();

    /*!
    ** \brief Update controls for modifying the inner grid values
    */
    void updaterModifiersControls();

    /*!
    ** \brief Update visual component according the current grid
    */
    void rendererGridUpdate();

    /*!
    ** \brief Force the refresh of all components
    */
    void forceRefresh();

    void applyLayerFiltering();

    void createAllInternalControls(const CreateOptions& options);

    void onPickDate(Button&, wxMenu& menu, void*);

    /*!
    ** \brief set the value of the search input
    **
    ** The component will be updated with the value of the text.
    */
    void onMapLayerChanged(const wxString* text);
    //@}

    void onMapLayerAdded(const wxString* text);

    void onMapLayerRemoved(const wxString* text);

    void onMapLayerRenamed(const wxString* text);

public:
    //! The original component
    Component* component;

    //! Datagrid caption
    wxString caption;

    //! Flag to mark the study as modified if cells are updated
    bool shouldMarkStudyModifiedWhenModifyingCell;

    //! The main sizer with all components that the user expects to see
    wxSizer* sizerForAllComponents;
    wxSizer* toolbarSizer;
    wxSizer* toolbarSizerValues;

    wxComboBox* pLayerFilter;
    std::vector<int> layerFilteredIndices;
    //! The attached renderer
    Renderer::IRenderer* renderer;
    //! Precision
    Date::Precision precision;

    struct
    {
        //! Label "No Data", when the datasource is not ready or empty
        wxStaticText* textLabel;
        //! Associated Sizer
        wxSizer* sizer;
    } nodata;

    struct
    {
        //! Filter
        Toolbox::Filter::Component* component;
        //! Sizer of the filter
        wxSizer* sizer;

    } filter;

    struct
    {
        // Current selected set of actions
        ModifierSet selectedSet;
        //! Specific Action from the current selected modifier set of actions
        /*enum*/ uint selectedAction;
        struct
        {
            //! Text input value
            wxTextCtrl* textValue;
            //! Button for selecting a specific data
            Button* btnSelectCalendar;
            //! Button for selecting a set of actions
            Button* btnSetSelector;
            //! Button for selecting a specific action
            Button* btnActionSelector;
            //! Button for applying the changes
            Button* btnApply;
            //! Global sizer for modifiers
            wxSizer* sizer;
            //! Sizer for input
            wxSizer* sizerForInput;
        } ui;

    } modifier;
    //@}

    //! The real datagrid component
    DBGrid* grid;

    //! Custom implementation of wxGridTable
    VGridHelper* gridHelper;
    //! Cell renderer
    AntaresWxGridRenderer* cellRenderer;

    //! Counter used for beginUpdate / endUpdate
    uint updateCount;

}; // class InternalState

InternalState::InternalState() :
 shouldMarkStudyModifiedWhenModifyingCell(true),
 precision(Date::stepAny),
 grid(nullptr),
 gridHelper(nullptr),
 cellRenderer(nullptr),
 updateCount(0),
 pLayerFilter(nullptr)
{
    filter.component = nullptr;
    filter.sizer = nullptr;

    layerFilteredIndices.clear();

    OnMapLayerChanged.connect(this, &InternalState::onMapLayerChanged);
    OnMapLayerAdded.connect(this, &InternalState::onMapLayerAdded);
    OnMapLayerRemoved.connect(this, &InternalState::onMapLayerRemoved);
    OnMapLayerRenamed.connect(this, &InternalState::onMapLayerRenamed);
}

InternalState::~InternalState()
{
    // Destroy all bound events as soon as possible to avoid stupid
    // race conditions
    destroyBoundEvents();

    if (grid)
        grid->DisableCellEditControl();

    // Cleaning the filter
    if (filter.component)
    {
        filter.component->grid(nullptr);
        filter.component->clear();
    }

    if (gridHelper)
    {
        if (grid)
            grid->SetTable(nullptr, false);
        delete gridHelper;
        gridHelper = nullptr;
    }

    grid = nullptr;

    if (renderer)
    {
        renderer->onRefresh.clear();
        delete renderer;
        renderer = nullptr;
    }
}

void InternalState::forceRefresh()
{
    if (grid and renderer and renderer->valid() and grid->canRefresh() and 0 == updateCount)
    {
        GUILocker locker;
        WIP::Locker wip;

        layerFilteredIndices.clear();

        rendererGridUpdate();

        // Make sure the filter is disabled
        if (filter.component)
        {
            if (filter.component->precision() != renderer->precision())
                recreateFilter();
            filter.component->gridHelper(nullptr);
        }

        // Starting a batch
        grid->BeginBatch();

        // Recreating the table
        // To avoid any issue, the var gridHelper will be destroyed
        // after the grid has its new table
        auto* tmpHelper = new VGridHelper(renderer, shouldMarkStudyModifiedWhenModifyingCell);
        tmpHelper->precision(precision);
        grid->SetTable(tmpHelper, false);
        if (filter.component)
        {
            filter.component->gridHelper(tmpHelper);
            filter.component->dataGridPrecision(precision);
        }

        delete gridHelper;
        gridHelper = tmpHelper;

        // Updating the filer
        if (filter.component)
            filter.component->updateSearchResults();

        // applyLayerFiltering();

        // End of the batch
        component->internalShowDatagrid(true);

        auto* g = dynamic_cast<DBGrid*>(grid);
        if (g)
        {
            g->ensureDataAreLoaded();
            g->resizeAllHeaders(true);
        }
        grid->EndBatch();
        return;
    }

    // Nothing - No Data
    assert(nodata.textLabel and "invalid nodata.textLabel");
    nodata.textLabel->SetLabel(wxT("No data available"));
    component->internalShowDatagrid(false);
}

void InternalState::applyLayerFiltering()
{
    // applying layer filtering
    if (pLayerFilter)
    {
        auto text = pLayerFilter->GetValue();
        auto study = Data::Study::Current::Get();
        auto layerListEnd = study->layers.end();
        auto layerIt = study->layers.begin();
        layerIt++; // ignore the first layer (All)
        for (; layerIt != layerListEnd; layerIt++)
        {
            if (layerIt->second == text)
            {
                renderer->applyLayerFiltering(layerIt->first, gridHelper);

                break;
            }
        }

        // Invalidating the cache of the wxGrid
        /*wxGridTableBase* tbl = grid->GetTable();
        if (tbl)
                grid->SetTable(tbl, false);*/
    }
}

void InternalState::recreateFilter()
{
    if (!filter.component) // no filter !
        return;

    if (filter.sizer)
    {
        precision = (renderer) ? renderer->precision() : Date::stepNone;

        filter.sizer->Detach(filter.component);
        if (filter.component)
            filter.component->Destroy();
        filter.component = new Toolbox::Filter::Component(component, precision);
        filter.component->onUpdateSearchResults.connect(this, &InternalState::applyLayerFiltering);
        filter.component->precision(precision);
        filter.sizer->Add(filter.component, 1, wxALL | wxEXPAND);
        filter.component->grid(grid);
    }
    else
    {
#ifndef NDEBUG
        logs.error() << "component: datagrid: invalid sizer when updating the filter";
#endif
        filter.component->precision(precision);
    }
    filter.component->clear();
    filter.component->add();
}

void InternalState::updaterModifiersControls()
{
    auto* name = ModifierOperators::Name(modifier.selectedSet);
    auto* apply = ModifierOperators::ApplyName(modifier.selectedSet, modifier.selectedAction);
    auto* opname
      = ModifierOperators::OperatorToCString(modifier.selectedSet, modifier.selectedAction);
    auto input
      = ModifierOperators::OperatorInputType(modifier.selectedSet, modifier.selectedAction);

    // captions
    modifier.ui.btnSetSelector->caption(name);
    modifier.ui.btnApply->caption(apply);
    modifier.ui.btnActionSelector->caption(opname);

    // layer
    modifier.ui.sizerForInput->Hide((wxWindow*)modifier.ui.textValue);
    modifier.ui.sizerForInput->Hide(modifier.ui.btnSelectCalendar);

    switch (input)
    {
    case opInputText:
    {
        modifier.ui.sizerForInput->Show(modifier.ui.textValue);
        break;
    }
    case opInputDate:
    {
        // we should reset internal values to prevent any misuse
        // TODO we should parse textValue instead and update the button
        // to indicate the good previous date
        modifier.ui.textValue->SetValue(wxT("0"));
        modifier.ui.btnSelectCalendar->caption(wxT("(date not set)"));

        modifier.ui.sizerForInput->Show(modifier.ui.btnSelectCalendar);
        break;
    }
    case opInputVoid:
        break;
    }

    // rebuild the layout
    modifier.ui.sizer->Layout();

    auto* frame = wxFindFrameParent(component);
    Dispatcher::GUI::Refresh(frame);
}

void InternalState::rendererGridUpdate()
{
    wxSizer* sizer = component->GetSizer();
    if (!sizer)
        return;

    // Resize the matrix
    if (renderer and renderer->valid() and !updateCount)
    {
        uint w = renderer->maxWidthResize();
        if (w != 0)
        {
            // Caption
            toolbarSizer->Layout();
            toolbarSizerValues->Layout();
            sizer->Layout();
            return;
        }
    }
    // Hide all controls associated with that
    toolbarSizer->Layout();
    toolbarSizerValues->Layout();
    sizer->Layout();
}

void InternalState::createAllInternalControls(const CreateOptions& flags)
{
    // The default sizer
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizerForAllComponents = sizer;

    // No Data
    {
        nodata.sizer = new wxBoxSizer(wxHORIZONTAL);
        nodata.sizer->AddStretchSpacer();
        auto* stT = new wxStaticText(component, wxID_ANY, wxT("No data available"));
        nodata.textLabel = stT;
        nodata.sizer->Add(stT, 0, wxALL | wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL);
        nodata.sizer->AddStretchSpacer();
    }

    // Adding the caption, if any
    if (not caption.empty())
        sizer->Add(new CaptionPanel(component, caption), 0, wxALL | wxEXPAND);

    if (renderer)
        precision = renderer->precision();

    // Panel
    component->createModifyPanel(sizer, flags.copypasteOnly, flags.readonly);

    // Layer Filter
    if (flags.hasLayerFilter)
    {
        pLayerFilter = new wxComboBox(component,
                                      wxID_ANY,
                                      wxEmptyString,
                                      wxDefaultPosition,
                                      wxSize(-1, 22),
                                      0,
                                      NULL,
                                      wxCB_READONLY);
        pLayerFilter->SetFont(wxFont(wxFontInfo().Bold()));
        pLayerFilter->AppendString("All");
        pLayerFilter->SetValue("All");
        pLayerFilter->Connect(pLayerFilter->GetId(),
                              wxEVT_COMBOBOX,
                              wxCommandEventHandler(Component::onComboUpdated),
                              nullptr,
                              component);

        auto* ssz = new wxBoxSizer(wxHORIZONTAL);
        ssz->AddSpacer(15);
        ssz->Add(
          CreateLabel(component, wxT("Layer"), false, true), 0, wxALIGN_CENTER_VERTICAL | wxALL);
        ssz->Add(3, 3);
        ssz->AddSpacer(26);
        ssz->Add(pLayerFilter, 0, wxALL | wxEXPAND);
        sizer->Add(ssz, 0, wxALL | wxEXPAND);
        sizer->AddSpacer(2);
    }

    // Filters
    if (flags.hasFilter)
    {
        auto* ssz = new wxBoxSizer(wxHORIZONTAL);
        ssz->AddSpacer(15);
        ssz->Add(
          CreateLabel(component, wxT("Filter"), false, true), 0, wxALIGN_CENTER_VERTICAL | wxALL);
        ssz->Add(3, 3);
        filter.component = new Toolbox::Filter::Component(component, precision);
        filter.component->onUpdateSearchResults.connect(this, &InternalState::applyLayerFiltering);
        filter.component->precision(precision);
        ssz->Add(filter.component, 1, wxALL | wxEXPAND);
        filter.sizer = ssz;

        sizer->Add(ssz, 0, wxALL | wxEXPAND);
    }

    // Values
    component->createModifyPanelValues(sizer, flags.copypasteOnly);

    // Grid
    grid = new DBGrid(component);
    if (not flags.colorMappingRowLabels)
        grid->disableColorMappingForRowLabels();

    // internal.grid->SetDefaultRenderer(new wxGridCellFloatRenderer(-1, 3));
    cellRenderer = new AntaresWxGridRenderer();
    cellRenderer->renderer = renderer;
    grid->SetDefaultRenderer(cellRenderer);
    {
        auto* hz = new wxBoxSizer(wxHORIZONTAL);
        hz->AddSpacer(10);
        hz->Add(grid, 1, wxALL | wxEXPAND);
        sizer->Add(hz, 1, wxALL | wxEXPAND);
    }

    sizer->Layout();
    mainSizer->Add(sizer, 1, wxALL | wxEXPAND);
    mainSizer->Add(nodata.sizer, 1, wxALL | wxEXPAND);
    component->SetSizer(mainSizer);

    // Attaching the grid to the filter
    if (flags.hasFilter)
        filter.component->grid(grid);

    if (renderer)
        renderer->onRefresh.connect(component, &Component::forceRefresh);

    // Rebuild the virtual table
    component->internalShowDatagrid(false);
}

void InternalState::onPickDate(Button&, wxMenu&, void*)
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return;

    auto* frame = wxFindFrameParent(component);
    Window::CalendarSelect calendarWindow(frame);

    calendarWindow.allowQuickSelect = true;
    calendarWindow.allowRangeSelection = false;
    calendarWindow.requireWholeWeekSelection = false;

    calendarWindow.ShowModal();

    if (calendarWindow.modified())
    {
        if (calendarWindow.selectionDayRange[0] < 365)
        {
            auto& calendar = study->calendar;
            auto& dayinfo = calendar.days[calendarWindow.selectionDayRange[0]];

            wxString text;
            text << (1 + dayinfo.dayMonth) << wxT(", ");
            text << wxStringFromUTF8(calendar.text.months[dayinfo.month].shortUpperName);
            modifier.ui.btnSelectCalendar->caption(text);

            text.clear();
            text << (1 + calendar.months[dayinfo.month].realmonth) << wxT('/');
            text << (dayinfo.dayMonth + 1);
            modifier.ui.textValue->SetValue(text);
        }
    }
}

void InternalState::onMapLayerAdded(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter && pLayerFilter->FindString(*text) == wxNOT_FOUND)
    {
        pLayerFilter->AppendString(*text);
        auto hzCombo = pLayerFilter->GetContainingSizer();
        if (pLayerFilter->GetCount() > 1 && !sizerForAllComponents->IsShown(hzCombo))
        {
            sizerForAllComponents->Show(hzCombo);
        }
    }
    // wxStringToString(*text, pLastResearch);
    // Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void InternalState::onMapLayerRemoved(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
    {
        auto hzCombo = pLayerFilter->GetContainingSizer();
        auto pos = pLayerFilter->FindString(*text);
        if (pos != wxNOT_FOUND)
        {
            pLayerFilter->Delete(pos);
            pLayerFilter->Select(0);
        }
        /*if (pLayerFilter->GetCount() == 1 && sizerForAllComponents->IsShown(hzCombo))
                sizerForAllComponents->Hide(hzCombo);*/
    }
    /*wxStringToString(*text, pLastResearch);
    Dispatcher::GUI::Post(this, &Spotlight::redoResearch);*/
}

void InternalState::onMapLayerChanged(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
        pLayerFilter->SetValue(*text);

    // wxStringToString(*text, pLastResearch);
    // Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

void InternalState::onMapLayerRenamed(const wxString* text)
{
    // Note: the method ChangeValue does not generate a wexEXT_COMMAND_TEXT_UPDATED
    // event
    if (pLayerFilter)
        pLayerFilter->SetString(pLayerFilter->GetSelection(), *text);

    // wxStringToString(*text, pLastResearch);
    // Dispatcher::GUI::Post(this, &Spotlight::redoResearch);
}

Component::Component(wxWindow* parent,
                     Renderer::IRenderer* renderer,
                     const wxString& caption,
                     bool hasFilter,
                     bool colorMappingRowLabels,
                     bool copypasteOnly,
                     bool readonly,
                     bool hasLayerFilter) :

 Panel(parent)
{
    pInternal = new InternalState();
    auto& internal = *pInternal;
    internal.component = this;
    internal.caption = caption;
    internal.renderer = renderer;

    // The creation will be delayed to considerably speedup the application startup
    // temporary disabled due to Segv :)
    CreateOptions options;
    options.hasFilter = hasFilter;
    options.copypasteOnly = copypasteOnly;
    options.readonly = readonly;
    options.colorMappingRowLabels = colorMappingRowLabels;
    options.hasLayerFilter = hasLayerFilter;

    // Bind<void ()> delayedCreation;
    // delayedCreation.bind(pInternal, &InternalState::createAllInternalControls, options);
    // Dispatcher::GUI::Post(delayedCreation);
    internal.createAllInternalControls(options);

    // Events
    OnStudySettingsChanged.connect(pInternal, &InternalState::rendererGridUpdate);
    OnStudyClosed.connect(this, &Component::onStudyClosed);
    OnStudyBeginUpdate.connect(this, &Component::onBeginUpdate);
    OnStudyEndUpdate.connect(this, &Component::onEndUpdate);
}

Component::~Component()
{
    // Destroy all bound events as soon as possible to avoid stupid
    // race conditions
    destroyBoundEvents();

    // destroying all internal ressources
    delete pInternal;

    // To avoid corrupt vtable in some rare cases / misuses
    // (when children try to access to this object for example),
    // we should destroy all children as soon as possible.
    auto* sizer = GetSizer();
    if (sizer)
        sizer->Clear(true);
}

void Component::internalShowDatagrid(bool v)
{
    auto& internal = *pInternal;
    internal.rendererGridUpdate();
    auto* s = GetSizer();
    if (s)
    {
        s->Show(internal.nodata.sizer, (not v), true);
        s->Show(internal.sizerForAllComponents, v, true);
        s->Layout();
    }
}

void Component::onBeginUpdate()
{
    auto& internal = *pInternal;
    if (!internal.updateCount)
    {
        assert(internal.nodata.textLabel);
        internal.nodata.textLabel->SetLabel(wxT("Waiting for data..."));
        internalShowDatagrid(false);
    }
    ++internal.updateCount;
    if (internal.grid)
        internal.grid->enableRefresh(false);
}

void Component::onEndUpdate()
{
    auto& internal = *pInternal;
    assert(internal.updateCount > 0);
    if (!internal.updateCount)
    {
        logs.error() << "internal error: invalid reference count";
        return;
    }
    if (!(--internal.updateCount))
    {
        assert(internal.nodata.textLabel);
        internal.nodata.textLabel->SetLabel(wxT("No data available"));
        if (internal.gridHelper)
            internal.gridHelper->valid = false;
        if (internal.renderer)
            internal.renderer->invalidate = true;
        if (internal.grid)
            internal.grid->enableRefresh(true);
        forceRefreshDelayed();
    }
}

void Component::createModifyPanel(wxSizer* sizer, bool copypasteOnly, bool readonly)
{
    // internal variable for GUI components and other stuff
    auto& internal = *pInternal;

    // Modify
    auto* ssz = new wxBoxSizer(wxHORIZONTAL);
    internal.toolbarSizer = ssz;
    ssz->AddSpacer(10);

    Antares::Component::Button* btn;

    {
        auto* grp = new Antares::Component::PanelGroup(this);
        ssz->Add(grp, 0, wxALL | wxEXPAND);
        auto* hz = grp->subpanel->GetSizer();

        if (not readonly)
        {
            btn = new Antares::Component::Button(grp->subpanel,
                                                 wxEmptyString,
                                                 "images/16x16/open.png",
                                                 this,
                                                 &Component::onLoadMatrix);
            hz->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);
        }

        btn = new Antares::Component::Button(grp->subpanel,
                                             wxEmptyString,
                                             "images/16x16/savestudy.png",
                                             this,
                                             &Component::onSaveMatrix);
        hz->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

        // Separator
        Antares::Component::AddVerticalSeparator(grp->subpanel, hz, 2);

        // COPY / PASTE
        btn = new Antares::Component::Button(
          grp->subpanel, wxT("Copy"), "images/16x16/copy.png", this, &Component::onCopyValues);
        btn->dropDown(true);
        btn->onPopupMenu(this, &Component::createPopupCopy);
        hz->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);

        if (not readonly)
        {
            btn = new Antares::Component::Button(grp->subpanel,
                                                 wxT("Paste"),
                                                 "images/16x16/paste.png",
                                                 this,
                                                 &Component::onPasteValues);
            hz->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxALL);
        }
    }

    if (false and not copypasteOnly)
    {
        ssz->AddSpacer(5);

        // default values
        internal.modifier.selectedSet = modifierValues;
        internal.modifier.selectedAction = (uint)ModifierOperatorsData<modifierValues>::opPlus;

        auto* grp = new Antares::Component::PanelGroup(this);
        ssz->Add(grp, 0, wxALL | wxEXPAND);
        internal.modifier.ui.sizer = ssz;

        auto* sbmp
          = Resources::StaticBitmapLoadFromFile(grp, wxID_ANY, "images/16x16/update_values.png");
        grp->leftSizer->Add(sbmp, 0, wxALL | wxALIGN_CENTRE_VERTICAL);

        auto* hz = grp->subpanel->GetSizer();
        auto* operatorGroup
          = new Antares::Component::Button(grp->subpanel, wxT("Change values"), nullptr);
        operatorGroup->bold(true);
        operatorGroup->menu(true);
        operatorGroup->onPopupMenu(this, &Component::onPopupMenuModifierSet);

        hz->Add(operatorGroup, 0, wxALL | wxEXPAND);
        internal.modifier.ui.btnSetSelector = operatorGroup;

        auto* operatorSelector = new Antares::Component::Button(grp->subpanel, wxT("+"), nullptr);
        operatorSelector->menu(true);
        operatorSelector->onPopupMenu(this, &Component::onPopupMenuModifierOperators);
        auto color = operatorSelector->GetBackgroundColour();
        color.Set((unsigned char)Math::MinMax<int>(color.Red() + 20, 0, 255),
                  (unsigned char)Math::MinMax<int>(color.Green() + 20, 0, 255),
                  (unsigned char)Math::MinMax<int>(color.Blue() + 20, 0, 255));
        operatorSelector->SetBackgroundColour(color);
        hz->Add(operatorSelector, 0, wxALL | wxEXPAND);
        internal.modifier.ui.btnActionSelector = operatorSelector;

        auto* textValue = new wxTextCtrl(grp->subpanel,
                                         wxID_ANY,
                                         wxT("0"),
                                         wxDefaultPosition,
                                         wxSize(42, 18),
                                         wxBORDER_NONE); // , Toolbox::Validator::Numeric());
        hz->Add(textValue, 0, wxALL | wxALIGN_CENTER_VERTICAL);
        internal.modifier.ui.textValue = textValue;

        auto* btnSelectCalendar
          = new Antares::Component::Button(grp->subpanel, wxT("(date not set)"));
        btnSelectCalendar->menu(true);
        hz->Add(btnSelectCalendar, 0, wxALL | wxALIGN_CENTER_VERTICAL);
        btnSelectCalendar->onPopupMenu(pInternal, &InternalState::onPickDate);
        internal.modifier.ui.btnSelectCalendar = btnSelectCalendar;

        Antares::Component::AddVerticalSeparator(grp->subpanel, hz, 2);
        auto* apply = new Antares::Component::Button(
          grp->subpanel, wxT("Apply"), nullptr, this, &Component::onModifyAll);
        internal.modifier.ui.btnApply = apply;
        hz->Add(apply, 0, wxALL | wxEXPAND);

        internal.modifier.ui.sizerForInput = hz;
        internal.updaterModifiersControls();
    }
    else
    {
        internal.modifier.ui.textValue = nullptr;
        internal.modifier.ui.btnSelectCalendar = nullptr;
        internal.modifier.ui.btnSetSelector = nullptr;
        internal.modifier.ui.btnActionSelector = nullptr;
        internal.modifier.ui.btnApply = nullptr;
        internal.modifier.ui.sizer = nullptr;
        internal.modifier.ui.sizerForInput = nullptr;
    }

    // End of the layout
    sizer->Add(ssz, 0, wxALL | wxEXPAND, 2);
    sizer->Layout();
}

void Component::createModifyPanelValues(wxSizer* sizer, bool copypasteOnly)
{
    // internal variable for GUI components and other stuff
    auto& internal = *pInternal;

    // Modify
    auto* ssz = new wxBoxSizer(wxHORIZONTAL);
    internal.toolbarSizerValues = ssz;
    ssz->AddSpacer(5);

    if (not copypasteOnly)
    {
        ssz->AddSpacer(5);

        // default values
        internal.modifier.selectedSet = modifierValues;
        internal.modifier.selectedAction = (uint)ModifierOperatorsData<modifierValues>::opPlus;

        auto* grp = new Antares::Component::PanelGroup(this);
        ssz->Add(grp, 0, wxALL | wxEXPAND);
        internal.modifier.ui.sizer = ssz;

        auto* sbmp
          = Resources::StaticBitmapLoadFromFile(grp, wxID_ANY, "images/16x16/update_values.png");
        grp->leftSizer->Add(sbmp, 0, wxALL | wxALIGN_CENTRE_VERTICAL);

        auto* hz = grp->subpanel->GetSizer();
        auto* operatorGroup
          = new Antares::Component::Button(grp->subpanel, wxT("Change values"), nullptr);
        operatorGroup->bold(true);
        operatorGroup->menu(true);
        operatorGroup->onPopupMenu(this, &Component::onPopupMenuModifierSet);

        hz->Add(operatorGroup, 0, wxALL | wxEXPAND);
        internal.modifier.ui.btnSetSelector = operatorGroup;

        auto* operatorSelector = new Antares::Component::Button(grp->subpanel, wxT("+"), nullptr);
        operatorSelector->menu(true);
        operatorSelector->onPopupMenu(this, &Component::onPopupMenuModifierOperators);
        auto color = operatorSelector->GetBackgroundColour();
        color.Set((unsigned char)Math::MinMax<int>(color.Red() + 20, 0, 255),
                  (unsigned char)Math::MinMax<int>(color.Green() + 20, 0, 255),
                  (unsigned char)Math::MinMax<int>(color.Blue() + 20, 0, 255));
        operatorSelector->SetBackgroundColour(color);
        hz->Add(operatorSelector, 0, wxALL | wxEXPAND);
        internal.modifier.ui.btnActionSelector = operatorSelector;

        auto* textValue = new wxTextCtrl(grp->subpanel,
                                         wxID_ANY,
                                         wxT("0"),
                                         wxDefaultPosition,
                                         wxSize(42, 18),
                                         wxBORDER_NONE); // , Toolbox::Validator::Numeric());
        hz->Add(textValue, 0, wxALL | wxALIGN_CENTER_VERTICAL);
        internal.modifier.ui.textValue = textValue;

        auto* btnSelectCalendar
          = new Antares::Component::Button(grp->subpanel, wxT("(date not set)"));
        btnSelectCalendar->menu(true);
        hz->Add(btnSelectCalendar, 0, wxALL | wxALIGN_CENTER_VERTICAL);
        btnSelectCalendar->onPopupMenu(pInternal, &InternalState::onPickDate);
        internal.modifier.ui.btnSelectCalendar = btnSelectCalendar;

        Antares::Component::AddVerticalSeparator(grp->subpanel, hz, 2);
        auto* apply = new Antares::Component::Button(
          grp->subpanel, wxT("Apply"), nullptr, this, &Component::onModifyAll);
        internal.modifier.ui.btnApply = apply;
        hz->Add(apply, 0, wxALL | wxEXPAND);

        internal.modifier.ui.sizerForInput = hz;
        internal.updaterModifiersControls();
    }
    else
    {
        internal.modifier.ui.textValue = nullptr;
        internal.modifier.ui.btnSelectCalendar = nullptr;
        internal.modifier.ui.btnSetSelector = nullptr;
        internal.modifier.ui.btnActionSelector = nullptr;
        internal.modifier.ui.btnApply = nullptr;
        internal.modifier.ui.sizer = nullptr;
        internal.modifier.ui.sizerForInput = nullptr;
    }

    // End of the layout
    sizer->Add(ssz, 0, wxALL | wxEXPAND, 2);
    sizer->Layout();
}

void Component::forceRefreshDelayed()
{
    if (pInternal)
        Dispatcher::GUI::Post(pInternal, &InternalState::forceRefresh);
}

void Component::forceRefresh()
{
    if (pInternal)
        pInternal->forceRefresh();
}

void Component::precision(const Date::Precision p)
{
    auto& internal = *pInternal;
    if (internal.grid)
    {
        internal.precision = p;
        if (internal.filter.component)
        {
            internal.recreateFilter();
            // internal.filter.component->precision(p);
        }
        if (internal.gridHelper)
            internal.gridHelper->precision(p);
    }
}

void Component::onLoadMatrix(void*)
{
    auto& internal = *pInternal;
    if (!internal.gridHelper or !internal.grid)
        return;

    GUILocker locker;
    // avoid useless memory flush
    MemoryFlushLocker memflushlocker;

    if (not IO::IsAbsolute(LastPathForOpeningAFile))
    {
#ifdef YUNI_OS_WINDOWS
        LastPathForOpeningAFile = "C:\\";
#else
        System::Environment::Read("HOME", LastPathForOpeningAFile);
#endif
    }

    auto* dialog
      = new wxFileDialog(this,
                         wxString(wxT("Load")),
                         wxStringFromUTF8(LastPathForOpeningAFile),
                         wxT("matrix.txt"),
                         wxT("Text files (*.txt)|*.txt|CSV files (*.csv)|*.csv|All files|*.*"));
    dialog->SetFilterIndex(0);
    dialog->CentreOnParent();

    if (dialog->ShowModal() == wxID_OK)
    {
        String stdFilename;
        wxStringToString(dialog->GetPath(), stdFilename);
        dialog->Destroy();

        // Update the grid
        {
            Matrix<> m;
            if (m.loadFromCSVFile(
                  stdFilename, 1, internal.gridHelper->GetNumberRows(), Matrix<>::optImmediate))
            {
                wxString value;
                for (uint x = 0; x != m.width; ++x)
                {
                    for (uint y = 0; y != m.height; ++y)
                    {
                        value.Clear();
                        value << m.entry[x][y];
                        internal.gridHelper->SetValue(y, x, value);
                    }
                }
            }
        }

        // Force a refresh
        forceRefresh();
        // Last opened file
        IO::ExtractFilePath(LastPathForOpeningAFile, stdFilename);

        if (pInternal->shouldMarkStudyModifiedWhenModifyingCell)
            MarkTheStudyAsModified();
    }
    else
        dialog->Destroy();
}

void Component::onSaveMatrix(void*)
{
    auto& internal = *pInternal;
    if (!internal.gridHelper or !internal.grid)
        return;

    GUILocker locker;
    // avoid useless memory flush
    MemoryFlushLocker memflushlocker;

    if (!IO::IsAbsolute(LastPathForOpeningAFile))
    {
#ifdef YUNI_OS_WINDOWS
        LastPathForOpeningAFile = "C:\\";
#else
        System::Environment::Read("HOME", LastPathForOpeningAFile);
#endif
    }

    wxFileDialog* dialog = new wxFileDialog(
      this,
      wxString(wxT("Save a ")) << internal.gridHelper->GetNumberCols() << wxT("x")
                               << internal.gridHelper->GetNumberRows() << wxT(" matrix"),
      wxStringFromUTF8(LastPathForOpeningAFile),
      wxT("matrix.txt"),
      wxT("Text files (*.txt)|*.txt|CSV files (*.csv)|*.csv|All files|*.*"),
      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    dialog->SetFilterIndex(0);

    if (dialog->ShowModal() == wxID_OK)
    {
        String stdFilename;
        wxStringToString(dialog->GetPath(), stdFilename);
        dialog->Destroy();

        Matrix<> m(internal.gridHelper->GetNumberCols(), internal.gridHelper->GetNumberRows());

        // Importing the matrix
        for (int x = 0; x < internal.gridHelper->GetNumberCols(); ++x)
        {
            for (int y = 0; y < internal.gridHelper->GetNumberRows(); ++y)
                m[x][y] = internal.gridHelper->GetNumericValue(y, x);
        }

        // Exporting the matrix
        m.saveToCSVFile(stdFilename);
        // Last opened file
        IO::ExtractFilePath(LastPathForOpeningAFile, stdFilename);
    }
    else
        dialog->Destroy();
}

class SelectionRect
{
public:
    SelectionRect() : x((uint)-1), y((uint)-1), x2(), y2()
    {
    }

    uint x;
    uint y;
    uint x2;
    uint y2;

    void analyze(uint a, uint b)
    {
        if (x > a)
            x = a;
        if (x2 < a)
            x2 = a;
        if (y > b)
            y = b;
        if (y2 < b)
            y2 = b;
    }

    bool valid() const
    {
        return (x != (uint)-1);
    }
};

/*
 * Applies a functor to all selected cells. Returns the number of selected
 * cells.
 */
static void ForAllSelectedCells(wxGrid& grid, SelectionRect& rect)
{
    // Singly selected cells.
    const wxGridCellCoordsArray& cells(grid.GetSelectedCells());
    uint count = (uint)cells.size();
    for (uint i = 0; i < count; ++i)
    {
        const wxGridCellCoords& c = cells[i];
        rect.analyze(c.GetCol(), c.GetRow());
    }

    int colCount = grid.GetNumberCols();
    int rowCount = grid.GetNumberRows();

    // Whole selected rows.
    const wxArrayInt& rows(grid.GetSelectedRows());
    count = (uint)rows.size();
    for (uint i = 0; i < count; ++i)
    {
        for (int col = 0; col < colCount; ++col)
        {
            rect.analyze(0, rows[i]);
            rect.analyze(colCount, rows[i]);
        }
    }

    // Whole selected columns.
    const wxArrayInt& cols(grid.GetSelectedCols());
    count = (uint)cols.size();
    for (uint i = 0; i < count; ++i)
    {
        for (int row = 0; row < rowCount; ++row)
        {
            rect.analyze(cols[i], 0);
            rect.analyze(cols[i], rowCount);
        }
    }

    // Blocks. We always expect btl and bbr to have the same size, since their
    // entries are supposed to correspond.
    const wxGridCellCoordsArray& btl(grid.GetSelectionBlockTopLeft());
    const wxGridCellCoordsArray& bbr(grid.GetSelectionBlockBottomRight());
    count = (uint)btl.size();
    if (count == bbr.size())
    {
        for (uint i = 0; i != count; ++i)
        {
            const wxGridCellCoords& tl = btl[i];
            const wxGridCellCoords& br = bbr[i];
            for (int row = tl.GetRow(); row <= br.GetRow(); ++row)
            {
                for (int col = tl.GetCol(); col <= br.GetCol(); ++col)
                    rect.analyze(col, row);
            }
        }
    }
}

void Component::onCopyValues(void*)
{
    copyToClipboard();
}

void Component::onCopyAllValues(void*)
{
    copyAllToClipboard();
}

void Component::onPasteValues(void*)
{
    pasteFromClipboard();
}

void Component::onCopyValuesFromMenu(wxCommandEvent&)
{
    copyToClipboard();
}

void Component::onCopyAllValuesFromMenu(wxCommandEvent&)
{
    copyAllToClipboard();
}

void Component::copyToClipboard()
{
    auto& internal = *pInternal;
    GUILocker locker;
    WIP::Locker wip;

    if (internal.grid and internal.gridHelper)
    {
        auto* clipboard = new Toolbox::Clipboard();
        if (!clipboard)
            return;
        SelectionRect rect;
        ForAllSelectedCells(*internal.grid, rect);
        if (rect.valid())
        {
            if (rect.x <= rect.x2 and rect.y <= rect.y2)
            {
                clipboard->add(
                  internal.gridHelper, rect.x, rect.y, rect.x2 + 1, rect.y2 + 1, false, false);
            }
        }
        else
        {
            auto* grid = dynamic_cast<DBGrid*>(internal.grid);
            if (grid)
            {
                clipboard->add(internal.gridHelper,
                               grid->currentPosition().x,
                               grid->currentPosition().y,
                               grid->currentPosition().x + 1,
                               grid->currentPosition().y + 1,
                               false,
                               false);
            }
        }
        clipboard->copy();
        delete clipboard;
    }
}

void Component::copyAllToClipboard()
{
    auto& internal = *pInternal;
    if (internal.grid and internal.gridHelper)
    {
        GUILocker guilocker;
        WIP::Locker wip;
        Toolbox::Clipboard clipboard;
        clipboard.add(internal.gridHelper);
        clipboard.copy();
    }
}

static void PasteFromClipboard(DBGrid& grid, VGridHelper& gridHelper)
{
    if (!(&grid) or !(&gridHelper))
    {
        assert(false and "paste from clipboard: invalid grid");
        assert(false and "paste from clipboard: invalid gridHelper");
        return;
    }

    String::Vector rows;
    {
        String s;
        Toolbox::Clipboard::GetFromClipboard(s);
        if (s.empty())
            return;

        // Construction of the matrix
        s.trim("\r\n");
        if (!s)
            return;
        s.split(rows, "\n", true, true);
        if (rows.empty())
            return;
    }

    // Getting the first position
    SelectionRect rect;
    ForAllSelectedCells(grid, rect);
    if (!rect.valid())
    {
        rect.x = grid.currentPosition().x;
        rect.y = grid.currentPosition().y;
        rect.x2 = rect.x;
        rect.y2 = rect.y;
    }

    String::Vector cols;

    uint x;
    uint y = 0;
    rect.y2 = (uint)rows.size();

    {
        auto end = rows.end();
        for (auto i = rows.begin(); i != end; ++i)
        {
            x = 0;
            AnyString line = *i;
            line.trimRight("\n\r");
            cols.clear();
            line.split(cols, ";\t", true, true);
            if (cols.empty())
            {
                y++;
                continue;
            }
            logs.debug() << "paste : " << cols.size() << " cols, in " << rows.size() << " rows";
            if (y)
            {
                if (rect.x2 != cols.size())
                {
                    logs.warning() << "All rows must have the same number of columns";
                    return;
                }
            }
            else
            {
                rect.x2 = (uint)cols.size();
                if (!rect.x2)
                    return;
                if (rect.x + rect.x2 > (uint)gridHelper.GetNumberCols()
                    || rect.y + rect.y2 > (uint)gridHelper.GetNumberRows())
                {
                    logs.error() << "Impossible to paste the matrix " << rect.x2 << 'x' << rect.y2
                                 << " from the clipboard at position (" << rect.x << ',' << rect.y
                                 << ')' << " (Out of bounds)";
                    return;
                }
            }

            x = 0;
            auto cend = cols.end();
            for (auto j = cols.begin(); j != cend; ++j)
            {
                // raw text
                logs.debug() << "writing  : " << (rect.x + x) << "x" << (rect.y + y) << " = " << *j;
                gridHelper.SetValue(rect.y + y, rect.x + x, *j);
                logs.debug() << "  done writing  : " << (rect.x + x) << "x" << (rect.y + y) << " = "
                             << *j;
                ++x;
            }
            ++y;
        }
    }

    // Reduce the memory footprint before modifying cell values
    cols.clear();
    rows.clear();

    grid.ForceRefresh();
    if (gridHelper.markTheStudyAsModified())
        MarkTheStudyAsModified();
}

void Component::pasteFromClipboard()
{
    auto& internal = *pInternal;
    if (internal.gridHelper and internal.grid)
    {
        GUILocker locker;
        WIP::Locker wip;
        PasteFromClipboard(*(dynamic_cast<DBGrid*>(internal.grid)), *internal.gridHelper);
        forceRefreshDelayed();
    }
}

void Component::onStudyClosed()
{
    if (pInternal)
    {
        auto& internal = *pInternal;
        forceRefresh();
        if (internal.grid)
            internal.grid->SetTable(nullptr, false);
        delete internal.gridHelper;
        internal.gridHelper = nullptr;
    }
}

void Component::enableRefresh(bool enabled)
{
    assert(pInternal);
    auto& internal = *pInternal;
    if (internal.grid)
        internal.grid->enableRefresh(enabled);
}

void Component::createPopupCopy(Button&, wxMenu& menu, void*)
{
    wxMenuItem* it;

    it = Menu::CreateItem(
      &menu, wxID_ANY, wxT("Copy the selection to the clipboard\tCtrl+C"), "images/16x16/copy.png");
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::onCopyValuesFromMenu),
                 nullptr,
                 this);

    it = Menu::CreateItem(&menu, wxID_ANY, wxT("Copy the entire matrix with styles (HTML)"));
    menu.Connect(it->GetId(),
                 wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(Component::onCopyAllValuesFromMenu),
                 nullptr,
                 this);
}

void Component::scroll(wxScrolledWindow* component)
{
    if (!component)
        return;
    assert(pInternal);
    auto& internal = *pInternal;
    if (!internal.grid)
        return;

    if (dynamic_cast<wxScrolledWindow*>(internal.grid) == component)
        return;

    int x, y;
    component->GetViewStart(&x, &y);
    internal.grid->Scroll(x, y);
}

void Component::setOtherGrid(Component* other)
{ 
    pInternal->grid->setOtherGrid(other->pInternal->grid);
}

wxScrolledWindow* Component::gridAsScrolledWindow()
{
    assert(pInternal);
    auto& internal = *pInternal;
    return (internal.grid ? dynamic_cast<wxScrolledWindow*>(internal.grid) : nullptr);
}

bool Component::markTheStudyAsModified() const
{
    return pInternal ? pInternal->shouldMarkStudyModifiedWhenModifyingCell : false;
}

void Component::markTheStudyAsModified(bool flag)
{
    if (pInternal)
        pInternal->shouldMarkStudyModifiedWhenModifyingCell = flag;
}

void Component::onModifyAll(void*)
{
    assert(pInternal);
    auto& internal = *pInternal;
    if (not internal.gridHelper or not internal.modifier.ui.textValue or not internal.renderer)
        return;

    // Retrieving the edit value as double
    String value;
    wxStringToString(internal.modifier.ui.textValue->GetValue(), value);

    //
    onBeginUpdate();
    wxString str;

    ModifierOperators::ApplyChanges(internal.modifier.selectedSet,
                                    internal.modifier.selectedAction,
                                    value,
                                    internal.renderer,
                                    internal.gridHelper);

    internal.renderer->onRefresh();
    onEndUpdate();
}

void Component::onComboUpdated(wxCommandEvent& evt)
{
    auto study = Data::Study::Current::Get();
    if (!study)
        return;

    // OnMapLayerChanged(&(pInternal->pLayerFilter->GetValue()));

    onBeginUpdate();

    onEndUpdate();
}

void Component::onPopupMenuModifierSet(Button&, wxMenu& menu, void*)
{
    for (uint i = 0; i != (uint)modifierMax; ++i)
    {
        auto set = (ModifierSet)i;
        auto* caption = ModifierOperators::Name(set);
        auto* userdata = new PopupMenuModifierSet(set);

        auto* it
          = Menu::CreateItem(&menu, wxID_ANY, caption, "images/16x16/empty.png", wxEmptyString);
        Connect(it->GetId(),
                wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(Component::onChangeModifierSet),
                userdata,
                this);
    }
}

void Component::onPopupMenuModifierOperators(Button&, wxMenu& menu, void*)
{
    assert(pInternal);
    auto& internal = *pInternal;
    auto set = internal.modifier.selectedSet;
    uint count = ModifierOperators::OperatorCount(set);
    for (uint i = 0; i != count; ++i)
    {
        auto* caption = ModifierOperators::OperatorToCString(set, i);
        auto* userdata = new PopupMenuModifierOperator(i);

        auto* it
          = Menu::CreateItem(&menu, wxID_ANY, caption, "images/16x16/empty.png", wxEmptyString);
        Connect(it->GetId(),
                wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(Component::onChangeModifierOperator),
                userdata,
                this);
    }
}

void Component::onChangeModifierSet(wxCommandEvent& event)
{
    assert(pInternal);
    auto& internal = *pInternal;
    wxObject* userdataobj = event.m_callbackUserData;
    auto* userdata = dynamic_cast<PopupMenuModifierSet*>(userdataobj);
    if (userdata)
    {
        internal.modifier.selectedSet = userdata->modifierSet;
        internal.modifier.selectedAction = 0;
        internal.updaterModifiersControls();
        if (internal.modifier.ui.textValue)
            internal.modifier.ui.textValue->SetFocus();
    }
}

void Component::onChangeModifierOperator(wxCommandEvent& event)
{
    assert(pInternal);
    auto& internal = *pInternal;
    wxObject* userdataobj = event.m_callbackUserData;
    auto* userdata = dynamic_cast<PopupMenuModifierOperator*>(userdataobj);
    if (userdata)
    {
        internal.modifier.selectedAction = userdata->modifierOperator;
        internal.updaterModifiersControls();
        if (internal.modifier.ui.textValue)
            internal.modifier.ui.textValue->SetFocus();
    }
}

Renderer::IRenderer* Component::renderer() const
{
    assert(pInternal);
    return pInternal->renderer;
}

void Component::renderer(Renderer::IRenderer* newrenderer)
{
    assert(pInternal);
    auto& internal = *pInternal;
    if (!internal.grid) // no grid ? Strange but nothing to do here
        return;

    // If an existing renderer is already attached to this component,
    // we have to destroy it
    if (internal.renderer)
    {
        // Disconnect all events
        internal.renderer->onRefresh.clear();
        delete internal.renderer;
    }

    // Broadcasting the new renderer
    internal.cellRenderer->renderer = newrenderer;
    internal.renderer = newrenderer;

    // resetting the new date interval accordingly
    if (newrenderer)
    {
        internal.precision = newrenderer->precision();
        internal.renderer->dataGridPrecision = internal.precision;
    }
    else
        internal.precision = Date::stepNone; // default

    // recreate all filters accordingly
    internal.recreateFilter();

    forceRefresh();

    // post event
    if (internal.renderer)
    {
        internal.renderer->dataGridPrecision = internal.precision;
        internal.renderer->onRefresh.connect(this, &Component::forceRefresh);
    }
}

DBGrid* Component::grid() const
{
    return pInternal->grid;
}

Date::Precision Component::precision() const
{
    return pInternal->precision;
}

} // namespace Datagrid
} // namespace Component
} // namespace Antares
