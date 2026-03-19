// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__

namespace Yuni::Extension::CString
{
template<class CStringT>
class Append<CStringT, Antares::Data::StatisticalLaw>
{
public:
    static void Perform(CStringT& string, Antares::Data::StatisticalLaw law)
    {
        switch (law)
        {
        case Antares::Data::LawUniform:
            string += "uniform";
            break;
        case Antares::Data::LawGeometric:
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
class Into<Antares::Data::StatisticalLaw>
{
public:
    using TargetType = Antares::Data::StatisticalLaw;

    enum
    {
        valid = 1
    };

    static bool Perform(AnyString string, TargetType& out);

    template<class StringT>
    static TargetType Perform(const StringT& s)
    {
        TargetType law = Antares::Data::LawUniform;
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

} // namespace Yuni::Extension::CString

#endif // __ANTARES_LIBS_STUDY_PARTS_THERMAL_CLUSTER_HXX__
