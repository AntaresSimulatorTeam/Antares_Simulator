/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** XNothingX in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_DATASOURCE_H__
#define __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_DATASOURCE_H__

#include <map>
#include <wx/wx.h>

namespace Antares
{
namespace Component
{
namespace HTMLListbox
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
} // namespace HTMLListbox
} // namespace Component
} // namespace Antares

#include "datasource.hxx"

#endif // __ANTARES_TOOLBOX_COMPONENT_HTMLLISTBOX_COMPONENT_H__
