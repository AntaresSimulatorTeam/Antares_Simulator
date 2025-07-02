#pragma once

#include <memory>
#include <string>

#include <antares/study/study.h>

class StudyBuilder
{
public:
    class AreaBuilder
    {
    public:
        AreaBuilder(StudyBuilder& builder, Antares::Data::AreaList& areas):
            builder(builder),
            areas(areas)
        {
        }

        AreaBuilder& setName(const std::string& name)
        {
            this->name = name;
            return *this;
        }

        AreaBuilder& setNodalOptimization(unsigned int nodalOptimization)
        {
            this->nodalOptimization = nodalOptimization;
            return *this;
        }

        AreaBuilder& setUnsuppliedEnergyCost(double cost)
        {
            this->unsuppliedEnergyCost = cost;
            return *this;
        }

        AreaBuilder& setSpilledEnergyCost(double cost)
        {
            this->spilledEnergyCost = cost;
            return *this;
        }

        StudyBuilder& add()
        {
            auto toAdd = new Antares::Data::Area(name);
            toAdd->nodalOptimization = nodalOptimization;
            toAdd->thermal.unsuppliedEnergyCost = unsuppliedEnergyCost;
            toAdd->thermal.spilledEnergyCost = spilledEnergyCost;

            areas.add(toAdd);
            return builder;
        }

        // Area& setAdequacyPatchMode(AdequacyPatch::AdequacyPatchMode mode);
        friend class StudyBuilder;

    private:
        std::string name;
        unsigned int nodalOptimization = 0;
        double unsuppliedEnergyCost = 0.;
        double spilledEnergyCost = 0.;
        // AdequacyPatch::AdequacyPatchMode adequacyPatchMode;
        StudyBuilder& builder;
        Antares::Data::AreaList& areas;
    };

    StudyBuilder(Antares::Data::Study& study):
        study(study),
        areaBuilder(*this, study.areas)
    {
    }

    AreaBuilder& addArea()
    {
        return areaBuilder;
    }

    void build()
    {
    }

private:
    Antares::Data::Study& study;
    AreaBuilder areaBuilder;
};
