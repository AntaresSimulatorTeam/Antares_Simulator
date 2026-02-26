// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__
#define __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__

#include <map>
#include <optional>
#include <string>

#include <antares/array/matrix.h>

namespace Antares::Data
{
/*!
 ** \class TimeSeries
 ** \brief This class is used to represent the generic time series
 **
 **  The goal is to handle indexing with the time series numbers: getCoefficient()
 **  and also providing a wrapper for all the Matrix<> functions such as resize()
 */
class TimeSeries;

class TimeSeriesNumbers final
{
public:
    void registerSeries(const TimeSeries* s, std::string label);
    // Return a description of the error in case of inconsistent number of columns, std::nullopt
    // otherwis
    std::optional<std::string> checkSeriesNumberOfColumnsConsistency() const;

    uint32_t operator[](uint y) const;
    uint32_t& operator[](uint y);

    void clear();
    void reset(uint h);

    uint height() const;

    void saveToBuffer(std::string& data) const;

private:
    Matrix<uint32_t> tsNumbers;
    std::map<std::string, const TimeSeries*> series;
};

class TimeSeries final
{
public:
    using TS = Matrix<double>;

    explicit TimeSeries(TimeSeriesNumbers& tsNumbers);
    /*!
     ** \brief Load series from a file
     **
     ** \param path path of the file
     ** \param average used to average timeseries
     ** \return A non-zero value if the operation succeeded, 0 otherwise
     */
    bool loadFromFile(const std::filesystem::path& path,
                      const bool average,
                      unsigned options = Matrix<>::optNone);
    /*!
     ** \brief Save time series to a file
     **
     ** \param areaID The ID of the area associated to the data series
     ** \param folder The target folder
     ** \param prefix the prefix for the filename
     ** \return A non-zero value if the operation succeeded, 0 otherwise
     */
    int saveToFolder(const std::string& areaID,
                     const std::string& folder,
                     const std::string& prefix) const;

    int saveToFile(const std::string& filename, bool saveEvenIfAllZero) const;

    double getCoefficient(uint32_t year, uint32_t timestep) const;
    const double* getColumn(uint32_t year) const;
    uint32_t getSeriesIndex(uint32_t year) const;

    /// \brief overload operator to return a column
    /// Unlike getColumn() it uses direct indexing and not timeseriesNumbers
    double* operator[](uint32_t index);

    void reset();
    void reset(uint32_t width, uint32_t height);
    uint32_t numberOfColumns() const;
    void unloadFromMemory() const;
    void roundAllEntries();
    void resize(uint32_t timeSeriesCount, uint32_t timestepCount);
    void fill(double value);
    void averageTimeseries();

    TS timeSeries;
    TimeSeriesNumbers& timeseriesNumbers;

    static const std::vector<double> emptyColumn; ///< used in getColumn if timeSeries empty
};

} // namespace Antares::Data
#endif /* __ANTARES_LIBS_STUDY_PARTS_COMMON_TIMESERIES_H__ */
