// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <action/handler/antares-study/constraint/create.h>
#include <memory>
#include <antares/study/study.h>
#include "antares/study/fwd.h"
#include "data.h"
#include "comments.h"
#include "weights.h"
#include "offsets.h"
#include "enabled.h"
#include "antares/utils/utils.h"

using namespace Yuni;

namespace Antares::Action::AntaresStudy::Constraint
{
void Create::prepareSkipWL(Context& ctx)
{
    if (pTargetConstraintName.empty())
    {
        pTargetConstraintName = pOriginalConstraintName;
    }

    // area mapping
    ctx.constraint = nullptr;
    ctx.constraintForceCreate[pOriginalConstraintName] = false;

    pInfos.behavior = bhOverwrite;
}

bool Create::prepareWL(Context& ctx)
{
    pInfos.message.clear();

    ctx.area = nullptr;
    ctx.cluster = nullptr;
    ctx.link = nullptr;
    ctx.originalPlant = nullptr;

    bool explicitTarget = true;
    // Assign the area name if not already done
    // The variable pTargetConstraintName may be slighty different from the original
    // name (changed from the interface for example)
    if (pTargetConstraintName.empty())
    {
        pTargetConstraintName = pOriginalConstraintName;
        explicitTarget = false;
    }
    pFuturConstraintName = pTargetConstraintName;

    ctx.constraintForceCreate[pOriginalConstraintName] = false;

    // The name can not be empty
    if (pFuturConstraintName.empty())
    {
        pInfos.caption = "Unknown constraint";
        pInfos.message << "The name must not be empty";
        pInfos.state = stError;
        return false;
    }

    // Computing the futur ID of the area
    Antares::Data::ConstraintName id;
    std::shared_ptr<Antares::Data::BindingConstraint> constraintFound;

    // the suffix
    const String& suffix = ctx.property["constraint.name.suffix"];

    if (not suffix.empty() || !explicitTarget)
    {
        if (suffix == "<auto>" || !explicitTarget)
        {
            TransformNameIntoID(pFuturConstraintName, id);
            constraintFound = ctx.study->bindingConstraints.find(id);
            if (constraintFound)
            {
                Antares::Data::ConstraintName::Size sepPos = id.find_last_of('-');
                if (sepPos != YString::npos)
                {
                    Antares::Data::ConstraintName suffixChain(id, sepPos + 1);
                    int suffixNumber = suffixChain.to<int>();
                    if (suffixNumber > 0)
                    {
                        Antares::Data::ConstraintName suffixLess(pFuturConstraintName, 0, sepPos);
                        pFuturConstraintName = suffixLess;
                    }
                }
                unsigned int indx = 1;
                do
                {
                    ++indx;
                    pFuturConstraintName.clear() << pTargetConstraintName << "-" << indx;
                    id.clear();
                    TransformNameIntoID(pFuturConstraintName, id);
                    constraintFound = ctx.study->bindingConstraints.find(id);
                } while (constraintFound);
            }
        }
        else
        {
            pFuturConstraintName += suffix;
            TransformNameIntoID(pFuturConstraintName, id);
            constraintFound = ctx.study->bindingConstraints.find(id);
        }
    }
    else
    {
        TransformNameIntoID(pFuturConstraintName, id);
        constraintFound = ctx.study->bindingConstraints.find(id);
    }

    pInfos.caption.clear() << "Constraint " << pFuturConstraintName;
    // Default state
    pInfos.state = stReady;

    if (constraintFound && constraintFound->type() == pType)
    {
        // The area
        switch (pInfos.behavior)
        {
        case bhMerge:
            pInfos.message << "The constraint '" << pFuturConstraintName
                           << "' already exists and will remain untouched";
            break;
        case bhOverwrite:
            pInfos.message << "The constraint '" << pFuturConstraintName << "' will be reset";
            ctx.constraintForceCreate[pOriginalConstraintName] = true;
            break;
        default:
            break;
        }
    }
    else
    {
        // The area does not exist it will be created then
        pInfos.message << "The constraint '" << pFuturConstraintName << "' will be created";
        ctx.constraintForceCreate[pOriginalConstraintName] = true;
    }
    return true;
}

bool Create::performWL(Context& ctx)
{
    ctx.message.clear() << "Updating the constraint " << pFuturConstraintName;
    ctx.updateMessageUI(ctx.message);

    ctx.area = nullptr;
    ctx.cluster = nullptr;
    ctx.link = nullptr;
    ctx.originalPlant = nullptr;

    String id;
    TransformNameIntoID(pFuturConstraintName, id);
    ctx.constraint = ctx.study->bindingConstraints.find(id);

    // The area
    if (!ctx.constraint)
    {
        // create the area
        ctx.constraint = ctx.study->bindingConstraints.add(pFuturConstraintName);
        ctx.constraint->clearAndReset(pFuturConstraintName, pType, pOperator);
        logs.debug() << "[study-action] The constraint " << pFuturConstraintName
                     << " has been created";
    }
    else
    {
        logs.debug() << "[study-action] The constraint " << pFuturConstraintName
                     << " has been found";
        if (pInfos.behavior == bhOverwrite)
        {
            logs.debug() << "[study-action] The constraint " << pFuturConstraintName
                         << " has been reset";
            (ctx.constraint)->resetToDefaultValues();
        }
        ctx.constraint->clearAndReset(pFuturConstraintName, pType, pOperator);
        // ctx.constraint->type(pType);
    }
    ctx.autoselectConstraints.push_back(ctx.constraint);
    return (ctx.constraint != nullptr);
}

void Create::createActionsForAStandardConstraintCopy(Context&)
{
    *this += new Comments(pOriginalConstraintName);
    *this += new Enabled(pOriginalConstraintName);
    *this += new Weights(pOriginalConstraintName, pTargetConstraintName);
    *this += new Offsets(pOriginalConstraintName, pTargetConstraintName);
    *this += new Data(pOriginalConstraintName);
}

} // namespace Antares::Action::AntaresStudy::Constraint
