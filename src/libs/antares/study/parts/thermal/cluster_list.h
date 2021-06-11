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
    bool loadFromFolder(Study& s, const AnyString& folder, Area* area) override;

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
    ~ThermalClusterList();
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

    //! \name Informations
    //@{
    /*!
    ** \brief Retrieve the total capacity and the total unit count
    **
    ** Pseudo code:
    ** \code
    ** each thermal cluster do
    ** 	total += cluster{unit count} * cluster{nominal capacity}
    **	unit  += cluster{unit count}
    ** \endcode
    */
    void retrieveTotalCapacityAndUnitCount(double& total, uint& unitCount) const;
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
    ** \brief Ensure data for the time-series are initialized
    ** \ingroup thermalclusters
    **
    ** \todo Remaining of old C-library. this routine should be moved into the appropriate class
    ** \param l The list of thermal clusters
    */
    void ensureDataTimeSeries();
}; // class ThermalClusterList

/*!
** \brief Save a list of thermal clusters to a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l The list of thermal cluster
** \param folder The folder where the data will be written
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
bool ThermalClusterListSaveToFolder(const ThermalClusterList* l, const AnyString& folder);

/*!
** \brief Load data related to the preprocessor from a list of thermal clusters from a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
bool ThermalClusterListLoadPreproFromFolder(Study& s,
                                            const StudyLoadOptions& options,
                                            ThermalClusterList* l,
                                            const AnyString& folder);

/*!
** \brief Save data related to the preprocessor from a list of thermal clusters to a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
bool ThermalClusterListSavePreproToFolder(const ThermalClusterList* l, const AnyString& folder);

/*!
** \brief Load data related to time-series from a list of thermal clusters from a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int ThermalClusterListLoadDataSeriesFromFolder(Study& study,
                                               const StudyLoadOptions& options,
                                               ThermalClusterList* l,
                                               const AnyString& folder,
                                               int fast);

/*!
** \brief Save data related to time-series from a list of thermal clusters to a folder
** \ingroup thermalclusters
**
** \todo Remaining of old C-library. this routine should be moved into the appropriate class
** \param l A list of thermal clusters
** \param folder The target folder
** \param msg Message to display
** \return A non-zero value if the operation succeeded, 0 otherwise
*/
int ThermalClusterListSaveDataSeriesToFolder(const ThermalClusterList* l, const AnyString& folder);
int ThermalClusterListSaveDataSeriesToFolder(const ThermalClusterList* l,
                                             const AnyString& folder,
                                             const YString& msg);

} // namespace Data
} // namespace Antares
