//
// Created by marechaljas on 11/05/23.
//

#include "BindingConstraintsRepository.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "BindingConstraint.h"
#include <antares/study/study.h>
#include "BindingConstraintLoader.h"
#include "BindingConstraintSaver.h"

void Data::BindingConstraintsRepository::clear()
{
    constraints_.clear();
    activeConstraints_.reset();
}

namespace Antares::Data {
std::shared_ptr<Data::BindingConstraint> BindingConstraintsRepository::find(const AnyString &id) {
    for (auto const &i: constraints_) {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

std::shared_ptr<const Data::BindingConstraint> BindingConstraintsRepository::find(const AnyString &id) const {
    for (const auto & i : constraints_) {
        if (i->id() == id)
            return i;
    }
    return nullptr;
}

BindingConstraint *BindingConstraintsRepository::findByName(const AnyString &name) {
    for (auto const & i : constraints_) {
        if (i->name() == name)
            return i.get();
    }
    return nullptr;
}

const BindingConstraint *BindingConstraintsRepository::findByName(const AnyString &name) const {
    for (const auto & i : constraints_) {
        if (i->name() == name)
            return i.get();
    }
    return nullptr;
}

void BindingConstraintsRepository::removeConstraintsWhoseNameConstains(const AnyString &filter) {
    WhoseNameContains pred(filter);
    constraints_.erase(std::remove_if(constraints_.begin(), constraints_.end(), pred), constraints_.end());
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

/**
 * @brief Compare ranged constraint i.e their names end with _lt or _gt
 * if suffixes are equal then the "*_lt" constraint is less than the "*_gt"
 * @param constraint_1
 * @param constraint_2
 * @return true if constraint_1 < constraint_2
 */
bool compareRangedConstraint(const ConstraintName& constraint_1, const ConstraintName& constraint_2)
{
    std::string tmp_str1 = constraint_1.c_str();
    std::string tmp_str2 = constraint_2.c_str();
    if (auto left_pos = tmp_str1.find_last_of("_lt"); left_pos != std::string::npos)
    {
        std::string::iterator left = tmp_str1.begin() + left_pos;
        size_t right_pos = 0;
        if (right_pos = tmp_str2.find_last_of("_gt"); right_pos == std::string::npos)
        {
            logs.error() << "Ranged constraints comparison : " << tmp_str1 << " and " << tmp_str2
                         << " has failed!";
        }
        std::string::iterator right = tmp_str2.begin() + right_pos;

        auto new_s1 = std::string(tmp_str1.begin(), left - 2);
        auto new_s2 = std::string(tmp_str2.begin(), right - 2);
        bool ret = new_s1 == new_s2 ? true : constraint_1 < constraint_2;
        return ret;
    }
    return constraint_1 < constraint_2;
}
bool compareConstraints(const std::shared_ptr<BindingConstraint>& s1,
                        const std::shared_ptr<BindingConstraint>& s2)
{
    if (s1->name() != s2->name())
    {
        if (s1->name().endsWith("_lt") && s2->name().endsWith("_gt"))
        {
            return compareRangedConstraint(s1->name(), s2->name());
        }
        else if (s1->name().endsWith("_gt") && s2->name().endsWith("_lt"))
        {
            return !compareRangedConstraint(s2->name(), s1->name());
        }
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
    bc->pId(name);
    constraints_.push_back(bc);
    std::sort(constraints_.begin(), constraints_.end(), compareConstraints);
    return bc;
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
    if (std::any_of(constraints_.begin(), constraints_.end(), [&id](auto constraint) {
            return constraint->id() == id;
        }))
    {
        return false;
    }
    bc->name(name);
    bc->pId(name);
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
                std::copy(new_bc.begin(), new_bc.end(), std::back_inserter(constraints_));
            }
        }
    }

    // Logs
    if (constraints_.empty())
        logs.info() << "No binding constraint found";
    else
    {
        std::sort(constraints_.begin(), constraints_.end(), compareConstraints);

        if (constraints_.size() == 1)
            logs.info() << "1 binding constraint found";
        else
            logs.info() << constraints_.size() << " binding constraints found";
    }

    // When ran from the solver and if the simplex is in `weekly` mode,
    // all weekly constraints will become daily ones.
    if (study.usedByTheSolver && sorDay == study.parameters.simplexOptimizationRange) {
        changeConstraintsWeeklyToDaily();
    }

    return true;
}

void BindingConstraintsRepository::changeConstraintsWeeklyToDaily()
{
    each([](BindingConstraint &constraint) {
        if (constraint.type() == BindingConstraint::typeWeekly)
        {
            logs.info() << "  The type of the constraint '" << constraint.name()
                        << "' is now 'daily'";
            constraint.setTimeGranularity(BindingConstraint::typeDaily);
        }
    });
}

bool BindingConstraintsRepository::internalSaveToFolder(BindingConstraintSaver::EnvForSaving& env) const
{
    if (constraints_.empty())
    {
        logs.info() << "No binding constraint to export.";
        if (!Yuni::IO::Directory::Create(env.folder))
            return false;
        // stripping the file
        env.folder << Yuni::IO::Separator << "bindingconstraints.ini";
        return Yuni::IO::File::CreateEmptyFile(env.folder);
    }

    if (constraints_.size() == 1)
        logs.info() << "Exporting 1 binding constraint...";
    else
        logs.info() << "Exporting " << constraints_.size() << " binding constraints...";

    if (!Yuni::IO::Directory::Create(env.folder))
        return false;

    IniFile ini;
    bool ret = true;
    uint index = 0;
    auto end = constraints_.end();
    Yuni::ShortString64 text;

    for (auto i = constraints_.begin(); i != end; ++i, ++index)
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

uint64_t BindingConstraintsRepository::memoryUsage() const
{
    uint64_t m = sizeof(BindingConstraintsRepository);
    for (const auto & i : constraints_)
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

void BindingConstraintsRepository::remove(const Area* area)
{
    RemovePredicate<Area> predicate(area);
    auto e = std::remove_if(constraints_.begin(), constraints_.end(), predicate);
    constraints_.erase(e, constraints_.end());
    activeConstraints_.reset();
}

void BindingConstraintsRepository::remove(const AreaLink* lnk)
{
    RemovePredicate<AreaLink> predicate(lnk);
    auto e = std::remove_if(constraints_.begin(), constraints_.end(), predicate);
    constraints_.erase(e, constraints_.end());
    activeConstraints_.reset();
}

void BindingConstraintsRepository::remove(const BindingConstraint* bc)
{
    RemovePredicate<BindingConstraint> predicate(bc);
    auto e = std::remove_if(constraints_.begin(), constraints_.end(), predicate);
    constraints_.erase(e, constraints_.end());
    activeConstraints_.reset();
}


BindingConstraintsRepository::iterator BindingConstraintsRepository::begin()
{
    return constraints_.begin();
}

BindingConstraintsRepository::const_iterator BindingConstraintsRepository::begin() const
{
    return constraints_.begin();
}

BindingConstraintsRepository::iterator BindingConstraintsRepository::end()
{
    return constraints_.end();
}

BindingConstraintsRepository::const_iterator BindingConstraintsRepository::end() const
{
    return constraints_.end();
}

void BindingConstraintsRepository::markAsModified() const
{
    for (const auto & i : constraints_)
        i->markAsModified();
}

std::vector<std::shared_ptr<BindingConstraint>> BindingConstraintsRepository::activeContraints() const {
    if (activeConstraints_) {
        return activeConstraints_.value();
    } else {
        std::vector<std::shared_ptr<BindingConstraint>> out;
        std::copy_if(constraints_.begin(), constraints_.end(), std::back_inserter(out),
                     [](const auto &bc) {
                         return bc->isActive();
                     });
        activeConstraints_ = std::move(out);
        return activeConstraints_.value();
    }
}

static bool isBindingConstraintTypeInequality(const Data::BindingConstraint& bc)
{
    return bc.operatorType() == BindingConstraint::opLess || bc.operatorType() == BindingConstraint::opGreater;
}

std::vector<uint> BindingConstraintsRepository::getIndicesForInequalityBindingConstraints() const
{
    auto activeConstraints = activeContraints();
    const auto firstBC = activeConstraints.begin();
    const auto lastBC = activeConstraints.end();

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

void BindingConstraintsRepository::forceReload(bool reload) const
{
    if (!constraints_.empty())
    {
        for (const auto & i : constraints_)
            i->forceReload(reload);
    }
}

}
