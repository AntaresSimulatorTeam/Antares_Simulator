/*
** Copyright 2007-2023 RTE
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
#pragma once

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/noncopyable.h>
#include "../fwd.h"
#include "../../constants.h"

#include "../parts/thermal/cluster.h"
#include <antares/array/matrix.h>
#include <antares/inifile/inifile.h>

#include "antares/study/filter.h"

#include <memory>
#include <utility>
#include <vector>
#include <set>
#include <antares/utils/utils.h>
#include "antares/study/area/area.h"
#include <filesystem>

namespace Antares::Data
{
// Forward declaration
struct CompareMaintenanceGroupName;
class MaintenanceGroup : public Yuni::NonCopyable<MaintenanceGroup>
{
    friend class MaintenanceGroupLoader;
    friend class MaintenanceGroupSaver;

public:
    enum ResidualLoadDefinitionType
    {
        //! Unknown status
        typeUnknown = 0,
        //! residual Load timeserie is defined using weights
        typeWeights,
        //! residual Load timeserie is given directly as an input
        typeTimeserie,
        //! The maximum number of types
        typeMax
    };

    struct Weights final
    {
        double load;
        double renewable;
        double ror;
    };

    using MaintenanceGroupName = std::string;
    //! Map of load, renewable or ror weight-s
    using weightMap = std::map<const Area*, Weights, CompareAreaName>;

    /*!
     ** \brief Convert a Residual Load Definition Type into a mere C-String
     */
    static const char* ResidualLoadDefinitionTypeToCString(ResidualLoadDefinitionType t);

    /*!
    ** \brief Convert a string into its corresponding Residual Load Definition Type
    */
    static ResidualLoadDefinitionType StringToResidualLoadDefinitionType(const AnyString& text);

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Destructor
    */
    ~MaintenanceGroup();
    //@}

    //! \name / ID
    //@{
    /*!
    ** \brief Get the name of the Maintenance Group
    */
    const MaintenanceGroupName& name() const; // using here different string definition
    /*!
    ** \brief Set the name of the Maintenance Group
    **
    ** The ID will be changed in the same time
    */
    void name(const std::string& newname); // using here different string definition

    /*!
    ** \brief Get the ID of the binding constraint
    */
    const MaintenanceGroupName& id() const;
    //@}

    /*!
    ** \brief resets all to default data
    */
    void resetToDefaultValues();

    /*!
    ** \brief returns if Maintenance Group is skipped for generating ts and if it is active
    */
    bool skipped() const;
    bool isActive() const;

    // used only for UI - maybe to added later
    bool hasAllWeightedClustersOnLayer(size_t layerID);

    //! \name per Area
    //@{
    /*!
    ** \brief Get the load, renewable or ror weight of a given area
    **
    ** \return The load, renewable or ror weight of the thermal area. 0. if not found
    */
    double loadWeight(const Area* area) const;
    double renewableWeight(const Area* area) const;
    double rorWeight(const Area* area) const;

    /*!
    ** \brief Set the load, renewable or ror weight of a given area
    */
    void loadWeight(const Area* area, double w);
    void renewableWeight(const Area* area, double w);
    void rorWeight(const Area* area, double w);

    /*!
    ** \brief Remove all weights for load, renewable and ror
    */
    void removeAllWeights();

    // used only in UI
    /*!
    ** \brief Copy all weights from another Maintenance Group
    */
    void copyWeights();

    /*!
    ** \brief Get how many areas the Maintenance Group contains
    */
    uint areaCount() const;
    //@}

    //! \name Type of the Maintenance Group
    //@{
    /*!
    ** \brief Get the ResidualLoadDefinitionType of the Maintenance Group
    */
    ResidualLoadDefinitionType type() const;

    /*!
    ** \brief Set the ResidualLoadDefinitionType of the Maintenance Group
    */
    void setResidualLoadDefinitionType(ResidualLoadDefinitionType t);
    //@}

    //! \name Enabled / Disabled
    //@{
    //! Get if the Maintenance Group is enabled
    bool enabled() const;
    //! Enabled / Disabled the Maintenance Group
    void enabled(bool v);
    //@}

    //! \name Reset // this is only used in UI and for BC in Kirchhoff generator, so we do not
    //! really need it here
    //@{
    /*!
    ** \brief Clear all values and reset the Maintenance Group to its new type
    **
    ** \param name Name of the Maintenance Group
    ** \param newType Its new ResidualLoadDefinitionType
    */
    void clearAndReset(const MaintenanceGroupName& name, ResidualLoadDefinitionType newType);
    //@}

    //! \name Memory Usage
    //@{
    /*!
    ** \brief Get the memory usage
    */
    uint64_t memoryUsage() const;
    //@}

    /*!
    ** \brief Get if the Maintenance Group contains a given area
    */
    bool contains(const Area* area) const;

private:

    //! Raw name
    MaintenanceGroupName name_;
    //! Raw ID
    MaintenanceGroupName ID_;

    //! Weights for load, renewable and ror
    weightMap weights_;
    //! Type of the Maintenance Group
    ResidualLoadDefinitionType type_ = typeWeights;
    //! Enabled / Disabled
    bool enabled_ = true;

    void clear();
    void copyFrom(MaintenanceGroup const* original);

}; // class MaintenanceGroup

// class MntGrpList
struct CompareMaintenanceGroupName final
{
    bool operator()(const std::shared_ptr<MaintenanceGroup>& s1,
                    const std::shared_ptr<MaintenanceGroup>& s2) const
    {
        return s1->name() < s2->name();
    }
};

} // namespace Antares::Data

#include "MaintenanceGroup.hxx"