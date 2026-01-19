// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
