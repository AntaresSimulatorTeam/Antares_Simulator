// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_DATASOURCE_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_DATASOURCE_H__

#include <map>
#include <wx/wx.h>

namespace Antares::Component::HTMLListbox
{
// Forward declaration
class Component;

namespace Datasource
{
/*!
** \brief Datasource for HTMLListbox::Component
**
** This class allow a wrapper to provide informations
** for a HTMLListbox::Component.
*/
class IDatasource
{
public:
    //! List
    using Map = std::map<int, IDatasource*>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    ** \param parent The parent component
    */
    IDatasource(HTMLListbox::Component& parent);
    //! Destructor
    virtual ~IDatasource();
    //@}

    /*!
    ** \brief Get the name of the datasource
    */
    virtual wxString name() const = 0;

    /*!
    ** \brief Get the relative path to the icon file
    */
    virtual const char* icon() const = 0;

    /*!
    ** \brief Refresh the list
    */
    virtual void refresh(const wxString& search = wxEmptyString) = 0;

protected:
    //! The parent component
    HTMLListbox::Component& pParent;

}; // class IDatasource

} // namespace Datasource
} // namespace Antares::Component::HTMLListbox

#include "datasource.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_H__
