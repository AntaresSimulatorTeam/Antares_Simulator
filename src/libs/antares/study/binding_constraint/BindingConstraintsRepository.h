//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <functional>
#include <memory>
#include "BindingConstraint.h"
#include "BindingConstraintSaver.h"

namespace Antares::Data
{
class BindingConstraintsRepository final : public Yuni::NonCopyable<BindingConstraintsRepository>
{
public:
    //! Vector of binding constraints
    using Vector = std::vector<std::shared_ptr<BindingConstraint>>;
    //! Ordered Set of binding constraints
    using Set = std::set<std::shared_ptr<BindingConstraint>, CompareBindingConstraintName>;

    using iterator = Vector::iterator;
    using const_iterator = Vector::const_iterator;

    //! \name Constructor && Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    BindingConstraintsRepository() = default;
    /*!
    ** \brief Destructor
    */
    ~BindingConstraintsRepository() = default;
    //@}

    /*!
    ** \brief Delete all constraints
    */
    void clear();

    //! \name Iterating through all constraints
    //@{
    /*!
    ** \brief Iterate through all constraints
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all constraints (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const;

    iterator begin();
    [[nodiscard]] const_iterator begin() const;

    iterator end();
    [[nodiscard]] const_iterator end() const;

    [[nodiscard]] bool empty() const;
    //@}

    /*!
    ** \brief Add a new binding constraint
    */
    std::shared_ptr<BindingConstraint> add(const AnyString& name);

    /*!
    ** Try to find a constraint from its id
    */
    std::shared_ptr<Data::BindingConstraint> find(const AnyString& id);

    /*!
    ** \brief Try to find a constraint from its id (const)
    */
    [[nodiscard]] std::shared_ptr<const Data::BindingConstraint> find(const AnyString& id) const;

    /*!
     ** \brief Try to find a constraint from its name
     */
    [[nodiscard]] Data::BindingConstraint* findByName(const AnyString& name);

    /*!
    ** \brief Try to find a constraint from its name (const)
    */
    [[nodiscard]] const Data::BindingConstraint* findByName(const AnyString& name) const;

    /*!
    ** \brief Load all binding constraints from a folder
    */
    [[nodiscard]] bool loadFromFolder(Data::Study& s,
                                      const Data::StudyLoadOptions& options,
                                      const AnyString& folder);

    /*!
    ** \brief Save all binding constraints into a folder
    */
    [[nodiscard]] bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Reverse the sign of the weight for a given interconnection or thermal cluster
    **
    ** This method is used when reverting an interconnection or thermal cluster
    */
    void reverseWeightSign(const Data::AreaLink* lnk);

    //! Get the number of binding constraints
    [[nodiscard]] uint size() const;

    /*!
    ** \brief Remove a binding constraint
    */
    void remove(const Data::BindingConstraint* bc);
    /*!
    ** \brief Remove any binding constraint linked with a given area
    */
    void remove(const Data::Area* area);
    /*!
    ** \brief Remove any binding constraint linked with a given interconnection
    */
    void remove(const Data::AreaLink* area);

    /*!
    ** \brief Remove any binding constraint whose name contains the string in argument
    */
    void removeConstraintsWhoseNameConstains(const AnyString& filter);

    /*!
    ** \brief Rename a binding constraint
    */
    bool rename(Data::BindingConstraint* bc, const AnyString& newname);

    /*!
    ** \brief Convert all weekly constraints into daily ones
    */
    void changeConstraintsWeeklyToDaily();

    /*!
    ** \brief Get the memory usage
    */
    [[nodiscard]] uint64_t memoryUsage() const;

    /*!
    ** \brief Invalidate all matrices of all binding constraints
    */
    void forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the constraint as modified
    */
    void markAsModified() const;

    static std::vector<std::shared_ptr<BindingConstraint>> LoadBindingConstraint(EnvForLoading env);

    [[nodiscard]] std::vector<std::shared_ptr<BindingConstraint>> activeContraints() const;

    [[nodiscard]] std::vector<uint> getIndicesForInequalityBindingConstraints() const;

private:
    bool internalSaveToFolder(Data::BindingConstraintSaver::EnvForSaving& env) const;

    //! All constraints
    Data::BindingConstraintsRepository::Vector constraints_;

    mutable std::optional<std::vector<std::shared_ptr<BindingConstraint>>> activeConstraints_;
};

struct WhoseNameContains final
{
public:
    explicit WhoseNameContains(AnyString filter) : pFilter(std::move(filter))
    {
    }
    bool operator()(const std::shared_ptr<BindingConstraint>& s) const
    {
        return (s->name()).contains(pFilter);
    }

private:
    AnyString pFilter;
};
} // namespace Antares::Data
#include "BindingConstraintsRepository.hxx"