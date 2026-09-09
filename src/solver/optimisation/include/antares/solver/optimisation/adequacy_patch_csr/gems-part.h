#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/study/system-model/portType.h>

struct PROBLEME_HEBDO;
class PROBLEME_ANTARES_A_RESOUDRE;

namespace Antares::ModelerStudy::SystemModel
{
struct AreaConnection;
}

namespace VariableManagement
{
class VariableManager;
}

// Interface
class IGemsPart
{
public:
    virtual ~IGemsPart() = default;
    virtual void setHour(int triggeredHour) = 0;
    virtual void setBoundsOnENS(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                VariableManagement::VariableManager& varManager)
      = 0;
    virtual void setRHSfictitiousLoadValue(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                           std::map<int, int>& constraintMap)
      = 0;
    virtual void setRHSMaxEnsLoadValue(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                       std::map<int, int>& constraintMap)
      = 0;
};

// Null object — pure Legacy, does nothing
class NullGemsPart final: public IGemsPart
{
public:
    void setHour(int) override
    {
    }

    void setBoundsOnENS(PROBLEME_ANTARES_A_RESOUDRE&, VariableManagement::VariableManager&) override
    {
    }

    void setRHSfictitiousLoadValue(PROBLEME_ANTARES_A_RESOUDRE&, std::map<int, int>&) override
    {
    }

    void setRHSMaxEnsLoadValue(PROBLEME_ANTARES_A_RESOUDRE&, std::map<int, int>&) override
    {
    }
};

// Active object — hybrid Legacy + GEMS
class ActiveGemsPart final: public IGemsPart
{
public:
    explicit ActiveGemsPart(PROBLEME_HEBDO* problemeHebdo);

    void setHour(int triggeredHour) override;
    void setBoundsOnENS(PROBLEME_ANTARES_A_RESOUDRE& problem,
                        VariableManagement::VariableManager& varManager) override;
    void setRHSfictitiousLoadValue(PROBLEME_ANTARES_A_RESOUDRE& problem,
                                   std::map<int, int>& constraintMap) override;
    void setRHSMaxEnsLoadValue(PROBLEME_ANTARES_A_RESOUDRE& problem,
                               std::map<int, int>& constraintMap) override;

private:
    double gemsContributionForArea(
      uint32_t area,
      const std::function<std::string(const Antares::ModelerStudy::SystemModel::AreaConnection&)>&
        getFieldId) const;
    double gemsUnsupEnergyForArea(uint32_t area) const;
    double gemsSpilledForArea(uint32_t area) const;

    PROBLEME_HEBDO* problemeHebdo_;
    int triggeredHour_ = 0;
    Antares::LinearProblem::Api::FillContext fillContext_{0, 0, 0, 0, 0};
};

// Factory
std::unique_ptr<IGemsPart> makeGemsPart(PROBLEME_HEBDO* problemeHebdo);
