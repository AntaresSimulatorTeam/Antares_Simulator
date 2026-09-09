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
    virtual void setBoundsOnENS() = 0;
    virtual void setRHSfictitiousLoadValue() = 0;
    virtual void setRHSMaxEnsLoadValue() = 0;
};

// Null object — pure Legacy, does nothing
class NullGemsPart final: public IGemsPart
{
public:
    void setHour(int) override
    {
    }

    void setBoundsOnENS() override
    {
    }

    void setRHSfictitiousLoadValue() override
    {
    }

    void setRHSMaxEnsLoadValue() override
    {
    }
};

// Active object — hybrid Legacy + GEMS
class ActiveGemsPart final: public IGemsPart
{
public:
    ActiveGemsPart(PROBLEME_HEBDO* problemeHebdo,
                   PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre,
                   VariableManagement::VariableManager& variableManager,
                   std::map<int, int>& constraintCsrFictitiousLoad,
                   std::map<int, int>& constraintCsrMaxEnsLoad);

    void setHour(int triggeredHour) override;
    void setBoundsOnENS() override;
    void setRHSfictitiousLoadValue() override;
    void setRHSMaxEnsLoadValue() override;

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

    PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre_;
    VariableManagement::VariableManager& variableManager_;
    std::map<int, int>& constraintCsrFictitiousLoad_;
    std::map<int, int>& constraintCsrMaxEnsLoad_;
};

// Factory
std::unique_ptr<IGemsPart> makeGemsPart(PROBLEME_HEBDO* problemeHebdo,
                                        PROBLEME_ANTARES_A_RESOUDRE& problemeAResoudre,
                                        VariableManagement::VariableManager& variableManager,
                                        std::map<int, int>& constraintCsrFictitiousLoad,
                                        std::map<int, int>& constraintCsrMaxEnsLoad);
