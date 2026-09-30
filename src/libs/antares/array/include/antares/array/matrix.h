// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_ARRAY_MATRIX_H__
#define __ANTARES_LIBS_ARRAY_MATRIX_H__

#include <cassert>
#include <set>
#include <span>
#include <string>
#include <vector>

namespace Antares
{
/*!
** \brief A n-by-n matrix
**
** \ingroup matrix
** \tparam T A pod type for each cell of the matrix
*/
template<class T = double>
class Matrix
{
public:
    //! Type
    using Type = T;
    //! Pointer
    using TypePtr = T*;
    //! Matrix type
    using MatrixType = Matrix<T>;

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
    template<class U>
    Matrix(const Matrix<U>& rhs);

    /*!
    ** \brief Constructor with a initial size
    */
    Matrix(unsigned int w, unsigned int h);
    //! Destructor
    ~Matrix() = default;
    //@}

    //! \name Copy / Paste
    //@{
    /*!
    ** \brief Copy values from another matrix
    */
    template<class U>
    void copyFrom(const Matrix<U>& rhs);

    template<class U>
    void copyFrom(const Matrix<U>* rhs);
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
    void resize(unsigned int w, unsigned int h);

    /*!
    ** \brief Resize the matrix without destroying its content
    */
    void resizeWithoutDataLost(unsigned int x, unsigned int y, const T& defVal = T());

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
    void reset(unsigned int w, unsigned int h);

    //! Get the Nth column
    ColumnType& column(unsigned int n);
    //! Get the Nth column (const)
    const ColumnType& column(unsigned int n) const;

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
    void pasteToColumn(unsigned int x, const U* data);

    template<class U>
    void pasteToColumn(unsigned int x, const std::vector<U>& data)
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
    void fillColumn(unsigned int x, const T& value);

    /*!
    ** \brief Set to zero a entire column
    **
    ** \param x The column index (zero-based)
    */
    void columnToZero(unsigned int x);

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

    unsigned int width() const noexcept;
    unsigned int height() const noexcept;

    //! \name Operators
    //@{
    //! Assignement
    Matrix& operator=(const Matrix& rhs);

    Matrix& operator=(Matrix&& rhs) noexcept;

    //! Assignement
    template<class U>
    Matrix& operator=(const Matrix<U>& rhs);

    //! operator []
    ColumnType& operator[](unsigned int column);
    const ColumnType& operator[](unsigned int column) const;
    //@}

public:
    //! Width of the matrix
    ColumnType& mutableColumn(unsigned int column) const;

    struct PredicateIdentity
    {
        template<class U = Type>
        inline U operator()(const U& value) const
        {
            return value;
        }
    };

private:
    unsigned int width_ = 0;
    unsigned int height_ = 0;
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
template<class T>
bool MatrixTestForAtLeastOnePositiveValue(const Matrix<T>& m);

} // namespace Antares

#include "matrix.hxx"

#endif // __ANTARES_LIBS_ARRAY_MATRIX_H__
