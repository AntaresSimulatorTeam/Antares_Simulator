// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_BUILD_CONTEXT_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_BUILD_CONTEXT_H__

#include <antares/study/study.h>

namespace Antares::ExtSource::Handler
{
class BuildContext
{
public:
    //! Set of area' names
    using AreaSet = std::set<Data::AreaName>;
    using LinkSet = std::map<Data::AreaName, std::map<Data::AreaName, bool>>;
    using ThermalClusterSet = std::map<Data::AreaName, std::map<YString, bool>>;
    using ConstraintSet = std::set<Yuni::String>;

public:
    //! \name Constructor
    //@{
    /*!
    ** \brief Default constructor
    */
    BuildContext();
    //@}

    /*!
    ** \brief Check if the items copied are strictly equivalent to the current user selection
    */
    bool checkIdentity();

    /*!
    ** \brief Check if the paste can be done
    */
    bool checkIntegrity(bool forceDialog);

public:
    //! All areas
    Data::Area::NameSet area;
    LinkSet link;
    ThermalClusterSet cluster;
    ConstraintSet constraint;

    std::map<Data::AreaName, Data::AreaName> forceAreaName;

    bool shouldOverwriteArea;

    //! Flag to determine whether the source study was modified when copied
    // This flag is required to prevent against invalid copy from another instance
    bool modifiedWhenCopied;

}; // class BuildContext

} // namespace Antares::ExtSource::Handler

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_BUILD_CONTEXT_H__
