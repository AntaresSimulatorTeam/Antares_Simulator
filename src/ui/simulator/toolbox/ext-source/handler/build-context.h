/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_BUILD_CONTEXT_H__
#define __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_BUILD_CONTEXT_H__

#include <antares/study/study.h>

namespace Antares
{
namespace ExtSource
{
namespace Handler
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

} // namespace Handler
} // namespace ExtSource
} // namespace Antares

#endif // __ANTARES_TOOLBOX_EXT_SOURCE_HANDLER_BUILD_CONTEXT_H__
