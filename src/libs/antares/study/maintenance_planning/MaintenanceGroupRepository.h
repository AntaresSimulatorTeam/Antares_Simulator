//
// Created by milos on 1/11/23.
//

#pragma once

#include <functional>
#include <memory>
#include "MaintenanceGroup.h"
#include "MaintenanceGroupSaver.h"

namespace Antares::Data
{
class MaintenanceGroupRepository final : public Yuni::NonCopyable<MaintenanceGroupRepository>
{
public:
    //! Vector of Maintenance Group-s
    using Vector = std::vector<std::shared_ptr<MaintenanceGroup>>;
    //! Ordered Set of Maintenance Group-s
    using Set = std::set<std::shared_ptr<MaintenanceGroup>, CompareMaintenanceGroupName>;

    using iterator = Vector::iterator;
    using const_iterator = Vector::const_iterator;

    //! \name Constructor && Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    MaintenanceGroupRepository() = default;
    /*!
    ** \brief Destructor
    */
    ~MaintenanceGroupRepository() = default;
    //@}

    /*!
    ** \brief Delete all Maintenance Group-s
    */
    void clear();

    //! \name Iterating through all Maintenance Group-s
    //@{
    /*!
    ** \brief Iterate through all Maintenance Group-s
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all Maintenance Group-s (const)
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
    ** \brief Add a new Maintenance Group
    */
    std::shared_ptr<MaintenanceGroup> add(const AnyString& name);

    /*!
    ** Try to find a Maintenance Group from its id
    */
    std::shared_ptr<Data::MaintenanceGroup> find(const AnyString& id);

    /*!
    ** \brief Try to find a Maintenance Group from its id (const)
    */
    [[nodiscard]] std::shared_ptr<const Data::MaintenanceGroup> find(const AnyString& id) const;

    /*!
     ** \brief Try to find a Maintenance Group from its name
     */
    [[nodiscard]] Data::MaintenanceGroup* findByName(const AnyString& name);

    /*!
    ** \brief Try to find a Maintenance Group from its name (const)
    */
    [[nodiscard]] const Data::MaintenanceGroup* findByName(const AnyString& name) const;

    /*!
    ** \brief Load all Maintenance Group from a folder
    */
    [[nodiscard]] bool loadFromFolder(Data::Study& s,
                                      const Data::StudyLoadOptions& options,
                                      const AnyString& folder);

    /*!
    ** \brief Save all Maintenance Group into a folder
    */
    [[nodiscard]] bool saveToFolder(const AnyString& folder) const;

    // TODO CR27: do we need this at all
    /*!
    ** \brief Reverse the sign of the weight for a given Maintenance Group
    **
    ** This method is used when reverting an Maintenance Group
    */
    void reverseWeightSign(const Data::AreaLink* lnk);

    //! Get the number of Maintenance Group
    [[nodiscard]] uint size() const;

    /*!
    ** \brief Remove a Maintenance Group
    */
    void remove(const Data::MaintenanceGroup* bc);
    /*!
    ** \brief Remove any Maintenance Group linked with a given area
    */
    void remove(const Data::Area* area);

    /*!
    ** \brief Remove any Maintenance Group whose name contains the string in argument
    */
    void removeMaintenanceGroupsWhoseNameConstains(const AnyString& filter);

    /*!
    ** \brief Rename a Maintenance Group
    */
    bool rename(Data::MaintenanceGroup* bc, const AnyString& newname);

    /*!
    ** \brief Get the memory usage
    */
    [[nodiscard]] uint64_t memoryUsage() const;

    static std::vector<std::shared_ptr<MaintenanceGroup>> LoadMaintenanceGroup(EnvForLoading env);

    [[nodiscard]] std::vector<std::shared_ptr<MaintenanceGroup>> activeMaintenanceGroups() const;

    /*!
    ** \brief The scenarios number and length
    **
    ** \return Get the scenarios number and length
    */
    uint scenariosNumber() const;
    uint scenariosLength() const;

    /*!
    ** \brief Set the scenarios number and length
    */
    void scenariosNumber(uint num);
    void scenariosLength(uint l);

private:
    
    // number and length of the Maintenance Group-s scenarios
    uint scenariosNumber_ = 0;
    uint scenariosLength_ = 0;
    
    // private methods
    bool internalSaveToFolder(Data::MaintenanceGroupSaver::EnvForSaving& env) const;

    //! All Maintenance Group-s
    Data::MaintenanceGroupRepository::Vector maintenanceGroups_;
    mutable std::optional<std::vector<std::shared_ptr<MaintenanceGroup>>> activeMaintenanceGroups_;
};

struct NameContainsMnt final
{
public:
    explicit NameContainsMnt(AnyString filter) : pFilter(std::move(filter))
    {
    }
    bool operator()(const std::shared_ptr<MaintenanceGroup>& s) const
    {
        return ((s->name()).find(pFilter) != std::string::npos);
    }

private:
    AnyString pFilter;
};
} // namespace Antares::Data

#include "MaintenanceGroupRepository.hxx"