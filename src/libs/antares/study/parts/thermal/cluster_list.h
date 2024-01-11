#pragma once

#include "../common/cluster_list.h"
#include "cluster.h"

namespace Antares
{
namespace Data
{
/*!
** \brief List of clusters
** \ingroup thermalclusters
*/
class ThermalClusterList : public ClusterList<ThermalCluster>
{
public:
    std::string typeID() const override;
    uint64_t memoryUsage() const override;

    /*!
    ** \brief Get the size (bytes) occupied in memory by a `ThermalClusterList` structure
    ** \return A size (in bytes)
    */
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area);

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    ThermalClusterList();
    /*!
    ** \brief Destructor
    */
    virtual ~ThermalClusterList();
    //@}

    //! \name Spinning
    //@{
    /*!
    ** \brief Calculation of Spinning for all thermal clusters
    */
    void calculationOfSpinning();

    /*!
    ** \brief Calculation of Spinning for all thermal clusters (reverse)
    */
    void reverseCalculationOfSpinning();
    //@}

    //! \name Mustrun
    //@{
    /*!
    ** \brief Enable the 'mustrun' mode for every cluster
    **
    ** This method is especially useful for the adequacy mode, where all clusters
    ** must be in mustrun mode
    */
    void enableMustrunForEveryone();
    //@}

    auto each_mustrun_and_enabled() const 
    { 
        return allClusters | std::views::filter(&ThermalCluster::isMustRun)
                           | std::views::filter(&ThermalCluster::isEnabled);
    }

    auto each_enabled_and_not_mustrun() const 
    {   
        return allClusters | std::views::filter(&ThermalCluster::isEnabled)
                           | std::views::filter(&ThermalCluster::isNotMustRun); 
    }


    /*!
    ** \brief Ensure data for the prepro are initialized
    ** \ingroup thermalclusters
    **
    ** \todo Remaining of old C-library. this routine should be moved into the appropriate class
    ** \param l The list of thermal clusters
    */
    void ensureDataPrepro();

    /*!
     ** \brief Load data related to the preprocessor from a list of thermal clusters from a folder
     ** \ingroup thermalclusters
     **
     ** \todo Remaining of old C-library. this routine should be moved into the appropriate class
     ** \param l A list of thermal clusters
     ** \param folder The target folder
     ** \return A non-zero value if the operation succeeded, 0 otherwise
     */
    bool loadPreproFromFolder(Study& s, const StudyLoadOptions& options, const AnyString& folder);

    bool loadEconomicCosts(Study& s, const AnyString& folder);

    bool savePreproToFolder(const AnyString& folder) const;
    bool saveEconomicCosts(const AnyString& folder) const;

    bool saveToFolder(const AnyString& folder) const override;

    void sortCompleteList();
    void giveIndicesToClusters();
    unsigned int mustRunAndEnabledCount() const;
    std::shared_ptr<ThermalCluster> enabledClusterAt(unsigned int index) const;
    void removeMustRunClusters();
    void clearAll();
    unsigned int allClustersSize();

}; // class ThermalClusterList
} // namespace Data
} // namespace Antares
