// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_AREAS_H__
#define __ANTARES_LIBS_STUDY_AREAS_H__

#include <filesystem>
#include <set>
#include <stdlib.h>
#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>
#include <yuni/core/string.h>

#include <antares/array/matrix.h>
#include <antares/study/parameters/adq-patch-params.h>
#include "antares/study/filter.h"
#include "antares/study/parts/parts.h"

#include "constants.h"
#include "links.h"

namespace Antares::Data
{
struct CompareAreaName;

/*!
** \brief Definition for a single area
*/
class Area final: private Yuni::NonCopyable<Area>
{
public:
    using NameSet = std::set<AreaName>;
    using Map = std::map<AreaName, Area*>;
    using Vector = std::vector<Area*>;
    using ScratchMap = std::map<const Area*, AreaScratchpad&>;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    Area();
    /*!
    ** \brief Constructor with a given name
    **
    ** \param name The name of the area
    */
    explicit Area(const AnyString& name);
    /*!
    ** \brief Constructor
    **
    ** \param name Name of the area
    ** \param id id of the area
    */
    Area(const AnyString& name, const AnyString& id);
    /*!
    ** \brief Destructor
    */
    ~Area();
    //@}

    //! \name Links
    //@{
    /*!
    ** \brief Destroy all links
    */
    void clearAllLinks();

    void buildLinksIndexes();
    //@}

    /*!
    ** \brief Ensure all data are created
    */
    void createMissingData();

    /*!
    ** \brief Reset all values to their default one
    */
    void resetToDefaultValues();

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint nbYears);

    /*!
    ** \brief Check if a link with another area is already established
    **
    ** \param with Any area
    ** \return A pointer to an existing link if found, NULL otherwise
    */
    const AreaLink* findExistingLinkWith(const Area& with) const;

    //! \name Memory management
    //@{
    /*!
    ** \brief Load all data not already loaded
    **
    ** If the load-on-demand is enabled, some data might not be loaded (see `Matrix`)
    ** However, we would like to be able to force the load of all data, especially
    ** when saving a study.
    ** The flag `invalidateJIT` will be reset to false.
    */

    //! \name Thermal clusters min stable power validity checking
    //@{
    /*!
    ** \brief Check the validity of min stable power for all thermal clusters
    **
    ** \return True if the value of min stable power is a good one
    */
    bool thermalClustersMinStablePowerValidity(std::vector<YString>& output) const;
    //@}

    /*!
    ** \brief Get the XCast data according a given time-series type
    */
    template<enum TimeSeriesType T>
    XCast* xcastData();
    template<enum TimeSeriesType T>
    const XCast* xcastData() const;

    //! \name General
    //@{
    //! Name of the area
    AreaName name;
    //! Name of the area in lowercase format
    AreaName id;
    //! Index of the area  - only valid when already added to an area list
    uint index = (uint)(-1);
    //! Use adequacy patch for this area
    AdequacyPatch::AdequacyPatchMode adequacyPatchMode = AdequacyPatch::physicalAreaOutsideAdqPatch;
    /*@}*/

    //! \name Associate data */
    //@{
    /*!
    ** \brief Reserves
    **
    ** Reserves, DSM, D-1...
    ** \see enum ReservesIndex
    */
    Matrix<> reserves;
    /*!
    ** \brief Misc Gen
    **
    ** (previously called `Fatal hors hydro`) - fhhMax * HOURS_PER_YEAR
    ** The sum is done in runtimeinfos.cpp
    */
    Matrix<> miscGen; // [fhhMax][HOURS_PER_YEAR]
    //@}

    //! \name Load
    //@{
    Load::Container load;
    //@}

    //! \name Solar
    //@{
    Solar::Container solar;
    //@}

    //! \name Hydro
    //@{
    PartHydro hydro;
    //@}

    //! \name Wind
    //@{
    //! Wind time-series and Wind prepro data
    Wind::Container wind;
    //@}

    //! \name Thermal
    //@{
    PartThermal thermal;
    //@}

    //! \name Renewable
    //@{
    PartRenewable renewable;
    //@}

    ShortTermStorage::STStorageInput shortTermStorage;

    //! \name Interconnections
    //@{
    //! All connections with this area
    AreaLink::Map links;
    //@}

    //! \name Nodal Optimization
    //@{
    //! Nodal optimization (see AreaNodalOptimization)
    uint nodalOptimization = anoAll;
    //@}

    //! \name Spread
    //@{
    //! Spread for the unsupplied energy cost
    double spreadUnsuppliedEnergyCost = 0.;
    //! Spread for the spilled energy cost
    double spreadSpilledEnergyCost = 0.;
    //@}

    //! \name Output filtering
    //@{
    //! Print results for the area in the simulation synthesis
    uint filterSynthesis = filterAll;
    //! Print results for the area in the year-by-year mode
    uint filterYearByYear = filterAll;
    //@}

    //! \name Dynamic
    //@{
    /*!
    ** \brief Scratchpad used temporary calculations (solver only)
    */
    mutable std::vector<AreaScratchpad> scratchpad;
    //@}

    //! \name Data
    //@{
    /*!
    ** \brief Invalidate (JIT)
    **
    ** A non-zero value if the missing data must be loaded from HDD for the next
    ** save (only valid if JIT enabled).
    */
    mutable bool invalidateJIT = false;
    //@}

private:
    void createMissingTimeSeries();
    void createMissingPrepros();

}; // class Area

/*!
** \brief A list of areas
**
** Here is one way to iterate through the list :
** \code
** AreaList* l;
** ...
** <do some stuff here to fill the area list>
** ...
** Area* c = l->first;
** while (NULL != c)
** {
**		...
**		<do some stuff here with the area via `c`>
**		...
**		c = c->next;
** }
** \endcode
**
** It is possible to directly access to an area via its index :
** \code
** AreaList* l;
** ...
** <do some stuff here to fill the area list>
** ...
** printf("Area name : `%s`\n", (*(l->byIndex[2])).name);
** \endcode
*/
class AreaList final: public Yuni::NonCopyable<AreaList>
{
public:
    //! An iterator
    using iterator = Area::Map::iterator;
    //! A const iterator
    using const_iterator = Area::Map::const_iterator;
    //! An iterator
    using reverse_iterator = Area::Map::reverse_iterator;
    //! A const iterator
    using const_reverse_iterator = Area::Map::const_reverse_iterator;
    //! Key-value type
    using value_type = Area::Map::value_type;

    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit AreaList(Study& study);

    /*!
     ** \brief Destructor
     **
     ** Frees all Area instances owned by this list.
     */
    ~AreaList();
    //@}

    //! \name Iterating through all areas
    //@{
    /*!
    ** \brief Iterate through all areas
    */
    template<class PredicateT>
    void each(const PredicateT& predicate);
    /*!
    ** \brief Iterate through all areas (const)
    */
    template<class PredicateT>
    void each(const PredicateT& predicate) const;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    //! \name Internal Data TS-Generators / Series
    //@{
    /*!
    ** \brief Ensure data for time series/prepro are initalized if they should be
    **
    ** It initializes data for each area so it would be better to call this
    ** routine when areas are already loaded.
    */

    void ensureDataIsInitialized(Parameters& params);
    //@}

    //! \name Import / Export
    //@{
    /*!
    ** \brief Load all informations about areas from a folder (-> input/generalData)
    **
    ** \param l The list of areas
    ** \param folder The target folder
    */
    bool loadFromFolder(const StudyLoadOptions& options);

    /*!
    ** \brief Fill the list of areas from an input file
    **
    ** The file structure is merely composed by all names of areas, one line one area
    **
    ** \param l The structure
    ** \param filename The file to read
    ** \return A non-zero value if the operation was successful, 0 otherwise
    */
    bool loadListFromFile(const std::filesystem::path& filename);

    /*!
    ** \brief Write the list of all links into a file
    **
    ** \code
    ** <area name upstream>
    ** <tab><area name downstream>
    ** <tab><area name downstream>
    ** <area name upstream>
    ** <area name upstream>
    ** <tab><area name downstream>
    ** \endcode
    **
    ** \param filename The file to read
    ** \return A non-zero value if the operation was successful, 0 otherwise
    */
    void saveLinkListToBuffer(Yuni::Clob& buffer) const;

    //! \name Areas
    //@{
    /*!
    ** \brief Add an area in the container
    */
    Area* add(Area* a);

    /*!
    ** \brief Find an area from its unique string id
    */
    Area* find(const AreaName& id);
    /*!
    ** \brief Find an area from its unique string id (const)
    */
    const Area* find(const AreaName& id) const;

    /*!
    ** \brief Find an area from its name
    */
    Area* findFromName(const AreaName& name);

    /*!
    ** \brief Find an area from its name (const)
    */
    const Area* findFromName(const AreaName& name) const;

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    //! Get if the container is empty
    bool empty() const;

    /*!
    ** \brief Rebuild the indexes for accessing areas
    **
    ** It is sometimes quite usefull to use a mere index to access to
    ** a given area. This is mandatory when used from the solver.
    */
    void rebuildIndexes();

    /*!
    ** \brief Get the total number of areas
    */
    uint size() const;
    //@}

    //! \name Links
    //@{
    /*!
    ** \brief Find an interconnection between two areas
    **
    ** \param area The name of the first area (in lowercase)
    ** \param with The name of the second area (in lowercase)
    */
    const AreaLink* findLink(const AreaName& area, const AreaName& with) const;

    /*!
    ** \brief Try to find the link from a given INI key (<area1>%<area2>)
    */
    const AreaLink* findLinkFromINIKey(const AnyString& key) const;

    /*!
    ** \brief Try to find the cluster from a given INI key (<area>.<cluster>)
    */
    ThermalCluster* findClusterFromINIKey(const AnyString& key);

    /*!
    ** \brief Get the total number of interconnections between all areas
    */
    uint areaLinkCount() const;
    //@}

    //! \name Tools
    //@{

    /// create a map with the corresponding scratchpad for each area link to this numspace
    Area::ScratchMap buildScratchMap(uint numspace);

    /*!
    ** \brief Update the name id set
    */
    void updateNameIDSet() const;
    //@}

    //! \name Operators
    //@{
    /*!
    ** \brief Get an area from its index
    */
    Area* operator[](uint i);
    const Area* operator[](uint i) const;
    //@}

    //! All areas by their index
    std::vector<Area*> byIndex;
    //! All areas in the list
    Area::Map areas;

    //! Name set (must be updated by updateNameSet)
    // used by the copy/paste
    mutable Area::NameSet nameidSet;

private:
    //! The parent study
    Study& pStudy;

}; // class AreaList

/*!
** \brief Establish a link between two areas
**
** No check are made at this stade
**
** \param area The main area from where to establish the link
** \param with The second area
** \return A pointer to the structure used by the link between
** the two areas
*/
AreaLink* AreaAddLinkBetweenAreas(Area* area, Area* with, bool warning = true);

/*!
** \brief Load interconnections of a given area from a folder (`input/areas/[area]/ntc`)
**
** \param area The area
** \param folder The target folder
** \return A non-null value if the operation succeeded, 0 otherwise
*/
bool AreaLinksLoadFromFolder(Study& s,
                             AreaList* l,
                             Area* area,
                             const std::filesystem::path& folder);

/*!
** \brief Try to find an area by its name (in lowercase)
**
** \param l The area list
** \param lname The name of the area in lowercase
** \return A valid pointer to the area structure if found, NULL otherwise
*/
Area* AreaListLFind(AreaList* l, const char lname[]);

/*!
** \brief Try to find an area by its ptr
**
** \param l The area list
** \param ptr The ptr
** \return A valid pointer to the area structure if found, NULL otherwise
*/
Area* AreaListFindPtr(AreaList* l, const Area* ptr);

/*!
** \brief Add an area in the list from a given name
**
** \param l The list of areas
** \param name The name of the area
** \return A valid pointer to the area if successful, NULL otherwise
*/
Area* addAreaToListOfAreas(AreaList& list, const AnyString& name);

/*!
** \brief Add an area in the list from a given name
**
** \param l The list of areas
** \param name The name of the area
** \param lname The name of the area in lowercase
** \return A valid pointer to the area if successful, NULL otherwise
*/
Area* AreaListAddFromNames(AreaList& list, const AnyString& name, const AnyString& lname);

void AreaListClearAllLinks(AreaList* l);

/*!
** \brief Ensure data for load prepro are initialized
*/
void AreaListEnsureDataLoadPrepro(AreaList* l);

/*!
** \brief Ensure data for solar prepro are initialized
*/
void AreaListEnsureDataSolarPrepro(AreaList* l);

/*!
** \brief Ensure data for wind prepro are initialized
*/
void AreaListEnsureDataWindPrepro(AreaList* l);

/*!
** \brief Ensure data for hydro time-series are initialized
*/
void AreaListEnsureDataHydroTimeSeries(AreaList* l);

/*!
** \brief Ensure data for hydro prepro are initialized
*/
void AreaListEnsureDataHydroPrepro(AreaList* l);

/*!
** \brief Ensure data for thermal prepro are initialized
*/
void AreaListEnsureDataThermalPrepro(AreaList* l);

/*!
** \brief to check that Area name does not contains character *
*/
inline bool CheckForbiddenCharacterInAreaName(const AnyString& name)
{
    return name.contains('*');
}

} // namespace Antares::Data

#include "../load-options.h"
#include "area.hxx"

#endif // __ANTARES_LIBS_STUDY_AREAS_H__
