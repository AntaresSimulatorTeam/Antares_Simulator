// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
#define __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__

#include <map>
#include <memory>

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
    using Map = std::map<RulesScenarioName, Ptr>;
    //! Map ID
    using MappingID = std::map<int, Ptr>;

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
    bool readLine(const AreaName::Vector& splitKey, const String& value, bool updaterMode = false);

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
    hydroLevelsData hydroInitialLevels = {"hl,", initLevelApply};
    //! hydro final levels
    hydroLevelsData hydroFinalLevels = {"hfl,", finalLevelApply};

    // Links NTC
    std::vector<ntcTSNumberData> linksNTC;

    BindingConstraintsTSNumberData binding_constraints;

    std::vector<ShortTermInflowsTSNumberData> shortTermStorageInflows;
    std::vector<ShortTermAdditionalConstraintsTSNumberData> shortTermStorageAdditionalConstraints;

private:
    // Member methods
    bool readThermalCluster(const AreaName::Vector& instrs, const String& value, bool updaterMode);
    bool readRenewableCluster(const AreaName::Vector& instrs,
                              const String& value,
                              bool updaterMode);
    bool readLoad(const AreaName::Vector& instrs, const String& value, bool updaterMode);
    bool readWind(const AreaName::Vector& instrs, const String& value, bool updaterMode);
    bool readHydro(const AreaName::Vector& instrs, const String& value, bool updaterMode);
    bool readSolar(const AreaName::Vector& instrs, const String& value, bool updaterMode);
    bool readInitialHydroLevels(const AreaName::Vector& instrs,
                                const String& value,
                                bool updaterMode);
    bool readFinalHydroLevels(const AreaName::Vector& instrs,
                              const String& value,
                              bool updaterMode);
    bool readLink(const AreaName::Vector& instrs, const String& value, bool updaterMode);
    bool readBindingConstraints(const AreaName::Vector& splitKey, const String& value);

    bool readShortTermStorageInflows(const AreaName::Vector& splitKey,
                                     const String& value,
                                     bool updaterMode);

    bool readShortTermStorageAdditionalConstraints(const AreaName::Vector& splitKey,
                                                   const String& value,
                                                   bool updaterMode);

    Data::Area* getArea(const AreaName& areaname, bool updaterMode);
    const Data::AreaLink* getLink(const AreaName& fromAreaName,
                                  const AreaName& toAreaName,
                                  bool updaterMode) const;
    bool checkGroupExists(const std::string& groupName) const;

    // Member data
    Study& study_;
    //! Total number of areas
    uint pAreaCount;
    //! Name of the rules
    RulesScenarioName pName;
    // Disabled clusters when current rule is active (useful for sending warnings)
    std::map<std::string, std::vector<uint>> disabledClustersOnRuleActive;

}; // class Rules

} // namespace Antares::Data::ScenarioBuilder

#include "rules.hxx"

#endif // __LIBS_STUDY_SCENARIO_BUILDER_RULES_H__
