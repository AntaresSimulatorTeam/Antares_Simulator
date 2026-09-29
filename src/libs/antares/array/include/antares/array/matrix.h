// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_H__

#include <cassert>
#include <span>
#include <set>
#include <vector>

#include <yuni/yuni.h>
#include <string>

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
class Matrix
{
public:
    //! Type
    using Type = T;
    //! Pointer
    using TypePtr = T*;
    //! Matrix type
    using MatrixType = Matrix<T, ReadWriteT>;

    //! Read / Write type
    using ReadWriteType = ReadWriteT;

    //! Pointer
    using MatrixPtr = Matrix<T>*;

    //! Column type
    using ColumnType = std::vector<T>;
    using ColumnView = std::span<T>;
    using ConstColumnView = std::span<const T>;

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
        //! Do not warn if the file is empty
        optNoWarnIfEmpty = 16,
        //! The loading never fails
        optNeverFails = 32,
    };

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
    ** \brief Move constructor
    */
    Matrix(Matrix&& rhs) noexcept;

    /*!
    ** \brief Copy constructor
    */
    template<class U, class V>
    Matrix(const Matrix<U, V>& rhs);

    /*!
    ** \brief Constructor with a initial size
    */
    Matrix(uint w, uint h);
    //! Destructor
    ~Matrix() = default;
    //@}

    //! \name Copy / Paste
    //@{
    /*!
    ** \brief Copy values from another matrix
    */
    template<class U, class V>
    void copyFrom(const Matrix<U, V>& rhs);

    template<class U, class V>
    void copyFrom(const Matrix<U, V>* rhs);
    //@}

    //@{
    /*!
    ** \brief Swap contents of Matrix with another
    */
    void swap(MatrixType& rhs) noexcept;
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
    void resize(uint w, uint h);

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
    void reset(uint w, uint h);

    //! Get the Nth column
    ColumnType& column(uint n);
    //! Get the Nth column (const)
    const ColumnType& column(uint n) const;

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
    template<class U>
    void multiplyAllEntriesBy(const U& c);

    /*!
    ** \brief Compute the average of all timeseries (derated mode)
    */
    void averageTimeseries(bool roundValues = true);

    /*!
    ** \brief Round all entries
    */
    void roundAllEntries();

    /*!
    ** \brief Copy values into a given column in the matrix
    **
    ** \param x The column index (zero-based)
    ** \param data The data to copy
    */
    template<class U>
    void pasteToColumn(uint x, const U* data);

    template<class U>
    void pasteToColumn(uint x, const std::vector<U>& data)
    {
        assert(data.size() == height_);
        pasteToColumn(x, data.data());
    }

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

    //! \name Memory Management
    //@{
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

    uint width() const noexcept;
    uint height() const noexcept;

    //! \name Operators
    //@{
    //! Assignement
    Matrix& operator=(const Matrix& rhs);

    Matrix& operator=(Matrix&& rhs) noexcept;

    //! Assignement
    template<class U>
    Matrix& operator=(const Matrix<U>& rhs);

    //! operator []
    ColumnType& operator[](uint column);
    const ColumnType& operator[](uint column) const;
    //@}

public:
    //! Width of the matrix
    ColumnType& mutableColumn(uint column) const;

    struct PredicateIdentity
    {
        template<class U = Type>
        inline U operator()(const U& value) const
        {
            return value;
        }
    };

 private:
    uint width_ = 0;
    uint height_ = 0;
    mutable std::vector<ColumnType> columns_;
    /*!
    ** \brief Load data from a CSV file
    */
}; // class Matrix

/*!
** \brief Test if there is at least one positive value
**
** \param m The matrix
** \return true if the test succeeded, false otherwise
*/
template<class T1, class T2>
bool MatrixTestForAtLeastOnePositiveValue(const Matrix<T1, T2>& m);

} // namespace Antares

#include "matrix.hxx"

#endif // __ANTARES_LIBS_ARRAY_MATRIX_H__
