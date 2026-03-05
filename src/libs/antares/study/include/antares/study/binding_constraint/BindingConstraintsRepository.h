// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 11/05/23.
//

#pragma once

#include <functional>
#include <memory>

#include "BindingConstraint.h"

namespace Antares::Data
{
class BindingConstraintsRepository final: public Yuni::NonCopyable<BindingConstraintsRepository>
{
public:
    //! Vector of binding constraints
    using Vector = std::vector<std::shared_ptr<BindingConstraint>>;

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
                                      const std::filesystem::path& folder);

    //! Get the number of binding constraints
    [[nodiscard]] uint size() const;

    /*!
    ** \brief Convert all weekly constraints into daily ones
    */
    void changeConstraintsWeeklyToDaily();

    static Vector LoadBindingConstraint(EnvForLoading env);

    [[nodiscard]] Vector activeConstraints() const;

    [[nodiscard]] Vector getPtrForInequalityBindingConstraints() const;

private:
    //! All constraints
    Vector constraints_;

    mutable Vector activeConstraints_;
};

struct WhoseNameContains final
{
public:
    explicit WhoseNameContains(AnyString filter):
        pFilter(std::move(filter))
    {
    }

    WhoseNameContains(const WhoseNameContains&) = default;
    WhoseNameContains& operator=(const WhoseNameContains&) = default;
    WhoseNameContains(WhoseNameContains&&) noexcept = default;
    WhoseNameContains& operator=(WhoseNameContains&&) noexcept = default;

    bool operator()(const std::shared_ptr<BindingConstraint>& s) const
    {
        return (s->name()).contains(pFilter);
    }

private:
    AnyString pFilter;
};
} // namespace Antares::Data

#include "BindingConstraintsRepository.hxx"
