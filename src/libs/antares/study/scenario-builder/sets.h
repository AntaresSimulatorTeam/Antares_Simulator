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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_SETS_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_SETS_H__

#include <yuni/yuni.h>
#include "rules.h"

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
/*!
** \brief Sets for TS numbers, for all years and a single timeseries
*/
class Sets final : private Yuni::NonCopyable<Sets>
{
public:
    //! Iterator
    typedef Rules::Map::iterator iterator;
    //! Const iterator
    typedef Rules::Map::const_iterator const_iterator;

public:
    //! Update mode, indicates wether we are called from with a ScenarioBuilderUpdater instance or
    //! not
    bool inUpdaterMode;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param tstype Type of the timeseries
    */
    Sets();
    //! Destructor
    ~Sets();
    //@}

    //! \name Data manupulation
    //@{
    /*!
    ** \brief Load data from the study
    */
    bool loadFromStudy(Study& study);

    /*!
    ** \brief Clear all data
    */
    void clear();

    /*!
    ** \brief Load all rulesets from an INI file
    */
    template<class StringT>
    bool loadFromINIFile(const StringT& filename);
    /*!
    ** \brief Save all rulesets into a mere INI file
    */
    template<class StringT>
    bool saveToINIFile(const StringT& filename);
    //@}

    //! \name Set manipulation
    //@{
    /*!
    ** \brief Create a new set
    **
    ** \return A non-null pointer if the operation succeeded, null otherwise
    */
    Rules::Ptr createNew(const RulesScenarioName& name);

    /*!
    ** \brief Test if a rules set exist
    **
    ** \param lname Name of the rule set (in lower case)
    */
    bool exists(const RulesScenarioName& lname) const;

    /*!
    ** \brief Find a rule set
    */
    Rules::Ptr find(const RulesScenarioName& lname) const;

    /*!
    ** \brief Rename a given ruleset
    **
    ** \return The object to the ruleset, null if the operation failed
    */
    Rules::Ptr rename(const RulesScenarioName& lname, const RulesScenarioName& newname);

    /*!
    ** \brief Delete a ruleset
    **
    ** \return True if the operation suceeded, false otherwise
    */
    bool remove(const RulesScenarioName& lname);

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    /*!
    ** \brief Get the number of available sets
    */
    uint size() const;

    /*!
    ** \brief Get if empty
    */
    bool empty() const;
    //@}

private:
    //! Load all rulesets into a mere INI file
    bool internalLoadFromINIFile(const AnyString& filename);
    //! Save all rulesets into a mere INI file
    bool internalSaveToIniFile(const AnyString& filename) const;

private:
    //! All available sets, ordered by their lower name
    Rules::Map pMap;
    //! Alias to the study
    Study* pStudy;
}; // class Sets

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#include "sets.hxx"

#endif // __LIBS_STUDY_SCENARIO_BUILDER_SETS_H__
