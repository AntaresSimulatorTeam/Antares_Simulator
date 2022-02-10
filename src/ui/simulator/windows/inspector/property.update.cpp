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

#include "grid.h"
#include <ui/common/lock.h>
#include <yuni/core/math.h>
#include <antares/study/filter.h>
#include <antares/study/scenario-builder/updater.hxx>
#include <antares/study/area/constants.h>
#include <ui/common/lock.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include "../../application/main.h"
#include "../../application/study.h"
#include "../../toolbox/components/map/component.h"
#include "../../toolbox/components/mainpanel.h"
#include <yuni/core/math.h>
#include <antares/study/filter.h>
#include <antares/study/scenario-builder/updater.hxx>
#include <antares/study/area/constants.h>
#include "../message.h"
#include "../../application/main/internal-ids.h"
#include "property.cluster.update.h"

using namespace Yuni;

#include "accumulator.hxx"

namespace Antares
{
namespace Window
{
namespace Inspector
{
bool InspectorGrid::onPropertyChanging_A(wxPGProperty*,
                                         const PropertyNameType& name,
                                         const wxVariant& value)
{
    InspectorData::Ptr& data = pCurrentSelection;
    if (!data)
        return false;

    // aliases
    auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
    auto& map = *mainFrm.map();

    auto end = data->areas.end();
    auto i = data->areas.begin();

    if (name == "area.name" && data->areas.size() == 1)
    {
        auto* area = *i;
        if (area)
        {
            Data::AreaName name;
            wxStringToString(value.GetString(), name);

            bool result = StudyRenameArea(area, name, &(data->study));
            return result;
        }
        return false;
    }
    if (name == "area.x")
    {
        map.moveNodeFromAreaX(*i, (int)value.GetLong());
        map.refresh();
        return true;
    }
    if (name == "area.y")
    {
        map.moveNodeFromAreaY(*i, (int)value.GetLong());
        map.refresh();
        return true;
    }
    if (name == "area.energy.unsupplied")
    {
        double d = Math::Round(value.GetDouble(), 3);
        for (; i != end; ++i)
            (*i)->thermal.unsuppliedEnergyCost = d;
        OnStudyNodalOptimizationChanged();
        return true;
    }
    if (name == "area.energy.spilled")
    {
        const double d = Math::Round(value.GetDouble(), 3);
        for (; i != end; ++i)
            (*i)->thermal.spilledEnergyCost = d;
        OnStudyNodalOptimizationChanged();
        return true;
    }
    if (name == "area.color")
    {
        wxColour color;
        color << value;
        for (; i != end; ++i)
        {
            (*i)->ui->color[0] = Math::MinMax<int>(color.Red(), 0, 255);
            (*i)->ui->color[1] = Math::MinMax<int>(color.Green(), 0, 255);
            (*i)->ui->color[2] = Math::MinMax<int>(color.Blue(), 0, 255);
            map.reimportNodeColors(*i);
        }
        map.refresh();
        OnStudyAreaColorChanged(nullptr);
        return true;
    }
    if (name == "area.resort.area.non_dispatch_power")
    {
        if (value.GetBool())
        {
            for (; i != end; ++i)
                (*i)->nodalOptimization |= Data::anoNonDispatchPower;
        }
        else
        {
            for (; i != end; ++i)
                (*i)->nodalOptimization &= ~Data::anoNonDispatchPower;
        }
        OnStudyNodalOptimizationChanged();
        return true;
    }
    if (name == "area.resort.area.dispatch_hydropower")
    {
        if (value.GetBool())
        {
            for (; i != end; ++i)
                (*i)->nodalOptimization |= Data::anoDispatchHydroPower;
        }
        else
        {
            for (; i != end; ++i)
                (*i)->nodalOptimization &= ~Data::anoDispatchHydroPower;
        }
        OnStudyNodalOptimizationChanged();
        return true;
    }
    if (name == "area.resort.area.other_dispatch_power")
    {
        if (value.GetBool())
        {
            for (; i != end; ++i)
                (*i)->nodalOptimization |= Data::anoOtherDispatchPower;
        }
        else
        {
            for (; i != end; ++i)
                (*i)->nodalOptimization &= ~Data::anoOtherDispatchPower;
        }
        OnStudyNodalOptimizationChanged();
        return true;
    }
    if (name == "area.adequacy_patch_mode")
    {
        auto* area = *i;
        if (area)
        {
            String s;
            wxStringToString(value.GetString(), s);
            s.toLower();
            s.trim();

            if (s == "virtual area" || s == "0")
                area->adequacyPatchMode = Data::adqmVirtualArea;
            else if (s == "physical area outside patch" || s == "1")
                area->adequacyPatchMode = Data::adqmPhysicalAreaOutsideAdqPatch;
            else if (s == "physical area inside patch" || s == "2")
                area->adequacyPatchMode = Data::adqmPhysicalAreaInsideAdqPatch;

            OnStudyAreaUseAdequacyPatchChanged();
            return true;
        }
        return false;
    }
    if (name == "area.links_count")
        return false;
    if (name == "area.cluster_count")
        return false;
    if (name.startsWith("area.filtering-synthesis."))
    {
        AnyString precision(name, 25);
        uint flag = Data::stringIntoDatePrecision(precision);
        if (!flag)
            return false;

        if (value.GetBool())
        {
            for (; i != end; ++i)
                (*i)->filterSynthesis |= flag;
        }
        else
        {
            for (; i != end; ++i)
                (*i)->filterSynthesis &= ~flag;
        }
        return true;
    }
    if (name.startsWith("area.filtering-year-by-year."))
    {
        AnyString precision(name, 28);
        uint flag = Data::stringIntoDatePrecision(precision);
        if (!flag)
            return false;

        if (value.GetBool())
        {
            for (; i != end; ++i)
                (*i)->filterYearByYear |= flag;
        }
        else
        {
            for (; i != end; ++i)
                (*i)->filterYearByYear &= ~flag;
        }
        return true;
    }
    return false;
}

bool InspectorGrid::onPropertyChanging_C(wxPGProperty*,
                                         const PropertyNameType& name,
                                         const wxVariant& value)
{
    // Reference to the current study
    InspectorData::Ptr& data = pCurrentSelection;
    auto& study = (!data) ? *Data::Study::Current::Get() : data->study;

    if (name == "common.study.name")
    {
        wxStringToString(value.GetString(), study.header.caption);
        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        mainFrm.mainPanel()->refreshFromStudy();
        return true;
    }
    if (name == "common.study.author")
    {
        wxStringToString(value.GetString(), study.header.author);
        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        mainFrm.mainPanel()->refreshFromStudy();
        return true;
    }
    return false;
}

bool InspectorGrid::onPropertyChanging_L(wxPGProperty*,
                                         const PropertyNameType& name,
                                         const wxVariant& value)
{
    InspectorData::Ptr& data = pCurrentSelection;
    if (!data)
        return false;

    auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
    auto end = data->links.end();
    auto i = data->links.begin();

    if (name == "link.name")
    {
        return false;
    }

    if (name == "link.color")
    {
        wxColour color;
        color << value;
        for (; i != end; ++i)
        {
            (*i)->color[0] = Math::MinMax<int>(color.Red(), 0, 255);
            (*i)->color[1] = Math::MinMax<int>(color.Green(), 0, 255);
            (*i)->color[2] = Math::MinMax<int>(color.Blue(), 0, 255);
            OnStudyLinkChanged(*i);
        }
        return true;
    }

    if (name == "link.style")
    {
        for (; i != end; ++i)
        {
            Data::StyleType style = (Data::StyleType)(int)value.GetChar();
            (*i)->style = style;
            OnStudyLinkChanged(*i);
        }
        return true;
    }

    if (name == "link.width")
    {
        for (; i != end; ++i)
        {
            (*i)->linkWidth = Math::MinMax((int)value.GetChar(), 1, 6);
            OnStudyLinkChanged(*i);
        }
        return true;
    }

    if (name == "link.hurdles_cost")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
        {
            (*i)->useHurdlesCost = v;
            OnStudyLinkChanged(*i);
        }
        return true;
    }
    if (name == "link.use_loop_flow")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
        {
            (*i)->useLoopFlow = v;
            OnStudyLinkChanged(*i);
        }
        return true;
    }
    if (name == "link.use_phase_shifter")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
        {
            (*i)->usePST = v;
            OnStudyLinkChanged(*i);
        }
        return true;
    }
    if (name == "link.transmission-capacities")
    {
        String s;
        wxStringToString(value.GetString(), s);
        s.toLower();
        s.trim();

        Data::TransmissionCapacities tc;
        if (s == "enabled")
            tc = Data::tncEnabled;
        else if (s == "set to null")
            tc = Data::tncIgnore;
        else if (s == "set to infinite")
            tc = Data::tncInfinite;
        else
            tc = Data::tncIgnore;

        for (; i != end; ++i)
        {
            (*i)->transmissionCapacities = tc;
            OnStudyLinkChanged(*i);
        }
        return true;
    }
    if (name == "link.asset-type")
    {
        String s;
        wxStringToString(value.GetString(), s);
        s.toLower();
        s.trim();

        Data::AssetType at;
        if (s == "ac")
            at = Data::atAC;
        else if (s == "dc")
            at = Data::atDC;
        else if (s == "gaz")
            at = Data::atGas;
        else if (s == "virt")
            at = Data::atVirt;
        else if (s == "other")
            at = Data::atOther;
        else
            at = Data::atOther;

        for (; i != end; ++i)
        {
            (*i)->assetType = at;
            OnStudyLinkChanged(*i);
        }
        return true;
    }
    if (name == "link.display_comments")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
        {
            (*i)->displayComments = v;
            OnStudyLinkChanged(*i);
        }
        mainFrm.map()->invalidate();
        mainFrm.map()->refresh();
        return true;
    }
    if (name == "link.comments")
    {
        String s;
        wxStringToString(value.GetString(), s);
        s.trim(" \t\n\r");
        for (; i != end; ++i)
        {
            (*i)->comments = s;
            OnStudyLinkChanged(*i);
        }
        mainFrm.map()->invalidate();
        mainFrm.map()->refresh();
        return true;
    }
    /*if (name == "link.style")
    {
            String s;
            wxStringToString(value.GetString(), s);
            s.toLower();
            s.trim();

            Data::StyleType st;
            if (s == "stPlain")
                    st = Data::stPlain;
            else if (s == "dc")
                    st = Data::atDC;
            else if (s == "other")
                    at = Data::atOther;
            else
                    at = Data::atOther;

            for (; i != end; ++i)
            {
                    (*i)->assetType = at;
                    OnStudyLinkChanged(*i);
            }
    }*/
    if (name.startsWith("link.filtering-synthesis."))
    {
        AnyString precision(name, 25);
        uint flag = Data::stringIntoDatePrecision(precision);
        if (!flag)
            return false;

        if (value.GetBool())
        {
            for (; i != end; ++i)
            {
                (*i)->filterSynthesis |= flag;
                OnStudyLinkChanged(*i);
            }
        }
        else
        {
            for (; i != end; ++i)
            {
                (*i)->filterSynthesis &= ~flag;
                OnStudyLinkChanged(*i);
            }
        }
        return true;
    }
    if (name.startsWith("link.filtering-year-by-year."))
    {
        AnyString precision(name, 28);
        uint flag = Data::stringIntoDatePrecision(precision);
        if (!flag)
            return false;

        if (value.GetBool())
        {
            for (; i != end; ++i)
            {
                (*i)->filterYearByYear |= flag;
                OnStudyLinkChanged(*i);
            }
        }
        else
        {
            for (; i != end; ++i)
            {
                (*i)->filterYearByYear &= ~flag;
                OnStudyLinkChanged(*i);
            }
        }
        return true;
    }

    return false;
}

bool InspectorGrid::onPropertyChanging_Constraint(wxPGProperty*,
                                                  const PropertyNameType& name,
                                                  const wxVariant& value)
{
    InspectorData::Ptr& data = pCurrentSelection;
    if (!data)
        return false;
    Data::BindingConstraint::Set::iterator end = data->constraints.end();
    Data::BindingConstraint::Set::iterator i = data->constraints.begin();

    if (name == "constraint.name")
    {
        return false;
    }
    if (name == "constraint.enabled")
    {
        if (value.GetBool())
        {
            for (; i != end; ++i)
                (*i)->enabled(true);
        }
        else
        {
            for (; i != end; ++i)
                (*i)->enabled(false);
        }
        return true;
    }
    if (name == "constraint.comments")
    {
        String s;
        wxStringToString(value.GetString(), s);
        s.trim(" \t\n\r");
        for (; i != end; ++i)
            (*i)->comments(s);
        return true;
    }

    if (name == "constraint.type")
        return false;

    return false;
}

bool InspectorGrid::onPropertyChanging_ThermalCluster(wxPGProperty*,
                                                      const PropertyNameType& name,
                                                      const wxVariant& value)
{
    InspectorData::Ptr& data = pCurrentSelection;
    if (!data)
        return false;

    ClusterUpdaterThermal updater(data, pFrame);

    if (name == "cluster.name")
    {
        return updater.changeName(value);
    }

    // Group
    if (name == "cluster.group")
    {
        return updater.changeGroup(value);
    }

    // unit
    if (name == "cluster.unit")
    {
        return updater.changeUnit(value);
    }

    if (name == "cluster.nominal_capacity")
    {
        updater.changeNominalCapacity(value);
        // apply check and colour
        // specific to thermal
        AccumulatorCheck<PClusterMinStablePowerColor>::ApplyTextColor(
          pFrame.pPGThClusterMinStablePower, data->ThClusters);
        AccumulatorCheck<PClusterNomCapacityColor>::ApplyTextColor(
          pFrame.pPGThClusterNominalCapacity, data->ThClusters);
        AccumulatorCheck<PClusterSpinningColor>::ApplyTextColor(pFrame.pPGThClusterSpinning,
                                                                data->ThClusters);
        // Notify
        OnStudyThermalClusterCommonSettingsChanged();
        return true;
    }

    if (name == "cluster.enabled")
    {
        return updater.changeEnabled(value);
    }

    // THERMAL-SPECIFIC PROPERTIES

    Data::ThermalCluster::Set::iterator end = data->ThClusters.end();
    Data::ThermalCluster::Set::iterator i = data->ThClusters.begin();

    // must-run
    if (name == "cluster.must-run")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
            (*i)->mustrun = v;
        return true;
    }

    if (name == "cluster.minstablepower")
    {
        const double d = value.GetDouble();
        if (d < 0.)
        {
            for (; i != end; ++i)
                (*i)->minStablePower = 0.;
        }
        else
        {
            for (; i != end; ++i)
                (*i)->minStablePower = d;
        }

        // pFrame.delayApply();
        AccumulatorCheck<PClusterMinStablePowerColor>::ApplyTextColor(
          pFrame.pPGThClusterMinStablePower, data->ThClusters);
        AccumulatorCheck<PClusterNomCapacityColor>::ApplyTextColor(
          pFrame.pPGThClusterNominalCapacity, data->ThClusters);
        AccumulatorCheck<PClusterSpinningColor>::ApplyTextColor(pFrame.pPGThClusterSpinning,
                                                                data->ThClusters);
        // Notify
        OnStudyThermalClusterCommonSettingsChanged();
        return true;
    }

    if (name == "cluster.minuptime" || name == "cluster.mindowntime")
    {
        long index = value.GetLong();
        uint d;
        switch (index)
        {
        case 0:
            d = 1;
            break;
        case 1:
            d = 24;
            break;
        case 2:
            d = 168;
            break;
        case 3:
            return false;
        default:
        {
            index -= 3;
            if (index < 1)
                d = 1;
            else
            {
                if (index > 168)
                    d = 168;
                else
                    d = static_cast<uint>(index);
            }
        }
        }
        for (; i != end; ++i)
        {
            if (name == "cluster.minuptime")
                (*i)->minUpTime = d;
            else if (name == "cluster.mindowntime")
                (*i)->minDownTime = d;
            else
                ;
        }
        return true;
    }
    if (name == "cluster.spinning")
    {
        const double d = value.GetDouble();
        if (d < 0.)
        {
            for (; i != end; ++i)
                (*i)->spinning = 0.;
            pFrame.delayApply();
        }
        else
        {
            for (; i != end; ++i)
                (*i)->spinning = d;
        }

        // pFrame.delayApply();

        // apply check and colour
        AccumulatorCheck<PClusterMinStablePowerColor>::ApplyTextColor(
          pFrame.pPGThClusterMinStablePower, data->ThClusters);
        AccumulatorCheck<PClusterNomCapacityColor>::ApplyTextColor(
          pFrame.pPGThClusterNominalCapacity, data->ThClusters);
        AccumulatorCheck<PClusterSpinningColor>::ApplyTextColor(pFrame.pPGThClusterSpinning,
                                                                data->ThClusters);
        // Notify
        OnStudyThermalClusterCommonSettingsChanged();
        return true;
    }
    if (name == "cluster.co2")
    {
        const double d = Math::Round(value.GetDouble(), 3);
        if (d < 0.)
        {
            for (; i != end; ++i)
                (*i)->co2 = 0.;
            pFrame.delayApply();
        }
        else
        {
            for (; i != end; ++i)
                (*i)->co2 = d;
        }
        return true;
    }

    // MBO 15/04/2014
    // New scheme when editing thermal clusters costs from the inspector
    // * Forbid values > 5.e4 and limit them to 5.e4
    // * Force values < 5.e-3 to 0 for
    //		- Marginal cost
    //		- Fixed cost
    //		- Satrup cost
    //		- Market Bid cost
    //		- Spread
    if (name == "cluster.opcost_marginal")
    {
        double d = value.GetDouble();

        // New scheme
        if (Math::Abs(d) < 5.e-3)
            d = 0.;
        else
        {
            if (d > 5.e4)
                d = 5.e4;
            else
            {
                if (d < -5.e4)
                    d = -5.e4;
                else
                    d = Math::Round(value.GetDouble(), 3);
            }
        }

        for (; i != end; ++i)
            (*i)->marginalCost = d;
        pFrame.delayApply();
        // Notify
        OnStudyThermalClusterCommonSettingsChanged();
        pFrame.Refresh();
        return true;
    }
    if (name == "cluster.opcost_fixed")
    {
        double d = value.GetDouble();

        // New scheme
        if (Math::Abs(d) < 5.e-3)
            d = 0.;
        else
        {
            if (d > 5.e4)
                d = 5.e4;
            else
            {
                if (d < -5.e4)
                    d = -5.e4;
                else
                    d = Math::Round(value.GetDouble(), 3);
            }
        }

        for (; i != end; ++i)
            (*i)->fixedCost = d;
        pFrame.delayApply();
        return true;
    }
    if (name == "cluster.opcost_startup")
    {
        double d = value.GetDouble();

        // New scheme
        // v5.0 format startup cost in [-5000000;5000000]
        if (Math::Abs(d) < 5.e-3)
            d = 0.;
        else
        {
            if (d > 5.e6)
                d = 5.e6;
            else
            {
                if (d < -5.e6)
                    d = -5.e6;
                else
                    d = Math::Round(value.GetDouble(), 3);
            }
        }

        for (; i != end; ++i)
            (*i)->startupCost = d;
        pFrame.delayApply();
        return true;
    }
    if (name == "cluster.opcost_marketbid")
    {
        double d = value.GetDouble();

        // New scheme
        if (Math::Abs(d) < 5.e-3)
            d = 0.;
        else
        {
            if (d > 5.e4)
                d = 5.e4;
            else
            {
                if (d < -5.e4)
                    d = -5.e4;
                else
                    d = Math::Round(value.GetDouble(), 3);
            }
        }

        for (; i != end; ++i)
            (*i)->marketBidCost = d;
        pFrame.delayApply();

        // Notify
        OnStudyThermalClusterCommonSettingsChanged();
        pFrame.Refresh();
        return true;
    }
    if (name == "cluster.opcost_spread")
    {
        double d = value.GetDouble();
        if (d < 5.e-3)
        {
            for (; i != end; ++i)
                (*i)->spreadCost = 0.;
            pFrame.delayApply();
        }
        else
        {
            if (d > 5.e4)
                d = 5.e4;

            for (; i != end; ++i)
                (*i)->spreadCost = d;
            pFrame.delayApply();
        }
        return true;
    }
    if (name == "cluster.forcedvolatility")
    {
        double d = value.GetDouble();

        if (d < 0. || d > 1.)
        {
            if (d < 0.)
                d = 0.;
            else if (d > 1.)
                d = 1.;

            for (; i != end; ++i)
                (*i)->forcedVolatility = d;
            pFrame.delayApply();
        }
        else
        {
            for (; i != end; ++i)
                (*i)->forcedVolatility = d;
        }
        return true;
    }
    if (name == "cluster.plannedvolatility")
    {
        double d = value.GetDouble();

        if (d < 0. || d > 1.)
        {
            if (d < 0.)
                d = 0.;
            else if (d > 1.)
                d = 1.;

            for (; i != end; ++i)
                (*i)->plannedVolatility = d;
            pFrame.delayApply();
        }
        else
        {
            for (; i != end; ++i)
                (*i)->plannedVolatility = d;
        }
        return true;
    }

    if (name == "cluster.forcedlaw")
    {
        long index = value.GetLong();
        Data::ThermalLaw law = Data::thermalLawUniform;

        switch (index)
        {
        case 0:
            law = Data::thermalLawUniform;
            break;
        case 1:
            law = Data::thermalLawGeometric;
            break;
        default:
            return false;
        }
        for (; i != end; ++i)
            (*i)->forcedLaw = law;
        return true;
    }

    if (name == "cluster.plannedlaw")
    {
        long index = value.GetLong();
        Data::ThermalLaw law = Data::thermalLawUniform;

        switch (index)
        {
        case 0:
            law = Data::thermalLawUniform;
            break;
        case 1:
            law = Data::thermalLawGeometric;
            break;
        default:
            return false;
        }
        for (; i != end; ++i)
            (*i)->plannedLaw = law;
        return true;
    }

    if (name == "cluster.gen-ts")
    {
        long index = value.GetLong();

        Data::LocalTSGenerationBehavior behavior
          = Data::LocalTSGenerationBehavior::useGlobalParameter;

        switch (index)
        {
        case 0:
            behavior = Data::LocalTSGenerationBehavior::useGlobalParameter;
            break;
        case 1:
            behavior = Data::LocalTSGenerationBehavior::forceGen;
            break;
        case 2:
            behavior = Data::LocalTSGenerationBehavior::forceNoGen;
            break;
        default:
            return false;
        }

        for (; i != end; ++i)
            (*i)->tsGenBehavior = behavior;

        return true;
    }

    return false;
}

bool InspectorGrid::onPropertyChanging_RenewableClusters(const PropertyNameType& name,
                                                         const wxVariant& value)
{
    using namespace Data;
    InspectorData::Ptr& data = pCurrentSelection;
    if (!data)
        return false;

    ClusterUpdaterRenewable updater(data, pFrame);

    if (name == "rn-cluster.name")
    {
        return updater.changeName(value);
    }

    // Group
    if (name == "rn-cluster.group")
    {
        return updater.changeGroup(value);
    }

    // unit
    if (name == "rn-cluster.unit")
    {
        return updater.changeUnit(value);
    }
    if (name == "rn-cluster.nominal_capacity")
    {
        return updater.changeNominalCapacity(value);
    }

    if (name == "rn-cluster.enabled")
    {
        return updater.changeEnabled(value);
    }

    // RENEWABLE-SPECIFIC PROPERTY
    RenewableCluster::Set::iterator end = data->RnClusters.end();
    RenewableCluster::Set::iterator i = data->RnClusters.begin();

    if (name == "rn-cluster.ts_mode")
    {
        long index = value.GetLong();
        Data::RenewableCluster::TimeSeriesMode tsMode = Data::RenewableCluster::powerGeneration;
        switch (index)
        {
        case 0:
            tsMode = Data::RenewableCluster::powerGeneration;
            break;
        case 1:
            tsMode = Data::RenewableCluster::productionFactor;
            break;
        default:
            return false;
        }
        for (; i != end; ++i)
            (*i)->tsMode = tsMode;

        OnStudyRenewableClusterCommonSettingsChanged();
        return true;
    }
    return false;
}

bool InspectorGrid::onPropertyChanging_S(wxPGProperty*,
                                         const PropertyNameType& name,
                                         const wxVariant& value)
{
    InspectorData::Ptr& data = pCurrentSelection;
    if (!data)
        return false;

    Data::Study::Set::iterator end = data->studies.end();
    Data::Study::Set::iterator i = data->studies.begin();

    if (name == "study.years")
    {
        uint years = value.GetLong();
        years = Math::MinMax(years, 1u, 50000u);
        bool error = false;
        for (; i != end; ++i)
        {
            // As long as we modify the number of years, we should
            // update the scenario builder as well
            ScenarioBuilderUpdater updater(*(*i));

            if ((*i)->parameters.derated && years != 1)
            {
                error = true;
                (*i)->parameters.years(1);
            }
            else
                (*i)->parameters.years(years);
        }
        if (error)
        {
            logs.warning() << "The derated mode is enabled. The number of years can not be changed";
            pFrame.delayApply();
        }
        else
        {
            if (years != value.GetLong())
                pFrame.delayApply();
        }

        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.yearbyyear")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
            (*i)->parameters.yearByYear = v;
        OnStudySimulationSettingsChanged();
        if (v)
            pFrame.delayApply();
        return true;
    }
    if (name == "study.simsynthesis")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
            (*i)->parameters.synthesis = v;
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.mode")
    {
        String s;
        wxStringToString(value.GetString(), s);
        s.toLower();

        Data::StudyMode mode = Data::stdmUnknown;
        if (s == "economy" || s == "0")
            mode = Data::stdmEconomy;
        else if (s == "adequacy" || s == "1")
            mode = Data::stdmAdequacy;
        else if (s == "draft" || s == "2")
            mode = Data::stdmAdequacyDraft;
        else
            mode = Data::stdmEconomy;

        for (; i != end; ++i)
        {
            (*i)->parameters.mode = mode;
            // For adequacy-draft mode, forcing the time interval to the whole year
            if (mode == Data::stdmAdequacyDraft)
            {
                auto& parameters = (*i)->parameters;
                parameters.simulationDays.first = 0;
                parameters.simulationDays.end = 365;
                pFrame.delayApply();
            }
            OnStudySimulationSettingsChanged();
        }
        return true;
    }

    if (name == "study.horizon")
    {
        String s;
        wxStringToString(value.GetString(), s);
        for (; i != end; ++i)
            (*i)->parameters.horizon = s;
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.cal.begin")
    {
        uint s = static_cast<uint>(value.GetLong());
        uint d = s;
        if (d > 0)
            --d;
        uint v = Math::MinMax(d, 0u, 364u);
        for (; i != end; ++i)
            (*i)->parameters.simulationDays.first = v;
        // Accumulator<PStudyCalBegin> :: Apply(pFrame.pPGStudyCalendarBegin, data->studies);
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.cal.end")
    {
        uint d = static_cast<uint>(value.GetLong());
        uint v = Math::MinMax(d, 1u, 365u);
        for (; i != end; ++i)
            (*i)->parameters.simulationDays.end = v;
        // Accumulator<PStudyCalEnd> :: Apply(pFrame.pPGStudyCalendarEnd, data->studies);
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.playlist")
    {
        uint d = value.GetLong();
        switch (d)
        {
        case 0: // automatic
        {
            for (; i != end; ++i)
                (*i)->parameters.userPlaylist = false;
            return true;
        }
        case 1: // manual
        {
            bool error = false;
            for (; i != end; ++i)
            {
                if ((*i)->parameters.nbYears < 2)
                    error = true;
                else
                    (*i)->parameters.userPlaylist = true;
            }
            if (error)
            {
                Window::Message message(
                  this,
                  wxT("Selection Mode"),
                  wxT("MC Playlist"),
                  wxT("Impossible to enable the MC playlist with only 1 year"));
                message.add(Window::Message::btnContinue, true);
                message.showModal();
                return false;
            }
            return true;
        }
        }
        return false;
    }

    if (name == "study.buildingmode")
    {
        // Gets menu Configure's scenario builder item
        auto& mainFrm = *Antares::Forms::ApplWnd::Instance();
        auto* menu = mainFrm.GetMenuBar();
        auto* sc_builder_menu_item
          = menu->FindItem(Antares::Forms::mnIDOptionConfigureMCScenarioBuilder);

        uint d = value.GetLong();
        switch (d)
        {
        case 0: // automatic
        {
            for (; i != end; ++i)
            {
                (*i)->parameters.derated = false;
                (*i)->parameters.useCustomScenario = false;
            }

            if (sc_builder_menu_item)
                sc_builder_menu_item->Enable(false);

            return true;
        }
        case 1: // manual
        {
            for (; i != end; ++i)
            {
                (*i)->parameters.derated = false;
                (*i)->parameters.useCustomScenario = true;
            }

            if (sc_builder_menu_item)
                sc_builder_menu_item->Enable(true);

            return true;
        }
        case 2: // derated
        {
            bool shouldReloadNbYears = false;
            bool shouldReloadPlaylist = false;
            for (; i != end; ++i)
            {
                if (1u != (*i)->parameters.nbYears)
                {
                    shouldReloadNbYears = true;
                    ScenarioBuilderUpdater updater(*(*i));
                    (*i)->parameters.derated = true;
                    (*i)->parameters.useCustomScenario = false;
                    (*i)->parameters.nbYears = 1u;
                    // scenario build updater
                }
                else
                {
                    // As long as we modify the number of years, we should
                    // update the scenario builder as well
                    (*i)->parameters.derated = true;
                    (*i)->parameters.useCustomScenario = false;
                }

                // in derated mode, the user playlist should be disabled since
                // there is only one MC year
                if ((*i)->parameters.userPlaylist)
                {
                    (*i)->parameters.userPlaylist = false;
                    shouldReloadPlaylist = true;
                }
            }

            if (sc_builder_menu_item)
                sc_builder_menu_item->Enable(false);

            // Updating the number of years
            if (shouldReloadNbYears)
                Accumulator<PStudyYears>::Apply(pFrame.pPGStudyYears, data->studies);
            if (shouldReloadPlaylist)
                Accumulator<PStudyPlaylist>::Apply(pFrame.pPGStudyPlaylist, data->studies);
            return true;
        }
        }
        return false;
    }
    if (name == "study.1rsjanuary")
    {
        uint d = Math::MinMax<uint>(value.GetLong(), 0, 6);
        for (; i != end; ++i)
            (*i)->parameters.dayOfThe1stJanuary = (Antares::DayOfTheWeek)d;
        StudyRefreshCalendar();
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.leap")
    {
        bool v = value.GetBool();
        for (; i != end; ++i)
            (*i)->parameters.leapYear = v;
        StudyRefreshCalendar();
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.calendarweek")
    {
        uint d = Math::MinMax<uint>(value.GetLong(), 0, 6);
        for (; i != end; ++i)
            (*i)->parameters.firstWeekday = (Antares::DayOfTheWeek)d;
        StudyRefreshCalendar();
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.calendarmonth")
    {
        uint d = Math::MinMax<uint>(value.GetLong(), 0, 11);
        for (; i != end; ++i)
            (*i)->parameters.firstMonthInYear = (Antares::MonthName)d;
        StudyRefreshCalendar();
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.stochasticts")
    {
        const bool v = value.GetBool();
        if (!v)
        {
            for (; i != end; ++i)
                (*i)->parameters.timeSeriesToArchive = 0;
            OnStudySimulationSettingsChanged();
            return true;
        }
        return false;
    }
    if (name == "study.scenarios")
    {
        const bool v = value.GetBool();
        for (; i != end; ++i)
            (*i)->parameters.storeTimeseriesNumbers = v;
        OnStudySimulationSettingsChanged();
        return true;
    }
    if (name == "study.geographictrimming")
    {
        uint d = value.GetLong();
        switch (d)
        {
        case 0: // none
        {
            for (; i != end; ++i)
                (*i)->parameters.geographicTrimming = false;
            OnStudySimulationSettingsChanged();
            return true;
        }
        case 1: // custom
        {
            for (; i != end; ++i)
                (*i)->parameters.geographicTrimming = true;
            OnStudySimulationSettingsChanged();
            return true;
        }
        }
        return false;
    }
    if (name == "study.thematictrimming")
    {
        uint d = value.GetLong();
        switch (d)
        {
        case 0: // none
        {
            for (; i != end; ++i)
                (*i)->parameters.thematicTrimming = false;
            OnStudySimulationSettingsChanged();
            return true;
        }
        case 1: // custom
        {
            for (; i != end; ++i)
                (*i)->parameters.thematicTrimming = true;
            OnStudySimulationSettingsChanged();
            return true;
        }
        }
        return false;
    }
    return false;
}

void InspectorGrid::OnPropertyChanging(wxPropertyGridEvent& event)
{
    wxPGProperty* property = event.GetProperty();
    if (!property)
        return;
    wxVariant value = event.GetValue();
    // Make sure value is not unspecified
    if (value.IsNull())
        return;

    // PropertyNameType propName;
    PropertyNameType propName;
    wxStringToString(property->GetName(), propName);
    if (propName.size() < 2)
        return;

    GUILocker guilocker;
    propName.toLower();
    logs.debug() << "  updated the property '" << propName << "'";

    bool result = true;
    switch (propName[0])
    {
    case 'a':
        result = onPropertyChanging_A(property, propName, value);
        break;
    case 'c':
    {
        switch (propName[1])
        {
        case 'o':
        {
            // constraint
            switch (propName[2])
            {
            case 'n':
                result = onPropertyChanging_Constraint(property, propName, value);
                break;
            case 'm':
                result = onPropertyChanging_C(property, propName, value);
                break;
            }
            break;
        }
        case 'l':
            // cluster
            result = onPropertyChanging_ThermalCluster(property, propName, value);
        }
        break;
    }
    case 'l':
        result = onPropertyChanging_L(property, propName, value);
        break;
    case 'r':
        result = onPropertyChanging_RenewableClusters(propName, value);
        break;
    case 's':
        result = onPropertyChanging_S(property, propName, value);
        break;
    default:
        break;
    }
    if (!result)
    {
        event.Veto();
        event.SetValidationFailureBehavior(0);
    }
    else
    {
        MarkTheStudyAsModified();
        // Broadcast the good news to the other inspector, because
        // they may have a selection in common
        OnInspectorRefresh(&pFrame);
    }
}

} // namespace Inspector
} // namespace Window
} // namespace Antares
