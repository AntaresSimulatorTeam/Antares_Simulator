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
#ifndef __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include "../fwd.h"
#include "TSnumberData.h"
#include "hydroLevelsData.h"
#include <map>
#include <memory>

namespace Antares
{
namespace Data
{
namespace ScenarioBuilder
{
/*!
** \brief Rules for TS numbers, for all years and a single timeseries
*/
class Rules final : private Yuni::NonCopyable<Rules>
{
public:
    //! Smart pointer
    using  Ptr = std::shared_ptr<Rules>;
    //! Map
    typedef std::map<RulesScenarioName, Ptr> Map;
    //! Map ID
    typedef std::map<int, Ptr> MappingID;

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    **
    ** \param tstype Type of the timeseries
    */
    explicit Rules(Study& study);
    //! Destructor
    ~Rules() = default;
    //@}

    //! \name Data manupulation
    //@{
    /*!
    ** \brief Initialize data from the study
    */
    bool reset();

    /*!
    ** \brief Load information from a single line (extracted from an INI file)
    */
    bool readLine(const AreaName::Vector& splitKey,
                        String value,
                        bool updaterMode);

    /*!
    ** \brief Export the data into a mere INI file
    */
    void saveToINIFile(Yuni::IO::File::Stream& file) const;
    //@}

    //! Get the number of areas
    uint areaCount() const;

    //! Name of the rules set
    const RulesScenarioName& name() const;
    void setName(RulesScenarioName name);

    /*!
    ** \brief Apply the changes to the timeseries number matrices
    **
    ** This method is only useful when launched from the solver.
    */
    bool apply();

    // When current rule is the active one, sends warnings for disabled clusters.
    void sendWarningsForDisabledClusters();

public:
    //! Load
    loadTSNumberData load;
    //! Solar
    solarTSNumberData solar;
    //! Hydro
    hydroTSNumberData hydro;
    //! Wind
    windTSNumberData wind;

    //! Thermal (array [0..pAreaCount - 1])
    std::vector<thermalTSNumberData> thermal;
    //! Renewable (array [0..pAreaCount - 1])
    std::vector<renewableTSNumberData> renewable;



    //! hydro levels
    hydroLevelsData hydroLevels;

    // Links NTC
    std::vector<ntcTSNumberData> linksNTC;

private:
    // Member methods
    bool readThermalCluster(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readRenewableCluster(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readLoad(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readWind(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readHydro(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readSolar(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readHydroLevels(const AreaName::Vector& instrs, String value, bool updaterMode);
    bool readLink(const AreaName::Vector& instrs, String value, bool updaterMode);

    Data::Area* getArea(const AreaName& areaname, bool updaterMode);
    Data::AreaLink* getLink(const AreaName& fromAreaName, const AreaName& toAreaName, bool updaterMode);

    // Member data 
    Study& study_;
    //! Total number of areas
    uint pAreaCount;
    //! Name of the rules
    RulesScenarioName pName;
    // Disabled clusters when current rule is active (useful for sending warnings)
    map<string, vector<uint>> disabledClustersOnRuleActive;

}; // class Rules

} // namespace ScenarioBuilder
} // namespace Data
} // namespace Antares

#include "rules.hxx"

#endif // __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
