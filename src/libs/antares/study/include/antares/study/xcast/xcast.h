/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_LIBS_STUDY_XCAST_XCAST_H__
#define __ANTARES_LIBS_STUDY_XCAST_XCAST_H__

#include <vector>

#include <yuni/yuni.h>
#include <yuni/core/noncopyable.h>

#include <antares/array/matrix.h>

namespace Antares::Data
{
class XCast final: private Yuni::NonCopyable<XCast>
{
public:
    //! Vector
    using Vector = std::vector<XCast*>;
    //! Vector with the const qualifier
    using VectorConst = std::vector<const XCast*>;

    /*!
    ** \brief All coefficients
    */
    enum
    {
        dataCoeffAlpha = 0,
        dataCoeffBeta,
        dataCoeffGamma,
        dataCoeffDelta,
        dataCoeffTheta,
        dataCoeffMu,
        //! The maximum number of coefficients
        dataMax,
    };

    /*!
    ** \brief All available probability distribution
    */
    enum Distribution
    {
        //! None
        dtNone = 0,
        //! The uniform distribution
        dtUniform = 1,
        //! The Beta distribution
        dtBeta = 2,
        //! The normal distribution
        dtNormal = 3,
        //! The Weibul distribution, of shape A
        dtWeibullShapeA = 4,
        //! The Gamma distribution, of shape A
        dtGammaShapeA = 5,
        //! The maximum number of distributions
        dtMax
    };

    enum
    {
        //! Hard limit for the number of points of the transfer function
        conversionMaxPoints = 100,
    };

    enum TSTranslationUse
    {
        //! Do not use the time-series average
        tsTranslationNone = 0,
        //! Add the time-series average before computing the transfer function
        tsTranslationBeforeConversion,
        //! Add the time-series average after computing the transfer function
        tsTranslationAfterConversion,
    };

public:
    /*!
    ** \brief Convert a distribution into its human readable representation
    */
    static const char* DistributionToCString(Distribution d);

    /*!
    ** \brief Convert a CString into a probability distribution
    */
    static Distribution StringToDistribution(AnyString str);

    /*!
    ** \brief Convert a probability distribution into its string id representation
    */
    static const char* DistributionToNameID(Distribution d);

    /*!
    ** \brief How to use the timeseries average
    */
    static const char* TSTranslationUseToCString(TSTranslationUse use);

    static TSTranslationUse CStringToTSTranslationUse(const AnyString& str);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    XCast(TimeSeriesType ts);
    /*!
    ** \brief Destructor
    */
    ~XCast();
    //@}

    /*!
    ** \brief Reset to default values
    */
    void resetToDefaultValues();

    /*!
    ** \brief Copy settings from another XCast struct
    */
    void copyFrom(const XCast& rhs);

    /*!
    ** \brief Load data from a folder
    */
    bool loadFromFolder(const AnyString& folder);

    /*!
    ** \brief Save data to a folder
    */
    bool saveToFolder(const AnyString& folder) const;

    //! \name Memory management
    //@{
    /*!
    ** \brief Make sure that all data are loaded in memory
    */
    bool forceReload(bool reload = false) const;

    /*!
    ** \brief Mark the load data as modified
    */
    void markAsModified() const;

    /*!
    ** \brief Get the amount of memory currently used by the XCast data
    */
    uint64_t memoryUsage() const;

    /*!
    ** \brief Estimate the amount of memory required by this class for a simulation
    */

    //@}

public:
    /*!
    ** \brief Data required for XCast: coefficients (coeffMax x 12)
    */
    Matrix<float> data;

    /*!
    ** \brief K (12x24)
    */
    Matrix<float> K;

    /*!
    ** \brief Time-series average
    */
    Matrix<int32_t> translation;

    //! How to use the timeseries average
    TSTranslationUse useTranslation;

    //! The probability distribution to use
    Distribution distribution;

    //! The installed capacity
    double capacity;

    //! True to use the transfer function after the generation of the time-series
    bool useConversion;
    /*!
    ** \brief The conversion function
    **
    ** The first row gives the X-Coordinates, the second one the Y-Coordinates.
    */
    Matrix<float> conversion;

    //! The related time-series
    const TimeSeriesType timeSeries;

protected:
    void resetTransferFunction();

}; // class XCast

} // namespace Antares::Data

#include "xcast.hxx"

#endif // __ANTARES_LIBS_STUDY_XCAST_XCAST_H__
