#pragma once

#include <memory>
#include <string>

#include <antares/study/study.h>

class StudyFiller
{
public:
    class Area
    {
    public:
        Area& setName(const std::string& name)
        {
            this->name = name;
            return *this;
        }

        Area& setNodalOptimization(unsigned int nodalOptimization)
        {
            this->nodalOptimization = nodalOptimization;
            return *this;
        }

        Area& setUnsuppliedEnergyCost(double cost)
        {
            this->unsuppliedEnergyCost = cost;
            return *this;
        }

        Area& setSpilledEnergyCost(double cost)
        {
            this->spilledEnergyCost = cost;
            return *this;
        }

        // Area& setAdequacyPatchMode(AdequacyPatch::AdequacyPatchMode mode);
        friend class StudyFiller;

    private:
        std::string name;
        unsigned int nodalOptimization = 0;
        double unsuppliedEnergyCost = 0.;
        double spilledEnergyCost = 0.;
        // AdequacyPatch::AdequacyPatchMode adequacyPatchMode;
    };

    StudyFiller(Antares::Data::Study& study):
        study(study)
    {
    }

    StudyFiller& addArea(const Area& area)
    {
        areas.emplace_back(std::move(area));
        return *this;
    }

    void build()
    {
        for (const auto& area: areas)
        {
            auto toAdd = new Antares::Data::Area(area.name);
            toAdd->nodalOptimization = area.nodalOptimization;
            toAdd->thermal.unsuppliedEnergyCost = area.unsuppliedEnergyCost;
            toAdd->thermal.spilledEnergyCost = area.spilledEnergyCost;
            study.areas.add(toAdd);
        }
    }

private:
    Antares::Data::Study& study;
    std::vector<StudyFiller::Area> areas;
};
