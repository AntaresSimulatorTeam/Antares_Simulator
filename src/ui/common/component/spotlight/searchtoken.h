// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_SEARCH_TOKEN_HXX__
#define __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_SEARCH_TOKEN_HXX__

/*!
** \brief A search token
*/
class SearchToken final
{
public:
    //! The most suitable smart point for the class
    using Ptr = std::shared_ptr<SearchToken>;
    //! Vector of search tokens
    using Vector = std::vector<Ptr>;
    //! Vector ptr
    using VectorPtr = std::shared_ptr<Vector>;

public:
    SearchToken() : weight(1.f)
    {
    }
    //! Text
    Yuni::String text;
    //! Weight of the token
    float weight;
};

#endif // __ANTARES_UI_COMMON_COMPONENT_SPOTLIGHT_SPOTLIGHT_SEARCH_TOKEN_HXX__
