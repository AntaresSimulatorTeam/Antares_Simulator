// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0


#include <action/action.h>
#include "antares/antares/antares.h"

using namespace Yuni;

namespace Antares::Action
{
IAction::IAction():
    pInfos()
{
}

IAction::~IAction()
{
}

String IAction::caption() const
{
    ThreadingPolicy::MutexLocker locker(*this);
    return pInfos.caption;
}

String IAction::message() const
{
    ThreadingPolicy::MutexLocker locker(*this);
    return pInfos.message;
}

void IAction::behavior(Behavior newBehavior)
{
    ThreadingPolicy::MutexLocker locker(*this);
    pInfos.behavior = newBehavior;
}

void IAction::dump() const
{
    String tmp;
    tmp.reserve(512);
    internalDump(tmp, 0);
}

void IAction::internalDump(String& tmp, unsigned int level) const
{
    // displaying informations about this node
    {
        ThreadingPolicy::MutexLocker locker(*this);

        // some space
        tmp.resize(level * 6);
        tmp.fill(' ');
        std::cout << tmp;

        // The node itself
        if (leaf())
        {
            std::cout << "  . ";
        }
        else
        {
            std::cout << "[+] ";
        }
        std::cout << pInfos.caption << " (behavior: " << BehaviorToString(pInfos.behavior)
                  << ", state: " << StateToString(pInfos.state) << ")\n";

        // comments, if any
        if (!pInfos.message.empty())
        {
            std::cout << tmp << "    \"" << pInfos.message << "\"\n";
        }
    }

    // iterating over all children
    ++level;
    foreach (auto& child, *this)
    {
        child.internalDump(tmp, level);
    }
}

bool IAction::prepare(Context& ctx)
{
    bool r = true;
    ThreadingPolicy::MutexLocker locker(*this);
    if ((pParent && pParent->pInfos.state == stDisabled) || pInfos.behavior == bhSkip)
    {
        pInfos.state = stDisabled;
        prepareSkipWL(ctx);
        pInfos.message.clear();
    }
    else
    {
        r = prepareWL(ctx);
    }

    // iterating over all children
    foreach (auto& child, *this)
    {
        r = child.prepare(ctx) && r;
    }
    return r;
}

void IAction::registerViews(Context& ctx)
{
    ThreadingPolicy::MutexLocker locker(*this);
    registerViewsWL(ctx);
    foreach (auto& child, *this)
    {
        child.registerViews(ctx);
    }
}

bool IAction::prepareRootNode(Context& ctx)
{
    IAction::Ptr parent;
    {
        ThreadingPolicy::MutexLocker locker(*this);
        parent = pParent;
    }
    return (!parent) ? prepare(ctx) : parent->prepareRootNode(ctx);
}

bool IAction::perform(Context& ctx, bool recursive)
{
    bool r = true;
    {
        ThreadingPolicy::MutexLocker locker(*this);
        if (pInfos.state == stDisabled)
        {
            return true;
        }
        if (pInfos.state == stConflict || pInfos.state == stError)
        {
            return false;
        }
        if (pInfos.state != stNothingToDo)
        {
            logs.debug() << "[study-action] executing " << pInfos.caption;
            r = performWL(ctx);
        }
        else
        {
            logs.debug() << "[study-action] ignoring " << pInfos.caption;
        }
    }

    // iterating over all children
    if (recursive)
    {
        foreach (auto& child, *this)
        {
            r = child.perform(ctx) && r;
        }
    }
    return r;
}

bool IAction::prepareStack(IAction::Vector& vector)
{
    {
        ThreadingPolicy::MutexLocker locker(*this);
        if (pInfos.state == stDisabled)
        {
            return true;
        }
        if (pInfos.state == stConflict || pInfos.state == stError)
        {
            return false;
        }
        if (pInfos.state != stNothingToDo)
        {
            vector.push_back(this);
        }
    }
    // iterating over all children
    foreach (auto& child, *this)
    {
        if (not child.prepareStack(vector))
        {
            return false;
        }
    }
    return true;
}

void IAction::createPostActions(const IAction::Ptr& node)
{
    {
        ThreadingPolicy::MutexLocker locker(*this);
        createPostActionsWL(node);
    }

    // iterating over all children
    foreach (auto& child, *this)
    {
        child.createPostActions(node);
    }
}

void IAction::behaviorToText(Behavior behavior, String& out)
{
    switch (behavior)
    {
    case bhOverwrite:
        out = "overwrite";
        break;
    case bhMerge:
        out = "merge";
        break;
    case bhSkip:
        out = "skip";
        break;
    case bhMax:
        out.clear();
        break;
    }
}

} // namespace Antares::Action
