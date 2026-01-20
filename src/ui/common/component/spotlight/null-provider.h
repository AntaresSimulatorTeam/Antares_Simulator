// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_NULL_PROVIDER_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_NULL_PROVIDER_HXX__

/*!
** \brief A null provider
*/
class NullProvider : public IProvider
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    NullProvider()
    {
    }
    //! Destructor
    virtual ~NullProvider()
    {
    }
    //@}

    /*!
    ** \brief Rebuild the result list from a list of tokens
    */
    virtual void search(IItem::Vector&, const SearchToken::Vector&)
    {
        // do nothing
    }

}; // class NullProvider

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_NULL_PROVIDER_HXX__
