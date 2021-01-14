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
#ifndef __ANTARES_LIBS_ARRAY_CORRELATION_H__
#define __ANTARES_LIBS_ARRAY_CORRELATION_H__

#include <yuni/yuni.h>
#include <assert.h>
#include "matrix.h"
#include "../inifile.h"
#include "../study/fwd.h"
#include "../study/version.h"
#include "../study/fwd.h"

namespace Antares
{
namespace Data
{
class Correlation final
{
public:
    enum Mode
    {
        modeNone = 0,
        modeAnnual = 1,
        modeMonthly = 2,
    };

public:
    /*!
    ** \brief Convert a correlation mode into its string representation
    */
    static const char* ModeToCString(Mode mode);

    /*!
    ** \brief Convert a string into the corresponding mode
    */
    static Mode CStringToMode(const AnyString& str);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Correlation();
    /*!
    ** \brief Destructor
    */
    ~Correlation();
    //@}

    //! \name Mode
    //@{
    /*!
    ** \brief Reset the mode
    */
    void mode(Mode mode);
    //! Get the current mode
    Mode mode() const;
    //@}

    void set(Matrix<>& m, const Area& from, const Area& to, double v);
    template<class StringT>
    void set(Matrix<>& m, const Area& from, const Area& to, const StringT& value);

    //! \name Load / Save
    //@{
    /*!
    ** \brief Reset the matrices
    */
    void reset(Study& study);

    /*!
    ** \brief Load correlated values from an INI file
    **
    ** \param study The attached study
    ** \param filename The INI file
    ** \param warnings True to enable warnings
    ** \param version Format version expected. Use studyVersionUnknown to use the format version
    **   of the given study
    */
    bool loadFromFile(Study& study,
                      const AnyString& filename,
                      bool warnings = true,
                      int version = versionUnknown);

    /*!
    ** \brief Load correlated values from an INI structure in memory
    **
    ** \param study The attached study
    ** \param ini The INI structure
    ** \param warnings True to enable warnings
    ** \param version Format version expected. Use studyVersionUnknown to use the format version
    **   of the given study
    */
    bool loadFromINI(Study& study,
                     const IniFile& ini,
                     bool warnings = true,
                     int version = versionUnknown);

    /*!
    ** \brief Save the correlation matrices into an INI file
    */
    bool saveToFile(Study& study, const AnyString& filename) const;

    /*!
    ** \brief Clear all data
    */
    void clear();
    //@}

    /*!
    ** \brief Copy correlation coefficients from another correlation grid
    **
    ** \param source The correlation matrix source
    ** \param studySource The associated study for the given correlation grid
    ** \param areaSource Name of the area to import
    ** \param mapping Area's name Mapping from the source to the target
    ** \param study Study for the local correlation grid
    */
    void copyFrom(const Correlation& source,
                  const Study& studySource,
                  const AreaName& areaSource,
                  const Area::NameMapping& mapping,
                  const Study& study);

    /*!
    ** \brief
    */
    void retrieveMontlyMatrixArray(const Matrix<>* array[12]) const;

    /*!
    ** \brief Get the amount of memory used the correlation matrices
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Estimate
    */
    void estimateMemoryUsage(StudyMemoryUsage& u) const;

    /*!
    ** \brief Invalidate all matrices
    */
    bool invalidate(bool reload = false) const;

    /*!
    ** \brief Mark as modified
    */
    void markAsModified() const;

public:
    //! The correlation matrix for the whole year
    Matrix<>* annual;
    //! All correlation matrices per month (12)
    Matrix<>* monthly; // [12]

    //! The name to displays in logs
    Yuni::CString<30, false> correlationName;
    //! The associate time-series
    int timeSeries;

private:
    bool internalLoadFromINI(Study& study, const IniFile& ini, bool warnings, int version);
    bool internalLoadFromINIv32(Study& study, const IniFile& ini, bool warnings);
    bool internalLoadFromINIPost32(Study& study, const IniFile& ini, bool warnings);

    void internalSaveToINI(Study& study, Yuni::IO::File::Stream& file) const;

private:
    Mode pMode;

}; // class Correlation

/*!
** \brief Initialize a inter-area correlation maitrix
**
** \param[in,out] m The matrix to initialize
** \param l The list of areas
*/
void InterAreaCorrelationResetMatrix(Matrix<>* m, AreaList* l);

/*!
** \brief Load an inter-area correlation matrix from a file
** \ingroup interareacorrelation
**
** The matrix will be a matrix N*N, where N is the count of
** areas. The routine InterAreaCorrelationResetMatrix() will be
** called. There is no need to initialize the matrix yourself.
**
** \param[in,out] m The matrix to (resize then) fill
** \param l The list of area
** \param filename The filename to use to load values
** \return A non-zero value if the matrix has been loaded, 0 otherwise
*/
int InterAreaCorrelationLoadFromFile(Matrix<>* m, AreaList* l, const char filename[]);

/*!
** \brief Save an inter-area correlation matrix to a file
** \ingroup interareacorrelation
**
** The matrix must have the good dimensions, otherwise nothin will be
** done (N*N with N = count of areas).
**
** \param m The matrix to save
** \param l The list of area
** \param filename The filename to use to values
** \return A non-zero value if the matrix has been saved, 0 otherwise
*/
int InterAreaCorrelationSaveToFile(const Matrix<>* m, const AreaList* l, const char filename[]);

/*!
** \brief Load an inter-area correlation matrix from a Ini file structure
** \ingroup interareacorrelation
**
** The matrix will be a matrix N*N, where N is the count of
** areas. The routine InterAreaCorrelationResetMatrix() will be
** called. There is no need to initialize the matrix yourself.
**
** \param[in,out] m The matrix to (resize then) fill
** \param l The list of area
** \param ini The ini file structure
** \param warnings A non-zero value to enable warnings
** \return A non-zero value if the matrix has been loaded, 0 otherwise
*/
int InterAreaCorrelationLoadFromIniFile(Matrix<>* m, AreaList* l, IniFile* ini, int warnings);

/*!
** \brief Save an inter-area correlation matrix to a Ini file structure
** \ingroup interareacorrelation
**
** The matrix must have the good dimensions, otherwise nothin will be
** done (N*N with N = count of areas).
**
** \param m The matrix to save
** \param l The list of area
** \return An Ini file structure
*/
IniFile* InterAreaCorrelationSaveToIniFile(const Matrix<>* m, const AreaList* l);

} // namespace Data
} // namespace Antares

#include "../study/area.h"
#include "correlation.hxx"

#endif /* __ANTARES_LIBS_ARRAY_CORRELATION_H__ */
