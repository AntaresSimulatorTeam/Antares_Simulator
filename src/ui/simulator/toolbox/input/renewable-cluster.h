// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_INPUT_RENEWABLE_CLUSTER_H__
#define __ANTARES_TOOLBOX_INPUT_RENEWABLE_CLUSTER_H__

// gp : remove useless header includes
// #include <yuni/core/event.h>
// #include <antares/study/study.h>
#include "input.h"
#include "area.h"
// #include <wx/panel.h>
// #include <wx/arrstr.h>
// #include <wx/stattext.h>
#include <wx/imaglist.h>
// #include "../components/htmllistbox/component.h"
// #include "../components/htmllistbox/item/thermal-cluster.h"
#include "../components/htmllistbox/item/renewable-cluster-item.h"
#include "../components/htmllistbox/datasource/renewable-cluster-order.h"
#include "../components/htmllistbox/item/renewable-cluster-item.h"

namespace Antares::Toolbox::InputSelector
{
using namespace Component::HTMLListbox::Datasource;

/*!
** \brief Visual Component for displaying renewable clusters of an arbitrary area
*/
class RenewableCluster final: public AInput, public Yuni::IEventObserver<RenewableCluster>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    RenewableCluster(wxWindow* parent, InputSelector::Area* area);
    //! Destructor
    virtual ~RenewableCluster();
    //@}

    virtual void update();

    // gp : to be adapted
    /*
    virtual wxPoint recommendedSize() const
    {
        return wxPoint(150, 240);
    }
    */

    void updateInnerValues();

    // gp : should be removed
    /*!
    ** \brief Rename a thermal cluster and refresh inner values
    **
    ** \param cluster The thermal cluster to rename
    ** \param newName The new name of the thermal cluster
    ** \param broadcast Trigger the event to tell that the thermal cluster has changed
    */
    // void renameAggregate(Antares::Data::ThermalCluster* cluster,
    //                      const wxString& newName,
    //                      const bool broadcast = true);

public:
    Yuni::Event<void(Antares::Data::RenewableCluster*)> onClusterChanged;

protected:
    /*!
    ** \brief Create all needed controls (called by the constructor)
    */
    virtual void internalBuildSubControls();
    // gp : to be adapted
    //! Event: a group has been changed
    // void onThermalGroupChanged(Antares::Data::Area* area);

private:
    /*!
    ** \brief Event: The parent area has changed
    ** \param area The new parent area (can be NULL)
    */
    void areaHasChanged(Antares::Data::Area* area);

    //! End update of the study
    void onStudyEndUpdate();

    //! Create a new thermal cluster
    void internalAddPlant(void*);
    //! Delete the selection thermal cluster
    void internalDeletePlant(void*);
    //! Delete the selection thermal cluster
    void internalClonePlant(void*);

    void internalDeleteAll(void*);

    /*!
    ** \brief Event: The application is going to quit
    ** \internal This method prevent against unwanted refresh with dead pointers
    */
    void onApplicationOnQuit();

    /*!
    ** \brief The study has been closed
    */
    void onStudyClosed();

    /*!
    ** \brief Event: An item has been selected in the listbox
    ** \param item The selected item
    */
    void onRnSelected(Component::HTMLListbox::Item::IItem::Ptr item);

    void onStudyRenewableClusterCommonSettingsChanged();

    void onStudyRenewableClusterRenamed(Antares::Data::RenewableCluster* cluster);

    void onStudyRenewableClusterGroupChanged(Antares::Data::Area* area);

    void onDeleteDropdown(Antares::Component::Button&, wxMenu& menu, void*);

    void evtPopupDelete(wxCommandEvent&);

    void evtPopupDeleteAll(wxCommandEvent&);

    void delayedSelection(Component::HTMLListbox::Item::IItem::Ptr item);

    void updateWhenGroupChanges();

private:
    //! The current Area
    Antares::Data::Area* pArea;
    //! Label for displaying additional informations (such as the total power in MW)
    wxStaticText* pTotalMW;
    //! Image list
    wxImageList pImageList;
    //! The listbox (which contain all items)
    Component::HTMLListbox::Component* pRnListbox;
    //! The area notifier
    InputSelector::Area* pAreaNotifier;
    //! The last selected thermal cluster
    Component::HTMLListbox::Item::RenewableClusterItem::Ptr pLastSelectedRenewableCluster;

    RenewableClustersByAlphaOrder* pDataSourceAZ;
    RenewableClustersByAlphaReverseOrder* pDataSourceZA;

}; // class Area

} // namespace Antares::Toolbox::InputSelector

#endif // __ANTARES_TOOLBOX_INPUT_RENEWABLE_CLUSTER_H__
