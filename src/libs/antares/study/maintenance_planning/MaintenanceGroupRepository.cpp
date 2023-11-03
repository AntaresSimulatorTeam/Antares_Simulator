//
// Created by milos on 1/11/23.
//

#include "MaintenanceGroupRepository.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "MaintenanceGroup.h"
#include <antares/study/study.h>
#include "MaintenanceGroupLoader.h"
#include "MaintenanceGroupSaver.h"

void Data::MaintenanceGroupRepository::clear()
{
    maintenanceGroups_.clear();
    activeMaintenanceGroups_.reset();
    scenariosLength_ = 0;
    scenariosNumber_ =0;
}

namespace Antares::Data
{
std::shared_ptr<Data::MaintenanceGroup> MaintenanceGroupRepository::find(const AnyString& id)
{
    for (auto const& i : maintenanceGroups_)
    {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

std::shared_ptr<const Data::MaintenanceGroup> MaintenanceGroupRepository::find(
  const AnyString& id) const
{
    for (const auto& i : maintenanceGroups_)
    {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

MaintenanceGroup* MaintenanceGroupRepository::findByName(const AnyString& name)
{
    for (auto const& i : maintenanceGroups_)
    {
        if (i->name() == name)
            return i.get();
    }
    return nullptr;
}

const MaintenanceGroup* MaintenanceGroupRepository::findByName(const AnyString& name) const
{
    for (const auto& i : maintenanceGroups_)
    {
        if (i->name() == name)
            return i.get();
    }
    return nullptr;
}

void MaintenanceGroupRepository::removeMaintenanceGroupsWhoseNameConstains(const AnyString& filter)
{
    NameContainsMnt pred(filter);
    maintenanceGroups_.erase(
      std::remove_if(maintenanceGroups_.begin(), maintenanceGroups_.end(), pred),
      maintenanceGroups_.end());
}

bool compareMaintenanceGroups(const std::shared_ptr<MaintenanceGroup>& s1,
                              const std::shared_ptr<MaintenanceGroup>& s2)
{
    if (s1->name() != s2->name())
    {
        return s1->name() < s2->name();
    }
    else
    {
        return s1->type() < s2->type();
    }
}

std::shared_ptr<MaintenanceGroup> MaintenanceGroupRepository::add(const AnyString& name)
{
    auto mnt = std::make_shared<MaintenanceGroup>();
    mnt->name(name);
    maintenanceGroups_.push_back(mnt);
    std::sort(maintenanceGroups_.begin(), maintenanceGroups_.end(), compareMaintenanceGroups);
    return mnt;
}

std::vector<std::shared_ptr<MaintenanceGroup>> MaintenanceGroupRepository::LoadMaintenanceGroup(
  EnvForLoading env)
{
    MaintenanceGroupLoader loader;
    return loader.load(std::move(env));
}

bool MaintenanceGroupRepository::saveToFolder(const AnyString& folder) const
{
    MaintenanceGroupSaver::EnvForSaving env;
    env.folder = folder;
    bool ret = internalSaveToFolder(env);
    env.folder = folder;
    ret = internalSaveScenariosToFolder(env) && ret;
    return ret;
}

bool MaintenanceGroupRepository::rename(MaintenanceGroup* mnt, const AnyString& newname)
{
    // Copy of the name
    MaintenanceGroup::MaintenanceGroupName name;
    name = newname;
    if (name == mnt->name())
        return true;
    MaintenanceGroup::MaintenanceGroupName id;
    Antares::TransformNameIntoID(name, id);
    if (std::any_of(maintenanceGroups_.begin(),
                    maintenanceGroups_.end(),
                    [&id](auto mntGroup) { return mntGroup->id() == id; }))
    {
        return false;
    }
    mnt->name(name);
    return true;
}

bool MaintenanceGroupRepository::loadFromFolder(Study& study,
                                                const StudyLoadOptions& options,
                                                const AnyString& folder)
{
    // do not load if below 870
    if (study.header.version < 870)
        return true;

    // Log entries
    logs.info(); // space for beauty
    logs.info() << "Loading maintenance groups...";

    // Cleaning
    clear();

    if (study.usedByTheSolver)
    {
        if (!study.parameters.maintenancePlanning.isOptimized())
        {
            logs.info()
              << "  The maintenance groups shall be ignored due to the advanced parameters";
            return true;
        }
    }

    EnvForLoading env(study.areas, study.header.version);
    env.folder = folder;

    env.iniFilename << env.folder << Yuni::IO::Separator << "maintenancegroups.ini";
    IniFile ini;
    if (!ini.open(env.iniFilename))
    {
        return false;
    }

    // For each section
    if (ini.firstSection)
    {
        for (env.section = ini.firstSection; env.section; env.section = env.section->next)
        {
            if (env.section->firstProperty)
            {
                auto new_mnt = LoadMaintenanceGroup(env);
                std::copy(new_mnt.begin(), new_mnt.end(), std::back_inserter(maintenanceGroups_));
            }
        }
    }

    // Logs
    if (maintenanceGroups_.empty())
        logs.info() << "No maintenance group found";
    else
    {
        std::sort(maintenanceGroups_.begin(), maintenanceGroups_.end(), compareMaintenanceGroups);

        if (maintenanceGroups_.size() == 1)
            logs.info() << "1 maintenance group found";
        else
            logs.info() << maintenanceGroups_.size() << " maintenance groups found";
    }

    return true;
}

bool MaintenanceGroupRepository::internalSaveToFolder(
  MaintenanceGroupSaver::EnvForSaving& env) const
{
    if (maintenanceGroups_.empty())
    {
        logs.info() << "No maintenance group to export.";
        if (!Yuni::IO::Directory::Create(env.folder))
            return false;
        // stripping the file
        env.folder << Yuni::IO::Separator << "maintenancegroups.ini";
        return Yuni::IO::File::CreateEmptyFile(env.folder);
    }

    if (maintenanceGroups_.size() == 1)
        logs.info() << "Exporting 1 maintenance group...";
    else
        logs.info() << "Exporting " << maintenanceGroups_.size() << " maintenance groups...";

    if (!Yuni::IO::Directory::Create(env.folder))
        return false;

    IniFile ini;
    bool ret = true;
    uint index = 0;
    auto end = maintenanceGroups_.end();
    Yuni::ShortString64 text;

    for (auto i = maintenanceGroups_.begin(); i != end; ++i, ++index)
    {
        text = index;
        env.section = ini.addSection(text);
        ret = Antares::Data::MaintenanceGroupSaver::saveToEnv(env, i->get()) && ret;
    }

    env.folder << Yuni::IO::Separator << "maintenancegroups.ini";
    return ini.save(env.folder) && ret;
}

bool MaintenanceGroupRepository::internalSaveScenariosToFolder(
  MaintenanceGroupSaver::EnvForSaving& env) const
{
    if (!Yuni::IO::Directory::Create(env.folder))
        return false;

    IniFile ini;
    std::string text = "ScenariosSettings";

    env.section = ini.addSection(text);
    env.section->add("Number", scenariosNumber_);
    env.section->add("Length", scenariosLength_);

    env.folder << Yuni::IO::Separator << "scenariossettings.ini";
    return ini.save(env.folder);
}

void MaintenanceGroupRepository::reverseWeightSign(const AreaLink* lnk)
{
}

uint64_t MaintenanceGroupRepository::memoryUsage() const
{
    uint64_t m = sizeof(MaintenanceGroupRepository);
    for (const auto& i : maintenanceGroups_)
        m += i->memoryUsage();
    return m;
}

namespace // anonymous
{
template<class T>
class RemovePredicate final
{
public:
    explicit RemovePredicate(const T* u) : pItem(u)
    {
    }

    bool operator()(const std::shared_ptr<MaintenanceGroup>& mnt) const
    {
        assert(mnt);
        if (mnt->contains(pItem))
        {
            logs.info() << "destroying the maintenance group " << mnt->name();
            return true;
        }
        return false;
    }

private:
    const T* pItem;
};

} // anonymous namespace

void MaintenanceGroupRepository::remove(const Area* area)
{
    RemovePredicate<Area> predicate(area);
    auto e = std::remove_if(maintenanceGroups_.begin(), maintenanceGroups_.end(), predicate);
    maintenanceGroups_.erase(e, maintenanceGroups_.end());
    activeMaintenanceGroups_.reset();
}

void MaintenanceGroupRepository::remove(const MaintenanceGroup* mnt)
{
    RemovePredicate<MaintenanceGroup> predicate(mnt);
    auto e = std::remove_if(maintenanceGroups_.begin(), maintenanceGroups_.end(), predicate);
    maintenanceGroups_.erase(e, maintenanceGroups_.end());
    activeMaintenanceGroups_.reset();
}

MaintenanceGroupRepository::iterator MaintenanceGroupRepository::begin()
{
    return maintenanceGroups_.begin();
}

MaintenanceGroupRepository::const_iterator MaintenanceGroupRepository::begin() const
{
    return maintenanceGroups_.begin();
}

MaintenanceGroupRepository::iterator MaintenanceGroupRepository::end()
{
    return maintenanceGroups_.end();
}

MaintenanceGroupRepository::const_iterator MaintenanceGroupRepository::end() const
{
    return maintenanceGroups_.end();
}

std::vector<std::shared_ptr<MaintenanceGroup>> MaintenanceGroupRepository::activeMaintenanceGroups()
  const
{
    if (activeMaintenanceGroups_)
    {
        return activeMaintenanceGroups_.value();
    }
    else
    {
        std::vector<std::shared_ptr<MaintenanceGroup>> out;
        std::copy_if(maintenanceGroups_.begin(),
                     maintenanceGroups_.end(),
                     std::back_inserter(out),
                     [](const auto& mnt) { return mnt->isActive(); });
        activeMaintenanceGroups_ = std::move(out);
        return activeMaintenanceGroups_.value();
    }
}

} // namespace Antares::Data
