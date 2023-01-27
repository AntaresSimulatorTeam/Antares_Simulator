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
#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_H__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_H__

#include <yuni/yuni.h>
#include <antares/study/xcast/xcast.h>
#include <antares/array/correlation.h>

namespace Antares
{
namespace Solver
{
namespace TSGenerator
{
namespace XCast
{
class StudyData final
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief
    */
    StudyData();
    /*!
    ** \brief Destructor
    */
    ~StudyData();
    //@}

    /*!
    ** \brief Load all informations from a study and a predicate
    **
    ** Example of a predicate:
    ** \code
    ** struct WindPredicate
    ** {
    ** 	bool operator () (const Area&) const
    ** 	{
    ** 		return true;
    ** 	}
    **
    ** 	Data::XCast* retrieveXCastData(const Area* area) const
    ** 	{
    ** 		return area->wind.prepro->xcast;
    ** 	}
    ** };
    ** \endcode
    **
    ** All areas excluded from the list will see their time-series matrix reset to 1x8760.
    **
    ** \param study A study
    ** \param predicate Predicate to determine whether the specified object meets some criteria
    **   and to retrive some specific data from the area
    */
    template<class PredicateT>
    void loadFromStudy(Data::Study& study,
                       const Data::Correlation& correlation,
                       PredicateT& predicate);

public:
    //! List of all areas (sub-set of the complete list)
    Data::Area::Vector localareas;
    //! Correlation coefficients for each month
    const Matrix<float>* correlation[12];
    /*!
    ** \brief Correlation mode (monthly / annual)
    **
    ** \internal This value is a copy from the correlation data and
    **   it's used to properly delete the coefficient matrices.
    */
    Data::Correlation::Mode mode;

private:
    //! Delete
    //! Rebuild data from our own area list
    void reloadDataFromAreaList(const Data::Correlation& correlation);

    void prepareMatrix(Matrix<float>& m, const Matrix<float>& source) const;

}; // class StudyData

} // namespace XCast
} // namespace TSGenerator
} // namespace Solver
} // namespace Antares

#include "studydata.hxx"

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_H__
