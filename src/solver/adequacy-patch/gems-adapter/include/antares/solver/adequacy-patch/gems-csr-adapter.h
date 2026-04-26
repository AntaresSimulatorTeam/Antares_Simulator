// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <regex>
#include <string>
#include <vector>

#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/study/system-model/system.h>

namespace Antares::AdequacyPatch
{

enum class CsrRowSense
{
    LE,
    GE,
    EQ
};

struct CsrTerm
{
    int column;
    double coefficient;
};

struct CsrRow
{
    std::string constraintId;
    std::vector<CsrTerm> terms;
    double rhs;
    CsrRowSense sense;
};

class CsrProblemBuilder
{
public:
    virtual ~CsrProblemBuilder() = default;
    virtual int allocateColumn(const std::string& name, double lb, double ub) = 0;
};

struct CsrProblemContext
{
    const std::map<std::string, int>* areaToColumnIndex = nullptr;
    const Optimisation::LinearProblemApi::ILinearProblemData* dataSeries = nullptr;
};

class GemsCsrAdapter final
{
public:
    GemsCsrAdapter(const ModelerStudy::SystemModel::System& system,
                   const CsrProblemContext& csrCtx,
                   const std::regex& constraintFilter
                     = std::regex(R"(^flow_based_constraint_)"));

    void registerExtraVariables(CsrProblemBuilder& builder);
    std::vector<CsrRow> rowsForHour(int hour, int mcYear) const;
    int countMatchingConstraints() const;
    // Returns the number of extra LP columns registerExtraVariables() will allocate
    // (variables not already mapped to a CSR area column via portToAreaConnections).
    int countExtraVariables() const;

private:
    using VarKey = std::pair<std::string, unsigned int>; // (componentId, varIndex)

    struct LinearExpr
    {
        std::map<int, double> colTerms;
        double constant = 0.0;
    };

    void buildAreaVarMap();

    double resolveParameter(const ModelerStudy::SystemModel::Component& component,
                            const std::string& paramName,
                            unsigned int hour,
                            unsigned int tsNumber) const;

    LinearExpr evalExpr(const Expressions::Nodes::Node* node,
                        const ModelerStudy::SystemModel::Component& component,
                        unsigned int hour,
                        unsigned int tsNumber) const;

    bool buildRow(const Expressions::Nodes::Node* root,
                  const ModelerStudy::SystemModel::Component& component,
                  CsrRow& row,
                  unsigned int hour,
                  unsigned int tsNumber) const;

    const ModelerStudy::SystemModel::System& system_;
    CsrProblemContext csrCtx_;
    std::regex constraintFilter_;
    std::map<VarKey, int> varIdToColIdx_;
    bool extraVarsRegistered_ = false;
};

} // namespace Antares::AdequacyPatch