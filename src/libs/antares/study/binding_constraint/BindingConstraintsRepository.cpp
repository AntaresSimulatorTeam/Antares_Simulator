//
// Created by marechaljas on 11/05/23.
//

#include "BindingConstraintsRepository.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "BindingConstraint.h"
#include "antares/study.h"
#include "BindingConstraintLoader.h"
#include "BindingConstraintSaver.h"

void Data::BindingConstraintsRepository::clear()
{
    pList.clear();
    enabledConstraints_.reset();
}

namespace Antares::Data {
std::shared_ptr<Data::BindingConstraint> BindingConstraintsRepository::find(const AnyString &id) {
    for (auto const &i: pList) {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

std::shared_ptr<const Data::BindingConstraint> BindingConstraintsRepository::find(const AnyString &id) const {
    for (const auto & i : pList) {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

BindingConstraint *BindingConstraintsRepository::findByName(const AnyString &name) {
    for (auto const & i : pList) {
        if (i->name() == name)
            return i.get();
    }
    return nullptr;
}

const BindingConstraint *BindingConstraintsRepository::findByName(const AnyString &name) const {
    for (const auto & i : pList) {
        if (i->name() == name)
            return i.get();
    }
    return nullptr;
}

void BindingConstraintsRepository::removeConstraintsWhoseNameConstains(const AnyString &filter) {
    WhoseNameContains pred(filter);
    pList.erase(std::remove_if(pList.begin(), pList.end(), pred), pList.end());
}

static int valueForSort(BindingConstraint::Operator op)
{
    switch (op)
    {
    case BindingConstraint::opLess:
        return 0;
    case BindingConstraint::opGreater:
        return 1;
    case BindingConstraint::opEquality:
        return 2;
    case BindingConstraint::opBoth:
        return 3;
    default:
        return -1;
    }
}

bool compareConstraints(const std::shared_ptr<BindingConstraint>& s1,
                        const std::shared_ptr<BindingConstraint>& s2)
{
    if (s1->name() != s2->name())
    {
        return s1->name() < s2->name();
    }
    else
    {
        return valueForSort(s1->operatorType()) < valueForSort(s2->operatorType());
    }
}

std::shared_ptr<BindingConstraint> BindingConstraintsRepository::add(const AnyString &name)
{
    auto bc = std::make_shared<BindingConstraint>();
    bc->name(name);
    pList.push_back(bc);
    std::sort(pList.begin(), pList.end(), compareConstraints);
    return bc;
}

void BindingConstraintsRepository::resizeAllTimeseriesNumbers(unsigned int nb_years) {
    initializeTsNumbers();
    std::for_each(groupToTimeSeriesNumbers.begin(), groupToTimeSeriesNumbers.end(), [&](auto &kvp) {
        groupToTimeSeriesNumbers[kvp.first].timeseriesNumbers.clear();
        groupToTimeSeriesNumbers[kvp.first].timeseriesNumbers.resize(1, nb_years);
    });
}

void BindingConstraintsRepository::fixTSNumbersWhenWidthIsOne() {
    std::map<std::string, bool, std::less<>> groupOfOneTS;
    std::for_each(pList.begin(), pList.end(), [&groupOfOneTS](auto bc) {
        auto hasOneTs = bc->RHSTimeSeries().width == 1;
        if (groupOfOneTS[bc->group()] && !hasOneTs) {
            assert(false && ("Group of binding constraints mixing 1TS and N TS group:" + bc->group()).c_str());
        }
        groupOfOneTS[bc->group()] |= hasOneTs;
    });
    std::for_each(groupToTimeSeriesNumbers.begin(), groupToTimeSeriesNumbers.end(),
                  [&groupOfOneTS](std::pair<std::string, BindingConstraintTimeSeriesNumbers> it) {
                      if (groupOfOneTS[it.first]) {
                          it.second.timeseriesNumbers.fillColumn(0, 0);
                      }
                  });
}

std::vector<std::shared_ptr<BindingConstraint>>
BindingConstraintsRepository::LoadBindingConstraint(EnvForLoading env) {
    BindingConstraintLoader loader;
    return loader.load(std::move(env));
}

bool BindingConstraintsRepository::saveToFolder(const AnyString &folder) const {
    BindingConstraintSaver::EnvForSaving env;
    env.folder = folder;
    return internalSaveToFolder(env);
}

bool BindingConstraintsRepository::rename(BindingConstraint *bc, const AnyString &newname) {
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
    JIT::Invalidate(bc->RHSTimeSeries().jit);
    return true;
}

bool BindingConstraintsRepository::loadFromFolder(Study &study,
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
               auto new_bc = LoadBindingConstraint(env);
                std::copy(new_bc.begin(), new_bc.end(), std::back_inserter(pList));
            }
        }
    }

    bool hasError = checkTimeSeriesWidthConsistency();

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

    initializeTsNumbers();

    return !hasError;
}

bool BindingConstraintsRepository::checkTimeSeriesWidthConsistency() const {
    bool hasError = false;
    std::map<std::string, unsigned, std::less<>> timeSeriesCountByGroup;
    for(const auto& bc: this->pList) {
        auto count = timeSeriesCountByGroup[bc->group()];
        auto width = bc->RHSTimeSeries().width;
        if (count == 0) {
            timeSeriesCountByGroup[bc->group()] = width;
            continue;
        }
        if (count != width) {
            logs.error() << "Inconsistent time series width for constraint of the same group. Group at fault: "
                         << bc->group()
                         << " .Previous width was " << count
                         << " new constraint " << bc->name()
                         << " found with width of " << width;
            hasError = true;
        }
    }
    return hasError;
}

void BindingConstraintsRepository::initializeTsNumbers() {
    for (const auto& bc: pList) {
        groupToTimeSeriesNumbers[bc->group()] = {};
    }
}

void BindingConstraintsRepository::mutateWeeklyConstraintsIntoDailyOnes()
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

bool BindingConstraintsRepository::internalSaveToFolder(BindingConstraintSaver::EnvForSaving& env) const
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
        ret = Antares::Data::BindingConstraintSaver::saveToEnv(env, i->get()) && ret;
    }

    env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
    return ini.save(env.folder) && ret;
}

void BindingConstraintsRepository::reverseWeightSign(const AreaLink* lnk)
{
    each([&lnk](BindingConstraint &constraint) { constraint.reverseWeightSign(lnk); });
}

uint64 BindingConstraintsRepository::memoryUsage() const
{
    uint64 m = sizeof(BindingConstraintsRepository);
    for (const auto & i : pList)
        m += i->memoryUsage();
    m += timeSeriesNumberMemoryUsage();
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

void BindingConstraintsRepository::remove(const Area* area)
{
    RemovePredicate<Area> predicate(area);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
    enabledConstraints_.reset();
}

void BindingConstraintsRepository::remove(const AreaLink* lnk)
{
    RemovePredicate<AreaLink> predicate(lnk);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
    enabledConstraints_.reset();
}

void BindingConstraintsRepository::remove(const BindingConstraint* bc)
{
    RemovePredicate<BindingConstraint> predicate(bc);
    auto e = std::remove_if(pList.begin(), pList.end(), predicate);
    pList.erase(e, pList.end());
    enabledConstraints_.reset();
}


BindingConstraintsRepository::iterator BindingConstraintsRepository::begin()
{
    return pList.begin();
}

BindingConstraintsRepository::const_iterator BindingConstraintsRepository::begin() const
{
    return pList.begin();
}

BindingConstraintsRepository::iterator BindingConstraintsRepository::end()
{
    return pList.end();
}

BindingConstraintsRepository::const_iterator BindingConstraintsRepository::end() const
{
    return pList.end();
}


void BindingConstraintsRepository::estimateMemoryUsage(StudyMemoryUsage& u) const
{
    // Disabled by the optimization preferences
    if (!u.study.parameters.include.constraints)
        return;

        // each constraint...
    for (const auto &constraint: pList) {
        u.requiredMemoryForInput += sizeof(void *) * 2;
        uint count = (constraint->operatorType() == BindingConstraint::opBoth) ? 2 : 1;
        for (uint constraints_counter = 0; constraints_counter != count; ++constraints_counter) {
            u.requiredMemoryForInput += (sizeof(long) + sizeof(double)) * constraint->linkCount();
            u.requiredMemoryForInput += (sizeof(long) + sizeof(double)) * constraint->clusterCount();
            Matrix<>::EstimateMemoryUsage(u, 1, HOURS_PER_YEAR);
        }
    }
}

void BindingConstraintsRepository::markAsModified() const
{
    for (const auto & i : pList)
        i->markAsModified();
}

uint64 BindingConstraintsRepository::timeSeriesNumberMemoryUsage() const {
    uint64 m = sizeof(groupToTimeSeriesNumbers);
    for (const auto& [key, value]: groupToTimeSeriesNumbers) {
        m += sizeof(key);
        m += value.memoryUsage();
    }
    return m;
}

std::vector<std::shared_ptr<BindingConstraint>> BindingConstraintsRepository::enabled() const {
    if (enabledConstraints_) {
        return enabledConstraints_.value();
    } else {
        std::vector<std::shared_ptr<BindingConstraint>> out;
        std::copy_if(pList.begin(), pList.end(), std::back_inserter(out),
                     [](const auto &bc) {
                         return bc->enabled();
                     });
        enabledConstraints_ = out;
        return enabledConstraints_.value();
    }
}

static bool isBindingConstraintTypeInequality(const Data::BindingConstraint& bc)
{
    return bc.operatorType() == BindingConstraint::opLess || bc.operatorType() == BindingConstraint::opGreater;
}

std::vector<uint> BindingConstraintsRepository::getIndicesForInequalityBindingConstraints() const
{
    auto enabledBCs = enabled();
    const auto firstBC = enabledBCs.begin();
    const auto lastBC = enabledBCs.end();

    std::vector<uint> indices;
    for (auto bc = firstBC; bc < lastBC; bc++)
    {
        if (isBindingConstraintTypeInequality(*(*bc)))
        {
            auto index = static_cast<uint>(std::distance(firstBC, bc));
            indices.push_back(index);
        }
    }
    return indices;
}
}