// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__

#include <map>
#include <memory>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/string.h>

#include "../fwd.h"
#include "BindingConstraintsTSNumbersData.h"
#include "HydroTSNumberData.h"
#include "LoadTSNumberData.h"
#include "NTCTSNumberData.h"
#include "RenewableTSNumberData.h"
#include "ShortTermAdditionalConstraintsTSNumberData.h"
#include "ShortTermInflowsTSNumberData.h"
#include "TSnumberData.h"
#include "ThermalTSNumberData.h"
#include "WindTSNumberData.h"
#include "hydroLevelsData.h"
#include "solarTSNumberData.h"

namespace Antares::Data::ScenarioBuilder
{
/*!
** \brief Rules for TS numbers, for all years and a single timeseries
*/
class Rules final: private Yuni::NonCopyable<Rules>
{
public:
    //! Smart pointer
    using Ptr = std::shared_ptr<Rules>;
    //! Map
    using Map = std::map<std::string, Ptr>;

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
    bool readLine(const std::vector<std::string>& splitKey, const String& value);

    //! Get the number of areas
    uint areaCount() const;

    //! Name of the rules set
    const std::string& name() const;
    void setName(std::string name);

    /*!
    ** \brief Apply the changes to the timeseries number matrices
    **
    ** This method is only useful when launched from the solver.
    */
    bool apply();

    // When current rule is the active one, sends warnings for disabled clusters.
    void sendWarningsForDisabledClusters();

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

    //! hydro initial levels
    hydroLevelsData hydroInitialLevels{initLevelApply};
    //! hydro final levels
    hydroLevelsData hydroFinalLevels{finalLevelApply};

    // Links NTC
    std::vector<ntcTSNumberData> linksNTC;

    BindingConstraintsTSNumberData binding_constraints;

    std::vector<ShortTermInflowsTSNumberData> shortTermStorageInflows;
    std::vector<ShortTermAdditionalConstraintsTSNumberData> shortTermStorageAdditionalConstraints;

private:
    // Member methods
    bool readThermalCluster(const std::vector<std::string>& instrs, const String& value);
    bool readRenewableCluster(const std::vector<std::string>& instrs, const String& value);
    bool readLoad(const std::vector<std::string>& instrs, const String& value);
    bool readWind(const std::vector<std::string>& instrs, const String& value);
    bool readHydro(const std::vector<std::string>& instrs, const String& value);
    bool readSolar(const std::vector<std::string>& instrs, const String& value);
    bool readInitialHydroLevels(const std::vector<std::string>& instrs, const String& value);
    bool readFinalHydroLevels(const std::vector<std::string>& instrs, const String& value);
    bool readLink(const std::vector<std::string>& instrs, const String& value);
    bool readBindingConstraints(const std::vector<std::string>& splitKey, const String& value);

    bool readShortTermStorageInflows(const std::vector<std::string>& splitKey, const String& value);

    bool readShortTermStorageAdditionalConstraints(const std::vector<std::string>& splitKey,
                                                   const String& value);

    Data::Area* getArea(const AreaName& areaname);
    const Data::AreaLink* getLink(const AreaName& fromAreaName, const AreaName& toAreaName) const;
    bool checkGroupExists(const std::string& groupName) const;

    // Member data
    Study& study_;
    //! Total number of areas
    uint pAreaCount;
    //! Name of the rules
    std::string pName;
    // Disabled clusters when current rule is active (useful for sending warnings)
    std::map<std::string, std::vector<uint>> disabledClustersOnRuleActive;

}; // class Rules

} // namespace Antares::Data::ScenarioBuilder

#include "rules.hxx"

#endif // __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
