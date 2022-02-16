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

#include "handler.h"
#include "../apply.h"
#include <antares/study/action/action.h>
#include <antares/study/action/text.h>
#include <antares/study/action/settings/suffix-for-area-names.h>
#include <antares/study/action/settings/suffix-for-cluster-names.h>
#include <antares/study/action/settings/suffix-for-constraint-names.h>
#include <antares/study/action/settings/decal-area-position.h>
#include <antares/study/action/handler/antares-study/area/create.h>
#include <antares/study/action/handler/antares-study/link/create.h>
#include <antares/study/action/handler/antares-study/constraint/create.h>
#include <set>
#include "build-context.h"
#include "../../jobs.h"
#include "../../../application/main.h"
#include "../../components/map/component.h"
#include "../../../windows/inspector.h"

using namespace Yuni;

namespace Antares
{
namespace ExtSource
{
namespace Handler
{
namespace // anonymous
{
/*!
** \brief Analyze a command for a paste operation
*/
static bool AppendCommand(BuildContext& ctx, const AnyString& command, const AnyString& value)
{
    // area
    if (command == "import-area")
    {
        Data::AreaName name = value;
        ctx.area.insert(name);
        ctx.shouldOverwriteArea = false;
        return true;
    }

    // thermal cluster
    if (command == "import-thermal-cluster")
    {
        Data::AreaName from;
        Data::ClusterName to;
        String s = value;
        if (s.size() < 3)
            return false;
        String::Size p = s.find('@');
        if (!p || p > s.size() - 2)
            return false;
        from.assign(s, p);
        to.assign(s, s.size() - p - 1, p + 1);
        ctx.cluster[from][to] = true;
        return true;
    }

    // link
    if (command == "import-link")
    {
        Data::AreaName from;
        Data::AreaName to;
        String s = value;
        if (s.size() < 3)
            return false;
        String::Size p = s.find('@');
        if (!p || p > s.size() - 2)
            return false;
        from.assign(s, p);
        to.assign(s, s.size() - p - 1, p + 1);
        ctx.link[from][to] = true;
        return true;
    }

    // modified when copied
    if (command == "was-modified")
    {
        ctx.modifiedWhenCopied = true;
        return true;
    }

    // constraint
    if (command == "import-constraint")
    {
        ctx.constraint.insert(value);
        return true;
    }
    return false;
}

bool checkConstraintSupportingElementsIntegrity(const Antares::Action::Context::Ptr context,
                                                const BuildContext* ctx)
{
    for (auto i = ctx->constraint.begin(); i != ctx->constraint.end(); ++i)
    {
        Data::BindingConstraint* bc = context->extStudy->bindingConstraints.findByName(*i);
        if (!bc)
            continue;

        Data::BindingConstraint::iterator endWeights = bc->end();
        uint count = bc->linkCount();
        for (Data::BindingConstraint::iterator j = bc->begin(); j != endWeights; ++j)
        {
            auto* areaLink = j->first;
            if (!areaLink)
                continue;

            auto from = ctx->link.find(areaLink->from->name);
            if (from != ctx->link.end())
            {
                auto with = from->second.find(areaLink->with->name);

                if (with != from->second.end() && with->second == true)
                {
                    --count;
                }
            }
            else
            {
                if (Antares::Window::Inspector::IsLinkSelected(areaLink->from->name,
                                                               areaLink->with->name))
                    --count;
            }
        }

        if (count != 0)
        {
            logs.error() << "Impossible to paste binding constraint: " << bc->name()
                         << ", missing supporting element";
            return false;
        }
    }

    return true;
}

bool checkLinkSupportingElementsIntegrity(const Antares::Action::Context::Ptr /* context */,
                                          const BuildContext* ctx)
{
    for (BuildContext::LinkSet::const_iterator i = ctx->link.begin(); i != ctx->link.end(); ++i)
    {
        Data::AreaName fromWantedName = i->first;

        for (std::map<Data::AreaName, bool>::const_iterator j = i->second.begin();
             j != i->second.end();
             ++j)
        {
            Data::AreaName toWantedName = j->first;

            // check wheither there is a selected area with the same name
            if (not Antares::Window::Inspector::isAreaSelected(fromWantedName)
                || not Antares::Window::Inspector::isAreaSelected(toWantedName))
            {
                if (ctx->area.find(fromWantedName) == ctx->area.end()
                    || ctx->area.find(toWantedName) == ctx->area.end())
                {
                    logs.error() << "Impossible to paste link: " << fromWantedName << "/"
                                 << toWantedName << ", missing supporting element";
                    return false;
                }
            }
        }
    }

    return true;
}

static void PreparePasteOperations(Antares::Action::Context::Ptr context,
                                   const String& text,
                                   uint offset,
                                   bool forceDialog)
{
    // assert
    assert(offset > 0 && offset < text.size());

    // The root action
    Antares::Action::IAction::Ptr root = new Antares::Action::Text<>("Paste from clipboard");

    // Context for preparing the action
    //
    // We have to keep temporary data for creation the action in order
    // to perform the paste.
    BuildContext ctx;

    // Another temporary variables
    String::Size begin = offset;
    bool stop = false;
    String rawCommand;
    CString<50, false> command;
    String param;

    // loop through all param for the handler
    do
    {
        String::Size end = text.find('\n', begin);
        if (end >= text.size())
        {
            end = text.size();
            stop = true;
        }
        if (end - 1 <= begin)
            continue;

        rawCommand.assign(text, end - begin, begin);

        String::Size point = rawCommand.find(':');
        if (point != String::npos)
        {
            command.assign(rawCommand, point);
            param.assign(rawCommand, rawCommand.size() - point - 1, point + 1);

            if (!AppendCommand(ctx, command, param))
                logs.warning() << "paste from clipboard: invalid command: " << command
                               << ", value = " << param;
        }

        // logs.info() << "rawCommand :: " << rawCommand;

        begin = end + 1;
    } while (!stop);

    if (context->extStudy->header.version != context->study.header.version)
    {
        logs.error() << "Impossible to paste data from a study with a different version number.";
        return;
    }

    if (not ctx.checkIntegrity(forceDialog))
        return;

    if (not checkConstraintSupportingElementsIntegrity(context, &ctx))
        return;

    if (not checkLinkSupportingElementsIntegrity(context, &ctx))
        return;

    if (ctx.modifiedWhenCopied && context->shouldDestroyExtStudy)
    {
        // We have to check that the study was not modified when copied
        // since we are re-loading the whole study, the data may not be up-to-date
        logs.error()
          << "Impossible to paste data from another instance. Please save the changes before.";
        return;
    }

    bool copyPosition = !ctx.shouldOverwriteArea;

    // Build the tree
    // Checking if the study comes from another folder
    if (context->extStudy->folder != context->study.folder)
    {
        ctx.shouldOverwriteArea = true;
        copyPosition = true;
    }
    else
    {
        if (ctx.checkIdentity())
            return;
    }

    // Settings
    {
        using namespace Antares::Action;
        auto* settings = new Text<false>("Settings");
        *settings += new Settings::SuffixAreaName("<auto>", !ctx.shouldOverwriteArea);
        *settings += new Settings::SuffixPlantName("<auto>", !ctx.shouldOverwriteArea);
        *settings += new Settings::SuffixConstraintName("<auto>", !ctx.shouldOverwriteArea);
        *settings += new Settings::DecalAreaPosition(!ctx.shouldOverwriteArea);
        *root += settings;
    }

    // Area first
    if (not ctx.area.empty())
    {
        // The root action
        auto* areaNode = new Antares::Action::Text<>("Areas");

        auto end = ctx.area.end();
        for (auto i = ctx.area.begin(); i != end; ++i)
        {
            const Data::AreaName& wantedName = ctx.forceAreaName[*i];
            copyPosition = (not wantedName);

            auto* create = (not wantedName)
                             ? new Antares::Action::AntaresStudy::Area::Create(*i)
                             : new Antares::Action::AntaresStudy::Area::Create(*i, wantedName);

            create->createActionsForAStandardAreaCopy(*context, copyPosition);
            *areaNode += create;
        }
        *root += areaNode;
    } // all areas

    // Thermal clusters
    if (not ctx.cluster.empty())
    {
        auto* clusterNode = new Antares::Action::Text<>("Thermal clusters");

        const BuildContext::LinkSet::const_iterator end = ctx.cluster.end();
        for (BuildContext::LinkSet::const_iterator i = ctx.cluster.begin(); i != end; ++i)
        {
            if (i->second.empty())
                continue;
            const Data::AreaName& wantedName = ctx.forceAreaName[i->first];
            Antares::Action::AntaresStudy::Area::Create* create;
            if (!wantedName)
                create = new Antares::Action::AntaresStudy::Area::Create(i->first);
            else
                create = new Antares::Action::AntaresStudy::Area::Create(i->first, wantedName);

            std::map<Data::ClusterName, bool>::const_iterator send = i->second.end();
            for (std::map<Data::ClusterName, bool>::const_iterator j = i->second.begin(); j != send;
                 ++j)
            {
                *create += Antares::Action::AntaresStudy::Area::Create::
                  StandardActionsToCopyThermalCluster(i->first, j->first);
            }
            *clusterNode += create;
        }
        *root += clusterNode;
    }

    // Links
    if (not ctx.link.empty())
    {
        auto* linkNode = new Antares::Action::Text<>("Links");
        bool empty = true;

        const BuildContext::LinkSet::const_iterator end = ctx.link.end();
        for (BuildContext::LinkSet::const_iterator i = ctx.link.begin(); i != end; ++i)
        {
            std::map<Data::AreaName, bool>::const_iterator send = i->second.end();

            Data::AreaName fromWantedName = ctx.forceAreaName[i->first];
            if (!fromWantedName)
                fromWantedName = i->first;

            for (std::map<Data::AreaName, bool>::const_iterator j = i->second.begin(); j != send;
                 ++j)
            {
                empty = false;

                Data::AreaName toWantedName = ctx.forceAreaName[j->first];
                if (!toWantedName)
                    toWantedName = j->first;

                // check wheither there is a selected link with the same name
                auto* create
                  = (not Antares::Window::Inspector::IsLinkSelected(fromWantedName, toWantedName))
                      ? new Antares::Action::AntaresStudy::Link::Create(i->first, j->first)
                      : new Antares::Action::AntaresStudy::Link::Create(
                        i->first, fromWantedName, j->first, toWantedName);

                create->createActionsForAStandardLinkCopy(*context);
                *linkNode += create;
            }
        }

        if (!empty)
            *root += linkNode;
        else
            delete linkNode;
    }

    // Constraints
    if (!ctx.constraint.empty())
    {
        // The root action
        auto* constraintNode = new Antares::Action::Text<>("Constraints");

        auto end = ctx.constraint.end();
        for (auto i = ctx.constraint.begin(); i != end; ++i)
        {
            auto* sourceConstraint = context->extStudy->bindingConstraints.findByName(*i);
            if (!sourceConstraint
                || (sourceConstraint->clusterCount() == 0 && sourceConstraint->linkCount() == 0))
                continue;
            // check wheither there is a selected binding constraint with the same name
            auto* create = (not Antares::Window::Inspector::isConstraintSelected(*i))
                             ? new Antares::Action::AntaresStudy::Constraint::Create(
                               *i, sourceConstraint->type(), sourceConstraint->operatorType())
                             : new Antares::Action::AntaresStudy::Constraint::Create(
                               *i, *i, sourceConstraint->type(), sourceConstraint->operatorType());

            create->createActionsForAStandardConstraintCopy(*context);
            *constraintNode += create;
        }
        *root += constraintNode;
    } // all areas

    // Post-Actions
    root->createPostActions(root);

    // Apply the actions
    Apply(context, root, forceDialog);
}

class JobOpenStudy final : public Toolbox::Jobs::Job
{
public:
    JobOpenStudy(const wxString& folder) :
     Toolbox::Jobs::Job(wxT("Opening a study"),
                        wxT("Gathering informations about the study"),
                        "images/32x32/open.png"),
     pFolder(folder),
     pStudy(NULL)
    {
    }
    //! Destructor
    virtual ~JobOpenStudy()
    {
    }

    void folder(const wxString& f)
    {
        pFolder = f;
    }

    Data::Study* study() const
    {
        return pStudy;
    }

protected:
    /*!
    ** \brief Load a study from a folder
    */
    virtual bool executeTask() override
    {
        // The folder
        Yuni::String sFl;
        wxStringToString(pFolder, sFl);

        {
            auto* study = new Data::Study(); // new study

            // Load all data
            Data::StudyLoadOptions options;
            options.loadOnlyNeeded = false;
            study->loadFromFolder(sFl, options);

            // Postflight
            logs.info();
            logs.info() << "PreAllocating all structures...";
            study->ensureDataAreAllInitialized();
            logs.info() << "The study is loaded.";
            pStudy = study;
        }

        // The loading of the study may disable the Just-In-Time mecanism
        // (to ensure compatibility with old studies)
        // So we have to re-enable it.
        JIT::enabled = true;

        // The task is complete
        return true;
    }

private:
    //! The folder where the study is located
    wxString pFolder;
    //! Our study
    Data::Study* pStudy;

}; // class JobOpenStudy

} // anonymous namespace

void AntaresStudy(Data::Study& target,
                  const String& content,
                  uint offset,
                  PropertyMap& map,
                  bool forceDialog)
{
    // The source seems to be an Antares Study
    // Two cases are possible: from the current opened study (the interface itself)
    //  or from another study, probably from another instance of Antares
    //
    // Note: The path may be empty if the study has not been saved yet
    // Source of the study :
    auto& path = map["path"];

    // update area name id set
    target.areas.updateNameIDSet();

    Map::Component* mainMap = Antares::Forms::ApplWnd::Instance()->map();
    size_t targetLayerID = 0;
    if (mainMap)
        targetLayerID = mainMap->getActiveLayerID();

    // Context for all actions
    auto context = std::make_shared<Antares::Action::Context>(target, targetLayerID);

    // If the path of the study where items should be extracted is the same than the current opened
    // study, we can directly use it
    if (!path || path == target.folder)
    {
        context->extStudy = &target;
        context->shouldDestroyExtStudy = false;

        PreparePasteOperations(context, content, offset, forceDialog);
    }
    else
    {
        // Otherwise, we have to open the other study before anything else
        auto* job = new JobOpenStudy(wxStringFromUTF8(path));
        job->run();

        if (not job->study())
        {
            return;
        }
        context->extStudy = job->study();
        context->shouldDestroyExtStudy = true;

        job->Destroy();

        PreparePasteOperations(context, content, offset, forceDialog);
    }

    // update area name id set for the external study too
    if (context->extStudy)
        context->extStudy->areas.updateNameIDSet();
}

} // namespace Handler
} // namespace ExtSource
} // namespace Antares
