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
#ifndef __ANTARES_LIBS_ARRAY_MATRIX_H__
# define __ANTARES_LIBS_ARRAY_MATRIX_H__

# include <yuni/yuni.h>
# include <yuni/io/file.h>
# include <assert.h>
# include "../jit.h"
# include "../constants.h"
# include <set>
# include "../memory/memory.h"
# include "../study/fwd.h"
# include "autoflush.h"

# include <yuni/core/string.h>


namespace Antares
{

	/*!
	** \brief A n-by-n matrix
	**
	** \ingroup matrix
	** \tparam T          A pod type for each cell of the matrix
	** \tparam ReadWriteT The type to use when reading/saving the matrix
	*/
	template<class T = double, class ReadWriteT = T>
	class Matrix final
	{
	public:
		//! Type
		typedef T Type;
		//! Pointer
		typedef T* TypePtr;
		//! Matrix type
		typedef Matrix<T, ReadWriteT> MatrixType;

		//! Read / Write type
		typedef ReadWriteT ReadWriteType;

		//! Pointer
		typedef Matrix<T>* MatrixPtr;
		//! Vector
		typedef std::set<MatrixPtr> Vector;

		//! Column type
		typedef typename Antares::Memory::Stored<T>::Type ColumnType;

		//! A buffer, for large amount of data
		typedef Yuni::Clob BufferType;

		/*!
		** \brief Options when loading a file
		*/
		enum Options
		{
			//! None
			optNone = 0,
			//! The matrix can not see its size modified
			optFixedSize = 1,
			//! Do not produce warnings/errors
			optQuiet = 2,
			//! Do not postpone the loading
			optImmediate = 4,
			//! mark the matrix as modified after loading
			optMarkAsModified = 8,
			//! Do not warn if the file is empty
			optNoWarnIfEmpty = 16,
			//! The loading never fails
			optNeverFails = 32,
		};

		enum
		{
			//! A Hard-coded maximum filesize
			filesizeHardLimit = 1536 * 1024 * 1024, // 1.5Go
		};

				/*!
		** \brief Estimate
		*/
		static void EstimateMemoryUsage(Antares::Data::StudyMemoryUsage& u, uint w, uint h);

	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		Matrix();
		/*!
		** \brief Copy constructor
		*/
		Matrix(const Matrix& rhs);

		/*!
		** \brief Copy constructor
		*/
		template<class U, class V> Matrix(const Matrix<U,V>& rhs);

		/*!
		** \brief Constructor with a initial size
		*/
		Matrix(uint w, uint h);
		//! Destructor
		~Matrix();
		//@}


		//! \name Copy / Paste
		//@{
		/*!
		** \brief Copy values from another matrix
		*/
		template<class U, class V> void copyFrom(const Matrix<U,V>& rhs);

		template<class U, class V> void copyFrom(const Matrix<U,V>* rhs);
		//@}


		//! \name File manipulation
		//@{
		/*!
		** \brief Load entries from a CSV file
		**
		** The data might not be actually loaded, if the load-on-demand is
		** enabled.
		** If the param `fixedSize` is false, the number of columns will be
		** automatically detected from the first row in the CSV file.
		**
		** \param filename A filename to a CSV file
		** \param minWidth The minimum allowed width
		** \param maxHeight The new height
		** \param fixedSize True to not automatically determine the width of the matrix
		** \param warning True to produce warnings when an error occurs
		** \param immediate True to not postpone the loading
		** \param buffer An optional buffer for reading the file
		** \return True if the operation succeeded
		*/
		bool loadFromCSVFile(const AnyString& filename, uint minWidth, uint maxHeight,
			uint options = optNone, BufferType* buffer = NULL);

		bool loadFromCSVFile(const AnyString& filename, uint minWidth, uint maxHeight,
			BufferType* buffer);

		bool loadFromCSVFile(const AnyString& filename);


		/*!
		** \brief Write the content of a matrix into a single file
		**
		** If JIT is enabled, the matrix will be resized to 0x0 if the write
		** is successful.
		**
		** \param m The matrix
		** \param filename The file where to write data
		** \return A non-zero value if the operation succeeded, 0 otherwise
		*/
		bool saveToCSVFile(const AnyString& filename, uint precision = 6, bool addHint = false) const;

		/*!
		** \brief Write the content of a matrix into a single file
		**
		** If JIT is enabled, the matrix will be resized to 0x0 if the write
		** is successful.
		**
		** \param m         The matrix
		** \param filename  The file where to write data
		** \param predicate A predicate for modyfing values on the fly
		** \return A non-zero value if the operation succeeded, 0 otherwise
		*/
		template<class PredicateT>
		bool saveToCSVFile(const AnyString& filename, uint precision, bool addHint, PredicateT& predicate) const;
		//@}


		//! \name Operations on columns and rows
		//@{
		/*!
		** \brief Resize the matrix
		**
		** All data will be lost in the process.
		** \param w The new width
		** \param h The new height
		*/
		void resize(uint w, uint h, bool fixedSize = false);

		/*!
		** \brief Resize the matrix without destroying its content
		*/
		void resizeWithoutDataLost(uint x, uint y, const T& defVal = T());

		/*!
		** \brief Empty the matrix
		*/
		void clear();

		/*!
		** \brief Empty the matrix and mark it as modified
		*/
		void reset();

		/*!
		** \brief Resize a matrix and reset its values
		**
		** \param height The height of the matrix
		** \see resize()
		** \see zero()
		*/
		void reset(uint w, uint h, bool fixedSize = false);


		/*!
		** \brief Flush the content of the whole matrix into the swap files
		**
		** This will reduce the memory usage
		*/
		void flush() const;

		//! Get the Nth column
		ColumnType& column(uint n);
		//! Get the Nth column (const)
		const ColumnType&  column(uint n) const;


		/*!
		** \brief Make the matrix a zero matrix
		*/
		void zero();

		/*!
		** \brief Fill the matrix with a given value
		*/
		void fill(const T& v);

		/*!
		** \brief Make the matrix an unit matrix (identity matrix)
		*/
		void fillUnit();

		/*!
		** \brief Multiply all entries by a given value
		*/
		template<class U> void multiplyAllEntriesBy(const U& c);

		/*!
		** \brief Multiply or divide a column by a given value
		*/
		template<class U> void multiplyColumnBy(uint x, const U& c);
		template<class U> void divideColumnBy(uint x, const U& c);

		/*!
		** \brief Compute the average of all timeseries (derated mode)
		*/
		void averageTimeseries(bool roundValues = true);

		/*!
		** \brief Round all entries
		*/
		void roundAllEntries();


		/*!
		** \brief Find the lower bound
		*/
		T findLowerBound() const;

		/*!
		** \brief Find the upper bound
		*/
		T findUpperBound() const;

		/*!
		** \brief Make all entries absolute
		*/
		void makeAllEntriesAbsolute();

		/*!
		** \brief Copy values into a given column in the matrix
		**
		** \param x The column index (zero-based)
		** \param data The data to copy
		*/
		template<class U> void pasteToColumn(uint x, const U* data);

		/*!
		** \brief Copy values into a given column in the matrix
		**
		** \param x The column index (zero-based)
		** \param data The data to copy
		*/
		template<class U> void pasteToColumn(uint x, const Antares::Memory::Array<U>& data);

		/*!
		** \brief Set a entire column with a given value
		**
		** \param x The column index (zero-based)
		** \param data The data to copy
		*/
		void fillColumn(uint x, const T& value);

		/*!
		** \brief Set to zero a entire column
		**
		** \param x The column index (zero-based)
		*/
		void columnToZero(uint x);

		/*!
		** \brief Get if the matrix only contains zero
		*/
		bool containsOnlyZero() const;

		/*!
		** \brief Get if the matrix only contains zero
		**
		** \param predicate A predicate to modify the values on the fly
		*/
		template<class PredicateT>
		bool containsOnlyZero(PredicateT& predicate) const;

		/*!
		** \brief Shift all rows
		*/
		void circularShiftRows(uint count);

		/*!
		** \brief Shift all rows of a specific column
		*/
		void circularShiftRows(uint column, uint count);
		//@}


		//! \name Memory Management
		//@{
		/*!
		** \brief Force the Load of data (if not done) for the next save and mark the matrix as modified
		**
		** This method is only useful if the load-on-demand is used.
		** The matrix will be marked as modified to force the written.
		**
		** \param reload True to load all JIT data
		** \return True if the data was really loaded (reload = true)
		*/
		bool invalidate(bool reload = false) const;

		/*!
		** \brief Try to remove from memory all data from the matrix
		**
		** This is possible only when enough informations is provided by the JIT
		** structure and when the matrix is not modified
		*/
		void unloadFromMemory() const;

		/*!
		** \brief Try to estimate the memory footprint used by the matrix
		*/
		void estimateMemoryUsage(Antares::Data::StudyMemoryUsage& u) const;

		/*!
		** \brief Try to estimate the memory footprint used by the matrix (conditional)
		**
		** If the condition is true, the given size will be used instead of the current one.
		** This method is used estimate the memory footprint when time-series are enabled/disabled.
		*/
		void estimateMemoryUsage(Antares::Data::StudyMemoryUsage& u, bool cond, uint w, uint h) const;

		/*!
		** \brief Mark the matrix as modified
		**
		** This routine has no effect when the UI is not present
		*/
		void markAsModified() const;

		/*!
		** \brief Get if the matrix is empty
		**
		** This method is equivalent to :
		** \code
		** if (!matrix.width() || !matrix.height())
		**     ; // empty
		** \endcode
		*/
		bool empty() const;

		//! Get the amount of memory used by the matrix
		Yuni::uint64 memoryUsage() const;

		//! Get the amount of memory used by the matrix
		Yuni::uint64 valuesMemoryUsage() const;
		//@}


		/*!
		** \brief Print the matrix to std::cout (debug)
		*/
		void print() const;

		//! \name Operators
		//@{
		//! Assignement
		Matrix& operator = (const Matrix& rhs);
		//! Assignement
		template<class U> Matrix& operator = (const Matrix<U>& rhs);
		//! operator []
		ColumnType& operator [] (uint column);
		const ColumnType& operator [] (uint column) const;
		//@}

	public:
		//! Width of the matrix
		mutable uint width;
		//! Height of the matrix
		mutable uint height;
		//! All entries of the matrix (bidimensional array)
		mutable ColumnType* entry;
		//! Just-in-time informations
		mutable JIT::Informations* jit;

		#ifdef TESTING
		mutable Yuni::Clob data;
		#endif


	private:
		struct PredicateIdentity
		{
			template<class U> inline U operator () (const U& value) const
			{
				return value;
			}
		};
	private:
		/*!
		** \brief Load data from a CSV file
		*/
		bool internalLoadCSVFile(const AnyString& filename,
			uint minWidth, uint maxHeight, uint options,
			BufferType* buffer = NULL);

		//! Initialize the JIT structures and returns true
		bool internalLoadJITData(const AnyString& filename,
			uint minWidth, uint maxHeight, uint options);

		/*!
		** \brief Save data to a CSV file
		*/
		template<class PredicateT>
		bool internalSaveCSVFile(const AnyString& filename, uint precision, bool addHint,
			PredicateT& predicate) const;

		template<class PredicateT>
		void internalSaveToFileDescriptor(Yuni::Clob& data, uint precision,
			bool addHint, PredicateT& predicate) const;

		bool loadFromBuffer(const AnyString& filename, BufferType& data,
			uint minWidth, uint maxHeight, const int fixedSize, uint options);

		/*!
		** \brief Make sure that all JIT Data are loaded into memory
		*/
		bool loadAllJITData() const;

		/*!
		** \brief Reverse all values for a specific column
		*/
		void reverseRows(uint column, uint start, uint end);

	}; // class Matrix





	template<class T>
	class MatrixSubColumn
	{
		// typedef <sub column>  Type;
		// typedef const <sub column>  Type;
	};


	template<class U>
	class MatrixSubColumn<U**>
	{
	public:
		typedef U*  Type;
		typedef const U* ConstType;
	};

	template<>
	class MatrixSubColumn<Matrix<double>::ColumnType*>
	{
	public:
		typedef Matrix<double>  MatrixType;
		typedef MatrixType::ColumnType& Type;
		typedef const MatrixType::ColumnType& ConstType;
	};

	template<>
	class MatrixSubColumn<Matrix<float>::ColumnType*>
	{
	public:
		typedef Matrix<float>  MatrixType;
		typedef MatrixType::ColumnType& Type;
		typedef const MatrixType::ColumnType& ConstType;
	};






	/*!
	** \brief Test if all the values in the matrix are positive (>= 0)
	**
	** \param m The matrix
	** \return A non-zero value if the test succeeded, 0 otherwise
	*/
	int MatrixTestForPositiveValues(const char* msg, const Matrix<>* m);

	/*!
	** \brief Test if all the values in the matrix are higher than an arbitrary number
	**
	** \param m The matrix
	** \param value The minimum value
	** \return A non-zero value if the test succeeded, 0 otherwise
	*/
	int MatrixTestIfValuesAreHigherThan(const char* msg, const Matrix<>* m, const double value);

	/*!
	** \brief Test if all the values in the matrix are lower than an arbitrary number
	**
	** \param m The matrix
	** \param value The maximum value
	** \return A non-zero value if the test succeeded, 0 otherwise
	*/
	int MatrixTestIfValuesAreLowerThan(const char* msg, const Matrix<>* m, const double value);


	/*!
	** \brief Test if all the values in the matrix are negative (<= 0)
	**
	** \param m The matrix
	** \return A non-zero value if the test succeeded, 0 otherwise
	*/
	int MatrixTestForNegativeValues(const char* msg, const Matrix<>* m);


	/*!
	** \brief Test if all the values in the matrix are positive (>= 0)
	**
	** \param m The matrix
	** \return A non-zero value if the test succeeded, 0 otherwise
	*/
	int MatrixTestForPositiveValues_LimitWidth(const char* msg, const Matrix<>* m, uint maxWidth);

	/*!
	** \brief Test if there is at least one positive value
	**
	** \param m The matrix
	** \return true if the test succeeded, false otherwise
	*/
	template<class T1, class T2>
	bool MatrixTestForAtLeastOnePositiveValue(const Matrix<T1, T2>& m);



	void MatrixEstimateMemoryUsage(size_t sizeofT, Antares::Data::StudyMemoryUsage& u, uint w, uint h);

	void MatrixEstimateMemoryUsageFromJIT(size_t sizeofT, Antares::Data::StudyMemoryUsage& u, JIT::Informations* jit);





} // namespace Antares

# include "matrix.hxx"

#endif // __ANTARES_LIBS_ARRAY_MATRIX_H__
