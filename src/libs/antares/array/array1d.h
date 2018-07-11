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
#ifndef __ANTARES_LIBS_ARRAY_ARRAY_1D_H__
# define __ANTARES_LIBS_ARRAY_ARRAY_1D_H__



/*!
** \defgroup array1d 1D-Array
** \ingroup toolbox
*/




/*!
** \brief Load an integer 1D array from a file
** \ingroup array1d
**
** \param filename The filename to read
** \param[out] data The data structure to fill
** \param height The size of the structure
** \return A non-zero value if the operation was successful, 0 otherwise
*/
int Array1DLoadFromFile(const char filename[], double data[], const int height);



/*!
** \brief Save a 1D-Array into a file
** \ingroup array1d
**
** \param filename The target filename
** \param data The array to store
** \param count The number of items in the array
** \return A non-zero value if the operation was successful, 0 otherwise
*/
int Array1DSaveToFile(const char filename[], const double data[], const int count, uint precision = 6);


/*!
** \brief Check if all values are strictly positive
**
** \param msg The message to put in logs if an error occurs
** \param data The 1D-array
** \param count The size of the array
** \return A non-zero value if all values are strictly positive
*/
int Array1DCheckStrictlyPositiveValues(const char msg[], const double data[], const int count);


/*!
** \brief Check if all values are positive or null
**
** \param msg The message to put in logs if an error occurs
** \param data The 1D-array
** \param count The size of the array
** \return A non-zero value if all values are strictly positive
*/
int Array1DCheckPositiveValues(const char msg[], const double data[], const int count);

/*!
** \brief Check if all values are negative or null
**
** \param msg The message to put in logs if an error occurs
** \param data The 1D-array
** \param count The size of the array
** \return A non-zero value if all values are strictly positive
*/
int Array1DCheckNegativeValues(const char msg[], const double data[], const int count);


/*!
** \brief Check if all values are within a range
**
** \param msg The message to put in logs if an error occurs
** \param data The 1D-array
** \param count The size of the array
** \param minV The minimum value
** \param maxV The maximum value
** \return A non-zero value if all values are strictly positive
*/
int Array1DCheckForValuesInRange(const char msg[], const double data[], const int count,
	const double minV, const double maxV);


/*!
** \brief Check if all values are greater than or equal to a given number
**
** \param msg The message to put in logs if an error occurs
** \param data The 1D-array
** \param count The size of the array
** \return A non-zero value if all values are strictly positive
*/
int Array1DCheckGreaterOrEqual(const char msg[], const double data[], const int count, const double v);



/*!
** \brief Set all values of a 1D-Array
**
** \param data The 1D-array
** \param count The size of the array
*/
void Array1DSetValues(double data[], const int count, const double value);



#endif /* __ANTARES_LIBS_ARRAY_ARRAY_1D_H__ */
