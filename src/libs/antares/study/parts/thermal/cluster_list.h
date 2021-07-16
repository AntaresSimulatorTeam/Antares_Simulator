#pragma once

#include "../common/cluster_list.h"

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
    // Overriden pure virtual methods
    YString typeID() const override;

    /*!
    ** \brief Get the size (bytes) occupied in memory by a `ThermalClusterList` structure
    ** \return A size (in bytes)
    */
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area);

public:
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

    bool remove(const ClusterName& id) override;

    void estimateMemoryUsage(StudyMemoryUsage&) const override;

    Map mapping;

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

    bool savePreproToFolder(const AnyString& folder) const;

    bool saveToFolder(const AnyString& folder) const override;

}; // class ThermalClusterList
} // namespace Data
} // namespace Antares
