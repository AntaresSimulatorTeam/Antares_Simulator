/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL 2.0
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__

namespace Yuni
{
namespace Extension
{
namespace CString
{
template<class CStringT>
class Append<CStringT, Antares::Data::ThermalLaw>
{
public:
    static void Perform(CStringT& string, Antares::Data::ThermalLaw law)
    {
        switch (law)
        {
        case Antares::Data::thermalLawUniform:
            string += "uniform";
            break;
        case Antares::Data::thermalLawGeometric:
            string += "geometric";
            break;
        }
    }
};

template<class CStringT>
class Append<CStringT, Antares::Data::CostGeneration>
{
public:
    static void Perform(CStringT& string, Antares::Data::CostGeneration costgeneration)
    {
        switch (costgeneration)
        {
        case Antares::Data::setManually:
            string += "setManually";
            break;
        case Antares::Data::useCostTimeseries:
            string += "useCostTimeseries";
            break;
        }
    }
};

template<class CStringT>
class Append<CStringT, Antares::Data::LocalTSGenerationBehavior>
{
public:
    static void Perform(CStringT& string, Antares::Data::LocalTSGenerationBehavior behavior)
    {
        switch (behavior)
        {
        case Antares::Data::LocalTSGenerationBehavior::forceGen:
            string += "force generation";
            break;
        case Antares::Data::LocalTSGenerationBehavior::forceNoGen:
            string += "force no generation";
            break;
        default:
            string += "use global";
            break;
        }
    }
};

template<>
class Into<Antares::Data::ThermalLaw>
{
public:
    using TargetType = Antares::Data::ThermalLaw;
    enum
    {
        valid = 1
    };

    static bool Perform(AnyString string, TargetType& out);

    template<class StringT>
    static TargetType Perform(const StringT& s)
    {
        TargetType law = Antares::Data::thermalLawUniform;
        Perform(s, law);
        return law;
    }
};

template<>
class Into<Antares::Data::CostGeneration>
{
public:
    using TargetType = Antares::Data::CostGeneration;
    enum class Validation
    {
        valid = 1
    };

    static bool Perform(AnyString string, TargetType& out);

    template<class StringT>
    static TargetType Perform(const StringT& s)
    {
        TargetType costgeneration = Antares::Data::setManually;
        Perform(s, costgeneration);
        return costgeneration;
    }
};

template<>
class Into<Antares::Data::LocalTSGenerationBehavior>
{
public:
    using TargetType = Antares::Data::LocalTSGenerationBehavior;
    enum
    {
        valid = 1
    };

    static bool Perform(AnyString string, TargetType& out);

    template<class StringT>
    static TargetType Perform(const StringT& s)
    {
        TargetType behavior = Antares::Data::LocalTSGenerationBehavior::useGlobalParameter;
        Perform(s, behavior);
        return behavior;
    }
};

} // namespace CString
} // namespace Extension
} // namespace Yuni

#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__
