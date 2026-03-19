// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_H__
#define __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_H__

#include <yuni/yuni.h>

#include <antares/correlation/correlation.h>
#include <antares/study/xcast/xcast.h>

namespace Antares::TSGenerator::XCast
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
    ~StudyData() = default;
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
    std::array<Matrix<float>, 12> correlation;
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

} // namespace Antares::TSGenerator::XCast

#include "studydata.hxx"

#endif // __ANTARES_SOLVER_TS_GENERATOR_XCAST_STUDY_DATA_H__
