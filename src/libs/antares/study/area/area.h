/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_AREAS_H__
#define __ANTARES_LIBS_STUDY_AREAS_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/noncopyable.h>
#include <stdlib.h>
#include <i_writer.h>
#include "../../array/matrix.h"
#include "../parts/parts.h"
#include <vector>
#include <set>
#include "links.h"
#include "ui.h"

namespace Antares
{
namespace Data
{
struct CompareAreaName;

/*!
** \brief Definition for a single area
*/
class Area final : private Yuni::NonCopyable<Area>
{
public:
    using NameSet = std::set<AreaName>;
    using Set = std::set<Area*, CompareAreaName>;
    using LinkMap = std::map<Area*, AreaLink::Set, CompareAreaName>;
    using Map = std::map<AreaName, Area*>;
    using Vector = std::vector<Area*>;
    using VectorConst = std::vector<const Area*>;
    using List = std::list<Area*>;
    //! Name mapping -> must be replaced by AreaNameMapping
    using NameMapping = std::map<AreaName, AreaName>;

public:
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
    explicit Area(const AnyString& name, uint nbParallelYears);
    /*!
    ** \brief Constructor
    **
    ** \param name Name of the area
    ** \param id id of the area
    */
    Area(const AnyString& name, const AnyString& id, uint nbParallelYears, uint indx = (uint)-1);
    /*!
    ** \brief Destructor
    */
    ~Area();
    //@}

    // !\name isVisibleOnLayer
    //@{
    /*!
    ** \brief check visibility on layer
    */
    bool isVisibleOnLayer(const size_t& layerID) const
    {
        if (ui == nullptr)
            return false;

        std::vector<size_t>& layerList = ui->mapLayersVisibilityList;
        std::vector<size_t>::iterator layerPosition
          = std::find(layerList.begin(), layerList.end(), layerID);
        return layerPosition != layerList.end();
    }

    //! \name Links
    //@{
    /*!
    ** \brief Destroy all links
    */
    void clearAllLinks();

    /*!
    ** \brief Properly detach all links attached to an area
    **
    ** It is the safe way to add an area and it is mainly used by the GUI
    */
    void detachAllLinks();

    /*!
    ** \brief Try to find the attached link from another area id
    */
    AreaLink* findLinkByID(const AreaName& id);
    const AreaLink* findLinkByID(const AreaName& id) const;

    /*!
    ** \brief Detach any link connected from this area to the given area
    */
    void detachLinkFromID(const AreaName& id);

    void detachLink(const AreaLink* lnk);

    /*!
    ** \brief Remove a link from its raw pointer
    */
    void detachLinkFromItsPointer(const AreaLink* lnk);
    //@}

    void buildLinksIndexes();
    /*!
    ** \brief Ensure all data are created
    */
    void ensureAllDataAreCreated();

    /*!
    ** \brief Reset all values to their default one
    */
    void resetToDefaultValues();

    /*!
    ** \brief Resize all matrices dedicated to the sampled timeseries numbers
    **
    ** \param n A number of years
    */
    void resizeAllTimeseriesNumbers(uint n);

    template<int TimeSeriesT>
    void storeTimeseriesNumbers(Solver::IResultWriter::Ptr writer) const;

    /*!
    ** \brief Check if a link with another area is already established
    **
    ** \param with Any area
    ** \return A pointer to an existing link if found, NULL otherwise
    */
    AreaLink* findExistingLinkWith(Area& with);
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
    **
    ** \param reload True to force the reload of data
    ** \return True if the operation succeeded
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark all areas as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Get the amount of memory currently used by the area
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Try to estimate the amount of memory required by the area for a simulation
    */
    void estimateMemoryUsage(StudyMemoryUsage&) const;
    //@}

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
    template<enum TimeSeries T>
    XCast* xcastData();
    template<enum TimeSeries T>
    const XCast* xcastData() const;

public:
    //! \name General
    //@{
    //! Name of the area
    AreaName name;
    //! Name of the area in lowercase format
    AreaName id;
    //! Index of the area  - only valid when already added to an area list
    uint index;
    //! Enabled
    bool enabled;
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

    //! \name Interconnections
    //@{
    //! All connections with this area
    AreaLink::Map links;
    //@}

    //! \name Nodal Optimization
    //@{
    //! Nodal optimization (see AreaNodalOptimization)
    uint nodalOptimization;
    //@}

    //! \name Spread
    //@{
    //! Spread for the unsupplied energy cost
    double spreadUnsuppliedEnergyCost;
    //! Spread for the spilled energy cost
    double spreadSpilledEnergyCost;
    //@}

    //! \name Output filtering
    //@{
    //! Print results for the area in the simulation synthesis
    uint filterSynthesis;
    //! Print results for the area in the year-by-year mode
    uint filterYearByYear;
    //@}

    //! \name UI
    //@{
    //! Information for the UI
    AreaUI* ui;
    //@}

    // Number of years actually run in parallel
    uint nbYearsInParallel;

    //! \name Dynamic
    //@{
    /*!
    ** \brief Scratchpad used temporary calculations (solver only)
    */
    mutable AreaScratchpad** scratchpad;
    //@}

    //! \name Data
    //@{
    /*!
    ** \brief Invalidate (JIT)
    **
    ** A non-zero value if the missing data must be loaded from HDD for the next
    ** save (only valid if JIT enabled).
    */
    mutable bool invalidateJIT;
    //@}

private:
    void internalInitialize();

    // Store time-series numbers
    void storeTimeseriesNumbersForLoad(Solver::IResultWriter::Ptr writer) const;
    void storeTimeseriesNumbersForSolar(Solver::IResultWriter::Ptr writer) const;
    void storeTimeseriesNumbersForWind(Solver::IResultWriter::Ptr writer) const;
    void storeTimeseriesNumbersForHydro(Solver::IResultWriter::Ptr writer) const;
    void storeTimeseriesNumbersForThermal(Solver::IResultWriter::Ptr writer) const;
    void storeTimeseriesNumbersForRenewable(Solver::IResultWriter::Ptr writer) const;
    void storeTimeseriesNumbersForTransmissionCapacities(Solver::IResultWriter::Ptr writer) const;
}; // class Area

bool saveAreaOptimisationIniFile(const Area& area, const Yuni::Clob& buffer);

bool saveAreaAdequacyPatchIniFile(const Area& area, const Yuni::Clob& buffer);

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
class AreaList final : public Yuni::NonCopyable<AreaList>
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

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    explicit AreaList(Study& study);
    //! Destructor
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
    bool loadListFromFile(const AnyString& filename);

    /*!
    ** \brief Save all informations about areas into a folder (-> input/generalData)
    **
    ** \param l The list of areas
    ** \param folder The target folder
    */
    bool saveToFolder(const AnyString& folder) const;

    /*!
    ** \brief Write the list of areas into a file
    **
    ** The file structure is merely composed by all names of areas, one line one area
    **
    ** \param filename The file to read
    ** \return A non-zero value if the operation was successful, 0 otherwise
    */
    bool saveListToFile(const AnyString& filename) const;

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

    /*!
    ** \brief Preload all areas which have been invalidated
    **
    ** \param [out] The number of areas which have been invalidated
    */
    bool preloadAndMarkAsModifiedAllInvalidatedAreas(uint* invalidateCount = nullptr) const;
    //@}

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

    Area* findFromPosition(const int x, const int y) const;

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

    /*!
    ** \brief Remove all elements in the container
    */
    void clear();

    //! Get if the container is empty
    bool empty() const;

    template<int TimeSeriesT>
    void storeTimeseriesNumbers(Solver::IResultWriter::Ptr writer) const;

    /*!
    ** \brief Invalidate all areas
    **
    ** \param reload True to reload data in the same time
    ** \return True if the operation succeeded
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark all data as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Rebuild the indexes for accessing areas
    **
    ** It is sometimes quite usefull to use a mere index to access to
    ** a given area. This is mandatory when used from the solver.
    */
    void rebuildIndexes();

    /*!
    ** \brief Remove an area from its ID
    **
    ** \warning When used by a study, do not forget to remove all binding
    **   constraints which depends upon this area before any call to this
    **   routine.
    */
    bool remove(const AnyString& id);

    /*!
    ** \brief Rename an area
    **
    ** \param oldid ID of the area to rename
    ** \param newName The new name for the area
    ** \return True if the operation succeeded (the area has been renamed)
    **   false otherwise (if another area has the same name)
    **
    ** \warning This function invalidates the index of all areas. If you need
    **   the indexes after a call to this routine, please use AreaListRebuildIndex()
    */
    bool renameArea(const AreaName& oldid, const AreaName& newName);

    /*!
    ** \brief Rename an area
    **
    ** \param oldid ID of the area to rename
    ** \param newID The new area ID
    ** \param newName The new name for the area
    ** \return True if the operation succeeded (the area has been renamed)
    **   false otherwise (if another area has the same name)
    **
    ** \warning This function invalidates the index of all areas. If you need
    **   the indexes after a call to this routine, please use AreaListRebuildIndex()
    */
    bool renameArea(const AreaName& oldid, const AreaName& newid, const AreaName& newName);

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
    AreaLink* findLink(const AreaName& area, const AreaName& with);
    const AreaLink* findLink(const AreaName& area, const AreaName& with) const;

    /*!
    ** \brief Try to find the link from a given INI key (<area1>%<area2>)
    */
    AreaLink* findLinkFromINIKey(const AnyString& key);

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
    /*!
    ** \brief Fix all invalid orientations
    */
    void fixOrientationForAllInterconnections(BindConstList& bindingconstraints);

    //! Remove all load timeseries
    void removeLoadTimeseries();
    //! Remove all hydro timeseries
    void removeHydroTimeseries();
    //! Remove all solar timeseries
    void removeSolarTimeseries();
    //! Remove all wind timeseries
    void removeWindTimeseries();
    //! Remove all thermal timeseries
    void removeThermalTimeseries();
    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Try to estimate the amount of memory required by the class for a simulation
    */
    void estimateMemoryUsage(StudyMemoryUsage&) const;

    /*!
    ** \brief Get the average amount of memory currently used by each area
    */
    double memoryUsageAveragePerArea() const;

    /*!
    ** \brief Get the amount of memory currently used by the class
    */
    Yuni::uint64 memoryUsage() const;

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

public:
    //! All areas by their index
    Area** byIndex;
    //! All areas in the list
    Area::Map areas;

    //! Name set (must be updated by updateNameSet)
    // used by the copy/paste
    mutable Area::NameSet nameidSet;

private:
    //! The parent study
    Study& pStudy;

}; // class AreaList

void AreaListDeleteLinkFromAreaPtr(AreaList* l, const Area* a);

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
bool AreaLinksLoadFromFolder(Study& s, AreaList* l, Area* area, const AnyString& folder);

/*!
** \brief Save interconnections of a given area into a folder (`input/areas/[area]/ntc`)
**
** \param area The area
** \param folder The target folder
** \return True if the operation succeeded, 0 otherwise
*/
bool AreaLinksSaveToFolder(const Area* area, const char* const folder);

// Save a given area's interconnexions configuration file into a folder
bool saveAreaLinksConfigurationFileToFolder(const Area* area, const char* const folder);

/*!
** \brief Clear all interconnection from an area
*/
int AreaLinkClear(AreaList* l, Area* area);

/*!
** \brief Remove a connection
*/
void AreaLinkRemove(AreaLink* lnk);

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
Area* AreaListAddFromName(AreaList& list, const AnyString& name, uint nbParallelYears);

/*!
** \brief Add an area in the list from a given name
**
** \param l The list of areas
** \param name The name of the area
** \param lname The name of the area in lowercase
** \return A valid pointer to the area if successful, NULL otherwise
*/
Area* AreaListAddFromNames(AreaList& list,
                           const AnyString& name,
                           const AnyString& lname,
                           uint nbParallelYears);

/*!
** \brief Try to establish a link between two areas
**
** \param l The list of areas
** \param area The area to make a link
** \param with The area to link with
*/
AreaLink* AreaListAddLink(AreaList* l, const char area[], const char with[], bool warning = true);

void AreaListClearAllLinks(AreaList* l);

/*!
** \brief Ensure data for load time-series are initialized
*/
void AreaListEnsureDataLoadTimeSeries(AreaList* l);

/*!
** \brief Ensure data for load prepro are initialized
*/
void AreaListEnsureDataLoadPrepro(AreaList* l);

/*!
** \brief Ensure data for load time-series are initialized
*/
void AreaListEnsureDataSolarTimeSeries(AreaList* l);

/*!
** \brief Ensure data for solar prepro are initialized
*/
void AreaListEnsureDataSolarPrepro(AreaList* l);

/*!
** \brief Ensure data for wind time-series are initialized
*/
void AreaListEnsureDataWindTimeSeries(AreaList* l);

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
** \brief Ensure data for thermal time-series are initialized
*/
void AreaListEnsureDataThermalTimeSeries(AreaList* l);

/*!
** \brief Ensure data for renewable time-series are initialized
*/
void AreaListEnsureDataRenewableTimeSeries(AreaList* l);

/*!
** \brief Ensure data for thermal prepro are initialized
*/
void AreaListEnsureDataThermalPrepro(AreaList* l);

} // namespace Data
} // namespace Antares

#include "../load-options.h"
#include "area.hxx"
#include "list.hxx"

#endif // __ANTARES_LIBS_STUDY_AREAS_H__
