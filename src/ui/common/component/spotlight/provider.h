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
#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_PROVIDER_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_PROVIDER_HXX__

/*!
** \brief Data Provider
*/
class IProvider : public Yuni::IEventObserver<Spotlight>
{
public:
    //! ptr
    typedef Yuni::SmartPtr<IProvider> Ptr;

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
