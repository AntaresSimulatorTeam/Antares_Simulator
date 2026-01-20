// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_OUTPUT_CONTENT_H__
#define __ANTARES_WINDOWS_OUTPUT_CONTENT_H__

#include <yuni/yuni.h>
#include <set>
#include <map>
#include "fwd.h"
#include <antares/study/fwd.h>

namespace Antares::Private::OutputViewerData
{
class Content final
{
public:
    using AreaNameSet = std::set<Data::AreaName>;
    using LinkNameSet = std::set<Data::AreaLinkName>;

    class SimulationType
    {
    public:
        //! Get if the result set is empty
        bool empty() const;

    public:
        //! All areas
        AreaNameSet areas;
        //! All links
        LinkNameSet links;

    }; // class SimulationType

public:
    //! Constructor & Destructor
    //@{
    //! Default contructor
    Content();
    //! Destructor
    ~Content();
    //@}

    bool empty() const;

public:
    //! All areas/links found for economy
    SimulationType economy;
    //! All areas/links found for adequacy
    SimulationType adequacy;

    //! All available clusters
    ThermalNameSetPerArea clusters;

    //! Flag to know if some year-by-year data are presetn
    bool hasYearByYear;
    //! Flag to know if concatened individual years are present
    bool hasConcatenedYbY;
    //! Year interval for year-by-year data
    uint ybyInterval[2];

}; // class Content

} // namespace Antares::Private::OutputViewerData

#include "content.hxx"

#endif // __ANTARES_WINDOWS_OUTPUT_CONTENT_H__
