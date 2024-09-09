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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
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
class Sets final: private Yuni::NonCopyable<Sets>
{
public:
    //! Iterator
    using iterator = Rules::Map::iterator;
    //! Const iterator
    using const_iterator = Rules::Map::const_iterator;

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

    void setStudy(Study& study);

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
