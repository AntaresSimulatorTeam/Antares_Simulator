//
// Created by marechaljas on 11/05/23.
//

#include "BindingConstraintsList.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "BindingConstraint.h"
#include "antares/study.h"
#include "BindingConstraintLoader.h"

namespace Antares::Data {
std::shared_ptr<Data::BindingConstraint> BindingConstraintsList::find(const AnyString &id) {
    for (auto const &i: pList) {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

std::shared_ptr<const Data::BindingConstraint> BindingConstraintsList::find(const AnyString &id) const {
    for (const auto & i : pList) {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

BindingConstraint *BindingConstraintsList::findByName(const AnyString &name) {
    for (uint i = 0; i != (uint) pList.size(); ++i) {
        if (pList[i]->name() == name)
            return pList[i].get();
    }
    return nullptr;
}

const BindingConstraint *BindingConstraintsList::findByName(const AnyString &name) const {
    for (uint i = 0; i != (uint) pList.size(); ++i) {
        if (pList[i]->name() == name)
            return pList[i].get();
    }
    return nullptr;
}

void BindingConstraintsList::removeConstraintsWhoseNameConstains(const AnyString &filter) {
    WhoseNameContains pred(filter);
    pList.erase(std::remove_if(pList.begin(), pList.end(), pred), pList.end());
}

bool compareConstraints(const std::shared_ptr<BindingConstraint>& s1, const std::shared_ptr<BindingConstraint>& s2) {
    return s1->name() < s2->name();
}

std::shared_ptr<BindingConstraint> BindingConstraintsList::add(const AnyString &name) {
    auto bc = std::make_shared<BindingConstraint>();
    bc->name(name);
    pList.push_back(bc);
    std::sort(pList.begin(), pList.end(), compareConstraints);
    return bc;
}

void BindingConstraintsList::resizeAllTimeseriesNumbers(unsigned int nb_years) {
    std::for_each(time_series_numbers.begin(), time_series_numbers.end(), [&](auto &kvp) {
        time_series_numbers[kvp.first].timeseriesNumbers.clear();
        time_series_numbers[kvp.first].timeseriesNumbers.resize(1, nb_years);
    });
}

void BindingConstraintsList::fixTSNumbersWhenWidthIsOne() {
    std::map<std::string, bool, std::less<>> groupOfOneTS;
    std::for_each(pList.begin(), pList.end(), [&groupOfOneTS](auto bc) {
        auto hasOneTs = bc->TimeSeries().width == 1;
        if (groupOfOneTS[bc->group()] && !hasOneTs) {
            assert(false && ("Group of binding constraints mixing 1TS and N TS group:" + bc->group()).c_str());
        }
        groupOfOneTS[bc->group()] |= hasOneTs;
    });
    std::for_each(time_series_numbers.begin(), time_series_numbers.end(),
                  [&groupOfOneTS](std::pair<std::string, BindingConstraintTimeSeriesNumbers> it) {
                      if (groupOfOneTS[it.first]) {
                          it.second.timeseriesNumbers.fillColumn(0, 0);
                      }
                  });
}

unsigned int BindingConstraintsList::NumberOfTimeseries(const std::string& group_name) const {
    //Assume all BC in a group have the same width
    const auto binding_constraint = std::find_if(pList.begin(), pList.end(), [&group_name](auto bc) {
        return bc->group() == group_name;
    });
    if (binding_constraint == pList.end())
        return 0;
    return (*binding_constraint)->TimeSeries().width;
}

std::vector<std::shared_ptr<BindingConstraint>>
BindingConstraintsList::LoadBindingConstraint(EnvForLoading env, uint years) {
    const BindingConstraintLoader bc_loader;
    return bc_loader.load(std::move(env));
}

bool BindingConstraintsList::saveToFolder(const AnyString &folder) const {
    BindingConstraint::EnvForSaving env;
    env.folder = folder;
    return internalSaveToFolder(env);
}

bool BindingConstraintsList::rename(BindingConstraint *bc, const AnyString &newname) {
    // Copy of the name
    ConstraintName name;
    name = newname;
    if (name == bc->name())
        return true;
    ConstraintName id;
    Antares::TransformNameIntoID(name, id);
    if (std::any_of(pList.begin(), pList.end(), [&id](auto constraint) {
            return constraint->id() == id;
        }))
    {
        return false;
    }
    bc->name(name);
    //TODO
    //JIT::Invalidate(bc->matrix().jit);
    return true;
}

bool BindingConstraintsList::loadFromFolder(Study &study,
                                            const StudyLoadOptions &options,
                                            const AnyString &folder) {
    // Log entries
    logs.info(); // space for beauty
    logs.info() << "Loading constraints...";

    // Cleaning
    clear();

    if (study.usedByTheSolver) {
        if (options.ignoreConstraints) {
            logs.info() << "  The constraints have been disabled by the user";
            return true;
        }
        if (!study.parameters.include.constraints) {
            logs.info() << "  The constraints shall be ignored due to the optimization preferences";
            return true;
        }
    }

    EnvForLoading env(study.areas, study.header.version);
    env.folder = folder;

    env.iniFilename << env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
    IniFile ini;
    if (!ini.open(env.iniFilename)) {
        return false;
    }

    // For each section
    if (ini.firstSection) {
        for (env.section = ini.firstSection; env.section; env.section = env.section->next) {
            if (env.section->firstProperty) {
               auto new_bc = LoadBindingConstraint(env, study.parameters.nbYears);
                std::copy(new_bc.begin(), new_bc.end(), std::back_inserter(pList));
            }
        }
    }

    // Logs
    if (pList.empty())
        logs.info() << "No binding constraint found";
    else
    {
        std::sort(pList.begin(), pList.end(), compareConstraints);

        if (pList.size() == 1)
            logs.info() << "1 binding constraint found";
        else
            logs.info() << pList.size() << " binding constraints found";
    }

    // When ran from the solver and if the simplex is in `weekly` mode,
    // all weekly constraints will become daily ones.
    if (study.usedByTheSolver && sorDay == study.parameters.simplexOptimizationRange) {
        mutateWeeklyConstraintsIntoDailyOnes();
    }
    //TODO load time series numbers
    return true;
}

void BindingConstraintsList::mutateWeeklyConstraintsIntoDailyOnes()
{
    each([](BindingConstraint &constraint) {
        if (constraint.type() == BindingConstraint::typeWeekly)
        {
            logs.info() << "  The type of the constraint '" << constraint.name()
                        << "' is now 'daily'";
            constraint.mutateTypeWithoutCheck(BindingConstraint::typeDaily);
        }
    });
}

bool BindingConstraintsList::internalSaveToFolder(BindingConstraint::EnvForSaving& env) const
{
    if (pList.empty())
    {
        logs.info() << "No binding constraint to export.";
        if (!IO::Directory::Create(env.folder))
            return false;
        // stripping the file
        env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
        return IO::File::CreateEmptyFile(env.folder);
    }

    if (pList.size() == 1)
        logs.info() << "Exporting 1 binding constraint...";
    else
        logs.info() << "Exporting " << pList.size() << " binding constraints...";

    if (!IO::Directory::Create(env.folder))
        return false;

    IniFile ini;
    bool ret = true;
    uint index = 0;
    auto end = pList.end();
    ShortString64 text;

    for (auto i = pList.begin(); i != end; ++i, ++index)
    {
        text = index;
        env.section = ini.addSection(text);
        ret = (*i)->saveToEnv(env) && ret;
    }

    env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
    return ini.save(env.folder) && ret;
}

void BindingConstraintsList::reverseWeightSign(const AreaLink* lnk)
{
    each([&lnk](BindingConstraint &constraint) { constraint.reverseWeightSign(lnk); });
}

uint64 BindingConstraintsList::memoryUsage() const
{
    uint64 m = sizeof(BindingConstraintsList);
    for (const auto & i : pList)
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

        bool operator()(const std::shared_ptr<BindingConstraint>& bc) const
        {
            assert(bc);
            if (bc->contains(pItem))
            {
                logs.info() << "destroying the binding constraint " << bc->name();
                return true;
            }
            return false;
        }

    private:
        const T *pItem;
    };

} // anonymous namespace

void BindingConstraintsList::remove(const Area* area)
{
    RemovePredicate<Area> predicate(area);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
}

void BindingConstraintsList::remove(const AreaLink* lnk)
{
    RemovePredicate<AreaLink> predicate(lnk);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
}

void BindingConstraintsList::remove(const BindingConstraint* bc)
{
    RemovePredicate<BindingConstraint> predicate(bc);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
}


BindingConstraintsList::iterator BindingConstraintsList::begin()
{
    return pList.begin();
}

BindingConstraintsList::const_iterator BindingConstraintsList::begin() const
{
    return pList.begin();
}

BindingConstraintsList::iterator BindingConstraintsList::end()
{
    return pList.end();
}

BindingConstraintsList::const_iterator BindingConstraintsList::end() const
{
    return pList.end();
}


void BindingConstraintsList::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    // Disabled by the optimization preferences
    if (!u.study.parameters.include.constraints)
        return;

    // each constraint...
    for (const auto & constraint : pList)
    {
        u.requiredMemoryForInput += sizeof(void *) * 2;
        uint count = (constraint->operatorType() == BindingConstraint::opBoth) ? 2 : 1;
        for (uint constraints_counter = 0; constraints_counter != count; ++constraints_counter)
        {
            u.requiredMemoryForInput += sizeof(BindingConstraintRTI);
            u.requiredMemoryForInput += (sizeof(long) + sizeof(double)) * constraint->linkCount();
            u.requiredMemoryForInput += (sizeof(long) + sizeof(double)) * constraint->clusterCount();
            Matrix<>::EstimateMemoryUsage(u, 1, HOURS_PER_YEAR);
        }
    }
}

void BindingConstraintsList::markAsModified() const
{
    for (const auto & i : pList)
        i->markAsModified();
}
}