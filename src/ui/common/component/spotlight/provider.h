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
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_PROVIDER_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_PROVIDER_HXX__

#include <memory>

/*!
** \brief Data Provider
*/
class IProvider : public Yuni::IEventObserver<Spotlight>
{
public:
    //! ptr
    using Ptr = std::shared_ptr<IProvider>;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    IProvider()
    {
    }
    //! Destructor
    virtual ~IProvider();
    //@}

    //! \name Search
    //@{
    /*!
    ** \brief Rebuild the result list from a list of tokens
    */
    virtual void search(IItem::Vector& out,
                        const SearchToken::Vector& tokens,
                        const Yuni::String& text = "")
      = 0;

    /*!
    ** \brief Redo the research
    */
    void redoResearch();
    //@}

    //! \name Selection
    //@{
    //! Event: an item has been selected
    virtual bool onSelect(IItem::Ptr& item);
    //! Event: a list of items have been selected (more than one)
    virtual bool onSelect(const IItem::Vector& items);

    //! Event: an item has been selected
    virtual bool onDoubleClickSelect(IItem::Ptr& item);
    //@}

    //! \name Component
    //@{
    //! Get the pointer of the attached component (can be null)
    Spotlight* component() const;
    //@}

protected:
    //! The parent component (can be null)
    Spotlight* pSpotlightComponent;
    friend class Spotlight;

}; // class IProvider

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_PROVIDER_HXX__
