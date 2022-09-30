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
#ifndef __ANTARES_LIBS_ARRAY_MATRIX_HXX__
#define __ANTARES_LIBS_ARRAY_MATRIX_HXX__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/math.h>
#include <logs.h>
#include <utility>
#include <cstdlib>
#include "../io/statistics.h"
#include "matrix-to-buffer.h"

#define ANTARES_MATRIX_CSV_COMMA "\t;,"
#define ANTARES_MATRIX_CSV_SEPARATORS "\t\r\n;,"

namespace Antares
{
namespace // anonymous
{
template<class T>
class MatrixData final
{
public:
    inline static void Init(T& data)
    {
        data = T();
    }
    template<class U>
    inline static void Copy(T& data, const U& value)
    {
        data = static_cast<T>(value);
    }

    inline static void Copy(T&, const AnyString&)
    {
        // to avoid gcc warnings
        // must never be called
        logs.error() << "internal error: matrix data conversion";
    }
};

template<uint ChunkSizeT, bool ExpandableT>
class MatrixData<Yuni::CString<ChunkSizeT, ExpandableT>> final
{
public:
    using StringT = Yuni::CString<ChunkSizeT, ExpandableT>;

public:
    inline static void Init(StringT& data)
    {
        data.clear();
    }
    template<class U>
    inline static void Copy(StringT& data, const U& value)
    {
        // we must avoid stupid static cast (stupid in this case)
        data = value;
    }
};

template<class ReadWriteType>
class MatrixStringConverter final
{
public:
    enum
    {
        direct = 0
    };

public:
    inline static bool Do(const AnyString& str, ReadWriteType& out)
    {
        return str.to(out);
    }
};

template<>
class MatrixStringConverter<double> final
{
public:
    enum
    {
        direct = 0
    };

public:
    inline static bool Do(const AnyString& str, double& out)
    {
        char* pend;
        out = ::strtod(str.c_str(), &pend);
        return (NULL != pend and '\0' == *pend);
    }
};

template<>
class MatrixStringConverter<float> final
{
public:
    enum
    {
        direct = 0
    };

public:
    inline static bool Do(const AnyString& str, float& out)
    {
        char* pend;
        out = static_cast<float>(::strtod(str.c_str(), &pend));
        return (NULL != pend and '\0' == *pend);
    }
};

template<uint ChunkSizeT, bool ExpandableT>
class MatrixStringConverter<Yuni::CString<ChunkSizeT, ExpandableT>> final
{
public:
    enum
    {
        direct = 1,
    };
    using StringT = Yuni::CString<ChunkSizeT, ExpandableT>;

public:
    inline static bool Do(const AnyString& str, StringT& out)
    {
        out.assign(str);
        return true;
    }
};

template<class T, class P>
class MatrixRound final
{
public:
    static T Value(P value)
    {
        return static_cast<T>(Yuni::Math::Trunc(value));
    }
};

template<class T>
class MatrixRound<T, double> final
{
public:
    static T Value(double value)
    {
        return static_cast<T>(value);
    }
};

template<class T>
class MatrixRound<T, float> final
{
public:
    static T Value(float value)
    {
        return static_cast<T>(value);
    }
};

} // anonymous namespace

template<class T, class ReadWriteT>
inline Matrix<T, ReadWriteT>::Matrix() : width(0), height(0), entry(nullptr), jit(nullptr)
{
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::Matrix(uint w, uint h) : width(w), height(h), jit(nullptr)
{
    if (0 == width or 0 == height)
    {
        entry = nullptr;
    }
    else
    {
        entry = new typename Antares::Memory::Stored<T>::Type[w + 1];
        entry[w] = nullptr;

        for (uint i = 0; i != w; ++i)
            Antares::Memory::Allocate<T>(entry[i], h);
    }
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::Matrix(const Matrix<T, ReadWriteT>& rhs) :
 width(rhs.width), height(rhs.height), jit(nullptr)
{
    if (0 == width or 0 == height)
    {
        entry = nullptr;
        width = 0;
        height = 0;
    }
    else
    {
        entry = new typename Antares::Memory::Stored<T>::Type[width + 1];
        entry[width] = nullptr;

        MatrixAutoFlush<MatrixType> autoflush(*this);
        MatrixAutoFlush<MatrixType> autoflushRhs(rhs);
        for (uint i = 0; i != rhs.width; ++i)
        {
            ++autoflush;
            ++autoflushRhs;

            Antares::Memory::Allocate<T>(entry[i], height);
            memcpy(entry[i], rhs.entry[i], sizeof(T) * height);
        }
    }
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::Matrix(Matrix<T, ReadWriteT>&& rhs) noexcept
{
    // use Matrix::operator=(Matrix&& rhs)
    *this = std::move(rhs);
}

template<class T, class ReadWriteT>
template<class U, class V>
Matrix<T, ReadWriteT>::Matrix(const Matrix<U, V>& rhs) :
 width(0), height(0), entry(nullptr), jit(nullptr)
{
    copyFrom(rhs);
}

template<class T, class ReadWriteT>
Matrix<T, ReadWriteT>::~Matrix()
{
    assert((JIT::enabled or (jit == NULL))
           and "Internal variable jit is set but JIT is not globally enabled (overflow?)");
    delete jit;

    if (entry)
    {
        for (uint i = 0; i != width; ++i)
            Antares::Memory::Release(entry[i]);
        delete[] entry;
    }
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::zero()
{
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint i = 0; i != width; ++i)
    {
        ++autoflush;
        ColumnType& column = entry[i];

#ifdef ANTARES_SWAP_SUPPORT
        column.assign(height);
#else
        (void)::memset((void*)column, 0, sizeof(T) * height);
#endif
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::averageTimeseries(bool roundValues)
{
    if (width > 1)
    {
        ColumnType& first = entry[0];

        // add the values of each timeseries to the first one
        for (uint i = 1; i != width; ++i)
        {
            ColumnType& column = entry[i];
            for (uint j = 0; j != height; ++j)
                first[j] += column[j];
        }

        // average
        double coeff = 1. / width;
        if (roundValues)
        {
            for (uint j = 0; j != height; ++j)
            {
                const double d = first[j] * coeff;
                first[j] = Yuni::Math::Round(d);
            }
        }
        else
        {
            for (uint j = 0; j != height; ++j)
                first[j] *= coeff;
        }

        // Release all timeseries no longer needed
        for (uint i = 1; i != width; ++i)
            Antares::Memory::Release(entry[i]);
        // reset the width to 1
        width = 1;
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::fill(const T& v)
{
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint i = 0; i != width; ++i)
    {
        ++autoflush;
        ColumnType& column = entry[i];
#ifdef ANTARES_SWAP_SUPPORT
        column.assign(height, v);
#else
        for (uint j = 0; j != height; ++j)
            column[j] = v;
#endif
    }
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::fillUnit()
{
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint i = 0; i != width; ++i)
    {
        ++autoflush;
        ColumnType& column = entry[i];

#ifdef ANTARES_SWAP_SUPPORT
        column.assign(height);
#else
        (void)::memset((void*)column, 0, sizeof(T) * height);
#endif

        column[i] = T(1);
    }
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::invalidate(bool reload) const
{
    JIT::Invalidate(jit);
    return (reload) ? loadAllJITData() : true;
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::reset(uint w, uint h, bool fixedSize)
{
    resize(w, h, fixedSize);
    zero();
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::internalLoadJITData(const AnyString& filename,
                                                uint minWidth,
                                                uint maxHeight,
                                                uint options)
{
    // To avoid undefined behavior when filename is jit->sourceFilename
    // we have to make a copy first.
    jit = JIT::Reset(jit, YString(filename));

    JIT::MarkAsNotLoaded(jit);
    clear();
    jit->minWidth = minWidth;
    jit->maxHeight = maxHeight;
    jit->options = options;
    return true;
}

template<class T, class ReadWriteT>
inline bool Matrix<T, ReadWriteT>::loadFromCSVFile(const AnyString& filename)
{
    return loadFromCSVFile(
      filename, 1, 0, optImmediate | optNoWarnIfEmpty | optNeverFails | optQuiet);
}

template<class T, class ReadWriteT>
inline bool Matrix<T, ReadWriteT>::loadFromCSVFile(const AnyString& filename,
                                                   uint minWidth,
                                                   uint maxHeight,
                                                   BufferType* buffer)
{
    return loadFromCSVFile(filename, minWidth, maxHeight, optNone, buffer);
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::loadFromCSVFile(const AnyString& filename,
                                            uint minWidth,
                                            uint maxHeight,
                                            uint options,
                                            BufferType* buffer)
{
    assert(not filename.empty() and "Matrix<>:: loadFromCSVFile: empty filename");
    // As the loading might be expensive, especially when dealing with
    // numerous matrices, we may want to delay this loading (a `lazy` mode)
    return (JIT::enabled and not(options & optImmediate))
             ? internalLoadJITData(filename, minWidth, maxHeight, options)
             // Reading data from file
             : internalLoadCSVFile(filename, minWidth, maxHeight, options, buffer);
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::saveToCSVFile(const AnyString& filename,
                                          uint precision,
                                          bool print_dimensions,
                                          bool saveEvenIfAllZero) const
{
    PredicateIdentity predicate;
    return internalSaveCSVFile(filename, precision, print_dimensions, predicate, saveEvenIfAllZero);
}

template<class T, class ReadWriteT>
template<class PredicateT>
bool Matrix<T, ReadWriteT>::saveToCSVFile(const AnyString& filename,
                                          uint precision,
                                          bool print_dimensions,
                                          PredicateT& predicate,
                                          bool saveEvenIfAllZero) const
{
    return internalSaveCSVFile(filename, precision, print_dimensions, predicate, saveEvenIfAllZero);
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::pasteToColumn(uint x, const U* data)
{
    assert(x < width and "Invalid column index (bigger than `this->width`)");
    ColumnType& column = entry[x];

    // if the two types are strictly equal, we can perform some major
    // optimisations
    if (Yuni::Static::Type::StrictlyEqual<T, U>::Yes)
    {
#ifdef ANTARES_SWAP_SUPPORT
        column.copy(height, data);
#else
        (void)::memcpy(column, data, sizeof(T) * height);
#endif
    }
    else
    {
        // ...otherwise we have to copy each item by hand in any cases
        for (uint y = 0; y != height; ++y)
            column[y] = (T)data[y];
    }

    markAsModified();
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::pasteToColumn(uint x, const Antares::Memory::Array<U>& data)
{
    assert(x < width and "Invalid column index (bigger than `this->width`)");
    ColumnType& column = entry[x];

    // if the two types are strictly equal, we can perform some major
    // optimisations
    if (Yuni::Static::Type::StrictlyEqual<T, U>::Yes)
    {
#ifdef ANTARES_SWAP_SUPPORT
        column.copy(height, data);
#else
        (void)::memcpy(column, data, sizeof(T) * height);
#endif
    }
    else
    {
        // ...otherwise we have to copy each item by hand in any cases
        for (uint y = 0; y != height; ++y)
            column[y] = (T)data[y];
    }

    markAsModified();
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::fillColumn(uint x, const T& value)
{
    assert(x < width and "Invalid column index (bigger than `this->width`)");
    ColumnType& column = entry[x];
#ifdef ANTARES_SWAP_SUPPORT
    column.assign(height, value);
#else
    for (uint y = 0; y != height; ++y)
        column[y] = value;
#endif

    markAsModified();
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::columnToZero(uint x)
{
    assert(x < width and "Invalid column index (bigger than `this->width`)");
    ColumnType& column = entry[x];
#ifdef ANTARES_SWAP_SUPPORT
    column.assign(height);
#else
    (void)::memset((void*)column, 0, sizeof(T) * height);
#endif

    markAsModified();
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::markAsModified() const
{
    if (jit)
        jit->markAsModified();
}

template<class T, class ReadWriteT>
inline bool Matrix<T, ReadWriteT>::empty() const
{
    return (!width) or (!height);
}

template<class T, class ReadWriteT>
inline Yuni::uint64 Matrix<T, ReadWriteT>::memoryUsage() const
{
#ifdef ANTARES_SWAP_SUPPORT
    Yuni::uint64 r = sizeof(Matrix<T, ReadWriteT>) + ((jit) ? jit->memoryUsage() : 0);
    if (entry)
    {
        for (uint i = 0; i != width; ++i)
            r += sizeof(ColumnType) + (entry[i].needFlush() ? sizeof(T) * height : 0);
    }
    return r;
#else
    return sizeof(Matrix<T, ReadWriteT>) + (sizeof(T) * (width * height))
           + ((jit) ? jit->memoryUsage() : 0);
#endif
}

template<class T, class ReadWriteT>
inline Yuni::uint64 Matrix<T, ReadWriteT>::valuesMemoryUsage() const
{
    return (sizeof(T) * (width * height));
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::clear()
{
    if (entry)
    {
        for (uint i = 0; i != width; ++i)
            Antares::Memory::Release(entry[i]);
        delete[] entry;
        entry = nullptr;
    }
    width = 0;
    height = 0;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::reset()
{
    clear();
    markAsModified();
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::flush() const
{
#ifdef ANTARES_SWAP_SUPPORT
    if (entry)
    {
        for (uint i = 0; i != width; ++i)
            entry[i].flush();
    }
#endif
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::resize(uint w, uint h, bool fixedSize)
{
    // Asserts
    // This limit is correlated with the maximal amount of years
    // See the routine GeneralData::fixBadValues() if some changes are needed
    assert(w <= 50000 and "The new width seems a bit excessive");
    assert(h <= 50000 and "The new height seems a bit excessive");

    // Checking if the matrix really needs to be resized
    if (w != width or h != height)
    {
        if (!w or !h)
        {
            clear();
        }
        else
        {
            if (entry)
            {
                for (uint i = 0; i != width; ++i)
                    Antares::Memory::Release(entry[i]);
                delete[] entry;
            }
            if (!w and !h)
            {
                entry = nullptr;
                width = 0;
                height = 0;
            }
            else
            {
                // Assigning the new size
                width = w;
                height = h;

                // Allocating the entry for the matrix
                entry = new typename Antares::Memory::Stored<T>::Type[width + 1];
                entry[width] = nullptr;

                MatrixAutoFlush<MatrixType> autoflush(*this);
                for (uint i = 0; i != w; ++i)
                {
                    ++autoflush;
                    Antares::Memory::Allocate<T>(entry[i], height);
                }
            }
        }
    }

    // JIT Update
    if (JIT::enabled and not jit and fixedSize)
        jit = JIT::Reset(jit);

    if (jit and w != 0 and h != 0)
    {
        jit->minWidth = w;
        jit->maxHeight = h;
        if (fixedSize)
            jit->options = jit->options | optFixedSize;
    }
    markAsModified();
}

namespace // anonymous
{
static inline bool DetectEncoding(const AnyString& filename, const AnyString& data, size_t& offset)
{
    if (data.size() > 1)
    {
        // UTF-16 Big endian
        if ((unsigned char)data[0] == 0xFE and ((unsigned char)data[1]) == 0xFF)
        {
            if (data.size() > 3 and !data[2] and !data[3])
                logs.error() << '`' << filename
                             << "`: UTF-32 Little Endian encoding detected. ASCII/UTF-8 required.";
            else
                logs.error() << '`' << filename
                             << "`: UTF-16 Big Endian encoding detected. ASCII/UTF-8 required.";
            return false;
        }
        // UTF-16 Little endian
        if ((unsigned char)data[0] == 0xFF and ((unsigned char)data[1]) == 0xFE)
        {
            logs.error() << '`' << filename
                         << "`: UTF-16 Little Endian encoding detected. ASCII/UTF-8 required.";
            return false;
        }
        // UTF-8 Little endian
        if ((unsigned char)data[0] == 0xEF and ((unsigned char)data[1]) == 0xBB and data.size() > 2
            and ((unsigned char)data[2]) == 0xBF)
        {
            // Slipping the byte-order mark
            offset = 3;
        }
        if (data.size() > 3)
        {
            if (!data[0] and !data[1] and ((unsigned char)data[2]) == 0xFE
                and ((unsigned char)data[3]) == 0xFF)
            {
                logs.error() << '`' << filename
                             << "`: UTF-32 Big Endian encoding detected. ASCII/UTF-8 required.";
                return false;
            }
        }
    }
    return true;
}

} // anonymous namespace

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::loadFromBuffer(const AnyString& filename,
                                           BufferType& data,
                                           uint minWidth,
                                           uint maxHeight,
                                           const int fixedSize,
                                           uint options)
{
    using namespace Yuni;

#ifndef NDEBUG
    logs.debug() << "  :: loading `" << filename << "'";
#endif

    // Detecting BOM
    // Antares currently only accepts ASCII and/or UTF-8 encodings.
    size_t bom = 0;
    if (not DetectEncoding(filename, data, bom))
    {
        // gp : dead code - can never be reached
        reset((minWidth > 0 ? minWidth : 1), maxHeight);
        return false;
    }

    uint offset = (uint)bom;
    uint x = 0;

    // Properly resizing the matrix
    // Directly resizing the matrix when its size is well-known
    if (fixedSize)
    {
        reset(minWidth, maxHeight, true);
    }
    else
    {
        // Autodetection of the number of lines
        if (!maxHeight)
        {
            assert(not data.empty());
            maxHeight = data.countChar('\n');
            if (data.last() == '\n')
                --maxHeight;
            else
                ++maxHeight;
        }
        // gp : dead code - can never be reached
        // The first occurence of the carriage return
        YString::Size max = data.find('\n');
        if (max == BufferType::npos)
        {
            logs.error() << filename << ": At least one line-return must be available";
            reset(minWidth, maxHeight);
            return false;
        }
        offset = max + 1;
        // remove all final trailing whitespaces
        if (max > 0)
        {
            do
            {
                char c = data[max - 1];
                if (c == '\r' or c == '\t' or c == ' ')
                {
                    if (!(--max))
                        break;
                }
                else
                    break;
            } while (true);
        }

        if (max > 7 /* ex: size:0x0 */ and data.startsWith("size:"))
        {
            CString<64, false> buffer;
            buffer.assign(data.c_str(), max);
            int x, y;
#ifdef YUNI_OS_MSVC
            sscanf_s(buffer.c_str(), "size:%dx%d", &x, &y);
#else
            sscanf(buffer.c_str(), "size:%dx%d", &x, &y);
#endif
            if (x < 1)
            {
                if (!(options & optQuiet))
                    logs.warning() << '`' << filename << "`: Invalid header";
                x = 1;
            }
            if (y < 1)
            {
                if (!(options & optQuiet))
                    logs.warning() << '`' << filename << "`: Invalid header";
                y = maxHeight;
            }
            maxHeight = y;
            resize((uint)x, (uint)y);
        }
        else
        {
            offset = 0;
            x = ((max > 0) ? 1 : 0);

            if (max > 0)
            {
                while ((offset = (uint)data.find_first_of(ANTARES_MATRIX_CSV_COMMA, offset)) < max)
                {
                    ++offset;
                    ++x;
                }
            }
            resize(((x < minWidth) ? minWidth : x), maxHeight);
#ifndef NDEBUG
// logs.debug() << "  :: (" << width << 'x' << height << ')';
#endif
            if (!x)
            {
                if (not(options & optQuiet) and not(options & optNoWarnIfEmpty))
                    logs.warning() << "`" << filename << "`: Invalid format: The file seems empty";
                zero();
                return false;
            }

            offset = (uint)bom;
        }
    }

    uint y = 0;
    uint pos;
    int errorCount = 6;
    char separator = '\0';
    AnyString converter;
    ReadWriteType cellValue;
    bool result = true;

    while (y < maxHeight and offset < (uint)data.size())
    {
        // Starting the reading of the begining of the line
        x = 0;
        pos = offset;
        uint lineOffset = (uint)offset;

        // autoflush for loading huge matrices
        MatrixAutoFlush<MatrixType> autoflush(*this);

        while ((offset = data.find_first_of(ANTARES_MATRIX_CSV_SEPARATORS, offset))
               != BufferType::npos)
        {
            assert(offset != BufferType::npos);

            ++autoflush;

            separator = data[offset];
            // the final zero is mandatory for string-to-double convertions
            data[offset] = '\0';
            // Adding the value
            converter = (const char*)((const char*)data.c_str() + pos);

            // Convert string into double or something else
            if (not converter.empty())
            {
                if (x >= width) // Out of bounds
                {
                    if ((options & optNeverFails))
                    {
                        if (separator == '\n')
                        {
                            resizeWithoutDataLost(x + 1, height);
                        }
                        else
                        {
                            // gp : dead code - can never be reached
                            // Looking for the new matrix width
                            uint newOffset = offset;
                            uint newWidth = width + 1;

                            while ((newOffset = data.find_first_of(ANTARES_MATRIX_CSV_SEPARATORS,
                                                                   (String::Size)newOffset))
                                   != BufferType::npos)
                            {
                                if (data[newOffset] == '\n')
                                {
                                    ++newWidth;
                                    break;
                                }
                                if (data[newOffset] != '\r')
                                    ++newWidth;
                                ++newOffset;
                            }
                            resizeWithoutDataLost(newWidth, height);
                        }
                        // logs.debug() << "  :: dynamic resize (" << width << 'x' << height << ')';
                    }
                    else
                    {
                        result = false;
                        if (not(options & optQuiet) and errorCount > 0)
                        {
                            logs.warning()
                              << '`' << filename << "`: Invalid format: Too many entry for the row "
                              << y << " (offset: " << (uint)pos << "byte)";
                            if (!(--errorCount))
                                logs.warning() << " ... (skipped)";
                        }
                        break;
                    }
                }

                if (MatrixStringConverter<ReadWriteType>::direct)
                {
                    // We can perform a direct copy, instead of using a temporary buffer
                    // for complex conversions
                    // This should considerably reduced the loading time
                    MatrixData<T>::Copy(entry[x][y], converter);
                }
                else
                {
                    // We will try to convert the string value into the corresponding
                    // type.
                    if (not MatrixStringConverter<ReadWriteType>::Do(converter, cellValue))
                    {
                        // We may fail in some special cases, like when using old studies
                        // where decimal values can be found instead of integer
                        double fallback;
                        if (not MatrixStringConverter<double>::Do(converter, fallback))
                        {
                            result = false;
                            if (not(options & optQuiet) and errorCount)
                            {
                                logs.warning()
                                  << '`' << filename << "`: Invalid numeric value (x:" << x
                                  << ",y:" << y << ", offset: " << (uint)pos << "byte), text: `"
                                  << converter << " read:" << entry[x][y] << '`';
                                if (not(--errorCount))
                                    logs.warning() << " ... (skipped)";
                            }
                            MatrixData<T>::Init(entry[x][y]);
                        }
                        else
                            entry[x][y] = MatrixRound<T, ReadWriteType>::Value(
                              static_cast<ReadWriteType>(fallback));
                    }
                    else
                    {
                        // Ok, the conversion succeeded
                        MatrixData<T>::Copy(entry[x][y], cellValue);
                    }
                }
            }
            else
            {
                // We may encounter final tabs, which must not be managed as an error
                if (x < width)
                {
                    MatrixData<T>::Init(entry[x][y]);
                    if (not(options & optQuiet))
                    {
                        logs.debug() << "  empty value at " << (x + 1) << 'x' << (y + 1)
                                     << " (line offset: " << (offset - lineOffset) << ")";
                    }
                }
            }

            // Offset for the next token
            pos = ++offset;

            // new column
            ++x;

            // next column
            if (separator == '\r')
            {
                if (data[offset] == '\n')
                {
                    pos = ++offset;
                    break;
                }
            }
            else
            {
                if (separator == '\n')
                    break;
            }
        }

        // Not enough columns to describe the row of the matrix
        if (x < width)
        {
            if (not(options & optNeverFails))
            {
                result = false;
                if (not(options & optQuiet) and errorCount)
                {
                    logs.warning()
                      << filename << ": at line " << (y + 1) << ", not enough columns (expected "
                      << width << ", got " << x << ')';
                    if (not(--errorCount))
                        logs.warning() << " ... (skipped)";
                }
            }
            while (x < width) // Init for missing entry
            {
                MatrixData<T>::Init(entry[x][y]);
                ++x;
            }
#ifdef ANTARES_SWAP_SUPPORT
            if (width > 100)
                flush();
#endif
        }

        // Go to the next line
        ++y;

    } // while (y ...)

    // Not enough lines to describe our matrix
    if (y < height)
    {
        result = false;
        if (!(options & optQuiet))
        {
            logs.warning() << filename << ": not enough rows (expected " << height << ", got " << y
                           << ')';
        }
        // Initialize missing entry
        while (y < height)
        {
            for (x = 0; x < width; ++x)
                MatrixData<T>::Init(entry[x][y]);
            ++y;
        }
    }

    return ((0 != (options & optNeverFails)) ? true : result);
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::internalLoadCSVFile(const AnyString& filename,
                                                uint minWidth,
                                                uint maxHeight,
                                                uint options,
                                                BufferType* buffer)
{
    using namespace Yuni;

    // Status
    bool result = false;

    const bool hasOwnership = (NULL == buffer);
    if (not buffer)
        buffer = new BufferType();

    switch (loadFromFileToBuffer(*buffer, filename))
    {
    case IO::errNone:
    {
        // Empty files
        if (buffer->empty())
        {
            if (maxHeight and minWidth)
                reset((minWidth != 0 ? minWidth : 1),
                      maxHeight); // gp : minWidth always != 0 here ==> Refactoring
            else
                clear();
            result = true;
            break;
        }

        // IO statistics
        Statistics::HasReadFromDisk(buffer->size());

        // Adding a final \n to make sure we have a line return at the end of the file
        *buffer += '\n';
        // Load the data
        result = loadFromBuffer(
          filename, *buffer, minWidth, maxHeight, (options & optFixedSize), options);

        // Mark as modified
        if (0 != (options & optMarkAsModified))
        {
            if (jit)
                jit->markAsModified();
        }
        break;
    }
    case IO::errNotFound:
    {
        if (not(options & optQuiet))
            logs.error() << "I/O Error: not found: '" << filename << "'";
        break;
    }
    case IO::errMemoryLimit:
    {
        if (not(options & optQuiet))
        {
            logs.error() << filename << ": The file is too large (>"
                         << (filesizeHardLimit / 1024 / 1024) << "Mo)";
        }
        break;
    }
    default:
    {
        if (not(options & optQuiet))
            logs.error() << "I/O Error: failed to load '" << filename << "'";
    }
    }

    if (hasOwnership)
        delete buffer;

    // The matrix may not be loaded but we have to initialize it to avoid
    // further SegV if the application decides to continue anyway.
    if (not result)
        reset(minWidth, maxHeight);

    // Post-processing when the Load-on-Demand is enabled
    if (JIT::enabled and not jit and (0 != (options & optFixedSize)))
    {
        jit = JIT::Reset(jit, filename);
    }

    if (jit)
    {
        jit->alreadyLoaded = true;
        jit->modified = false;
        jit->minWidth = (options & optFixedSize) ? (!width ? minWidth : width) : 1;
        jit->maxHeight = height;
        jit->options = options;
        if (jit->sourceFilename.empty())
        {
            jit->sourceFilename = filename;
            assert(not jit->sourceFilename.empty());
        }
    }

    // When swap support is enabled, releasing some memory
    flush();

    // We return `true` in any cases to not stop the execution of the solver, since
    // it may not be a fatal error
    return result;
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::containsOnlyZero() const
{
    if (width and height)
    {
        MatrixAutoFlush<MatrixType> autoflush(*this);
        for (uint x = 0; x != width; ++x)
        {
            ++autoflush;
            auto& column = entry[x];
            for (uint y = 0; y != height; ++y)
            {
                if (not Yuni::Math::Zero((T)column[y]))
                    return false;
            }
        }
    }
    return true;
}

template<class T, class ReadWriteT>
template<class PredicateT>
bool Matrix<T, ReadWriteT>::containsOnlyZero(PredicateT& predicate) const
{
    if (width and height)
    {
        MatrixAutoFlush<MatrixType> autoflush(*this);
        for (uint x = 0; x != width; ++x)
        {
            ++autoflush;
            /* auto& column = entry[x]; */
            for (uint y = 0; y != height; ++y)
            {
                if (not Yuni::Math::Zero((T)predicate((*this)[x][y])))
                    return false;
            }
        }
    }
    return true;
}

template<class T, class ReadWriteT>
template<class PredicateT>
void Matrix<T, ReadWriteT>::saveToBuffer(std::string& data,
                                         uint precision,
                                         bool print_dimensions,
                                         PredicateT& predicate,
                                         bool saveEvenIfAllZero) const
{
    using namespace Yuni;
    enum
    {
        // Get if the read/write type is a decimal type (e.g. double/float/long double)
        isDecimal = Static::Type::IsDecimal<ReadWriteType>::Yes,
    };

    if (not print_dimensions and (containsOnlyZero(predicate) and not saveEvenIfAllZero))
        // Does nothing if the matrix only contains zero
        return;

    matrix_to_buffer_dumper_factory mtx_to_buffer_dumper_factory(isDecimal, precision);

    I_mtx_to_buffer_dumper<T, ReadWriteT, PredicateT>* mtx_to_buffer_dpr
      = mtx_to_buffer_dumper_factory.get_dumper<T, ReadWriteT, PredicateT>(this, data, predicate);

    // Determining the string format to use according the given precision
    mtx_to_buffer_dpr->set_print_format(isDecimal, precision);

    // Pre-allocate memory in the buffer. It should be enough in nearly all cases.
    data.reserve(width * height * 6);

    if (print_dimensions)
        data += "size:" + std::to_string(width) + 'x' + std::to_string(height) + '\n';

    mtx_to_buffer_dpr->run();
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::openFile(Yuni::IO::File::Stream& file, const AnyString& filename) const
{
    if (not file.openRW(filename))
    {
        logs.error() << "I/O error: " << filename
                     << ": Impossible to write the file (not enough permission ?)";
        return false;
    }
    return true;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::saveBufferToFile(std::string& buffer, Yuni::IO::File::Stream& f) const
{
    f << buffer;
}

template<class T, class ReadWriteT>
template<class PredicateT>
bool Matrix<T, ReadWriteT>::internalSaveCSVFile(const AnyString& filename,
                                                uint precision,
                                                bool print_dimensions,
                                                PredicateT& predicate,
                                                bool saveEvenIfAllZero) const
{
    JIT::just_in_time_manager jit_mgr(jit, filename);

    jit_mgr.record_current_jit_state(width, height);

    if (jit_mgr.jit_activated() && jit_mgr.matrix_content_in_memory_is_same_as_on_disk())
    {
        // No difference between actual matrix content in memory and matrix on disk, so we don't
        // need to save on disk. Besides, as jit is on, we do not need it in memory, and matrix is
        // cleared.
        jit_mgr.clear_matrix(this);
        return true;
    }

    if (jit_mgr.jit_activated() && jit_mgr.do_we_force_matrix_load_from_disk())
        jit_mgr.load_matrix(this);

#ifndef NDEBUG
    // Attempt to open the file, and to write data
    // We have write access to the file
    logs.debug() << "  :: writing `" << filename << "' (" << width << 'x' << height << ')';
#endif

    Yuni::IO::File::Stream file;
    if (not openFile(file, filename))
        return false;

    if (height and width)
    {
        std::string buffer;

        saveToBuffer(buffer, precision, print_dimensions, predicate, saveEvenIfAllZero);
        Statistics::HasWrittenToDisk(buffer.size());

        saveBufferToFile(buffer, file);
    }

    // When the swap support is enabled, releasing some memory
    flush();

#ifndef NDEBUG
    // Attempt to open the file, and to write data
    // We have write access to the file
    logs.debug() << "  :: [end] writing `" << filename << "' (" << width << 'x' << height << ')';
#endif

    jit_mgr.unload_matrix_properly_from_memory(this);

    return true;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::resizeWithoutDataLost(uint x, uint y, const T& defVal)
{
    if (!x or !y)
    {
        clear();
    }
    else
    {
        if (x <= width and y <= height) // shrinking
        {
            for (uint i = x; i < width; ++i)
                Antares::Memory::Release(entry[i]);

            // Update the matrix size
            width = x;
            height = y;
        }
        else
        {
            const Matrix<T, ReadWriteT> copy(*this);
            resize(x, y);
            // Copy values
            uint minW = (x < copy.width) ? x : copy.width;
            uint minH = (y < copy.height) ? y : copy.height;

            MatrixAutoFlush<MatrixType> autoflush(*this);
            MatrixAutoFlush<MatrixType> autoflushCopy(copy);

            for (uint i = 0; i < minW; ++i)
            {
                ++autoflush;
                ++autoflushCopy;

                ColumnType& column = entry[i];
#ifdef ANTARES_SWAP_SUPPORT
                column.copy(minH, copy.entry[i]);
#else
                (void)::memcpy(column, copy.entry[i], sizeof(T) * minH);
#endif
                for (uint j = minH; j < y; ++j)
                    column[j] = defVal;

#ifdef ANTARES_SWAP_SUPPORT
                column.flush();
#endif
            }

            if (defVal == T())
            {
                for (uint i = minW; i < x; ++i)
                {
                    ++autoflush;
                    Memory::Zero(y, entry[i]);
                }
            }
            else
            {
                for (uint i = minW; i < x; ++i)
                {
                    ++autoflush;
                    Memory::Assign(y, entry[i], defVal);
                }
            }
        }
    }
    markAsModified();
    logs.debug() << "  :: end resizeWithoutDataLost";
}

template<class T, class ReadWriteT>
bool Matrix<T, ReadWriteT>::loadAllJITData() const
{
    if (jit and not JIT::IsReady(jit))
    {
        return (const_cast<Matrix<T, ReadWriteT>*>(this))
          ->loadFromCSVFile(
            jit->sourceFilename, jit->minWidth, jit->maxHeight, jit->options | optImmediate);
    }
    return true;
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::multiplyAllEntriesBy(const U& c)
{
    if (entry)
    {
        if (!Yuni::Math::Zero(c))
        {
            MatrixAutoFlush<MatrixType> autoflush(*this);
            for (uint x = 0; x != width; ++x)
            {
                ++autoflush;
                ColumnType& column = entry[x];
#ifdef ANTARES_SWAP_SUPPORT
                column.multiply(height, c);
#else
                for (uint y = 0; y != height; ++y)
                    column[y] *= (T)c;
#endif
            }
        }
        else
            zero();
    }
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::multiplyColumnBy(uint x, const U& c)
{
    assert(x < width and "Invalid column index (bigger than `this->width`)");
    ColumnType& column = entry[x];
    for (uint y = 0; y != height; ++y)
        column[y] *= (T)c;
}

template<class T, class ReadWriteT>
template<class U>
void Matrix<T, ReadWriteT>::divideColumnBy(uint x, const U& c)
{
    assert(x < width and "Invalid column index (bigger than `this->width`)");
    assert(c != (T)0 && "Dividing by zero");
    ColumnType& column = entry[x];
    for (uint y = 0; y != height; ++y)
        column[y] /= (T)c;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::roundAllEntries()
{
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint x = 0; x != width; ++x)
    {
        ++autoflush;
        ColumnType& col = entry[x];
        for (uint y = 0; y != height; ++y)
            col[y] = (T)Yuni::Math::Round(col[y]);
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::makeAllEntriesAbsolute()
{
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint x = 0; x != width; ++x)
    {
        ++autoflush;
        ColumnType& col = entry[x];
        for (uint y = 0; y != height; ++y)
            col[y] = Yuni::Math::Abs<T>(col[y]);
    }
}

template<class T, class ReadWriteT>
T Matrix<T, ReadWriteT>::findLowerBound() const
{
    double r = +1e30;
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint x = 0; x != width; ++x)
    {
        ++autoflush;
        ColumnType& col = entry[x];
        for (uint y = 0; y != height; ++y)
        {
            if (col[y] < r)
                r = col[y];
        }
    }
    return (T)r;
}

template<class T, class ReadWriteT>
T Matrix<T, ReadWriteT>::findUpperBound() const
{
    double r = -1e30;
    MatrixAutoFlush<MatrixType> autoflush(*this);
    for (uint x = 0; x != width; ++x)
    {
        ++autoflush;
        ColumnType& col = entry[x];
        for (uint y = 0; y != height; ++y)
        {
            if (col[y] > r)
                r = col[y];
        }
    }
    return (T)r;
}

template<class T, class ReadWriteT>
template<class U, class V>
void Matrix<T, ReadWriteT>::copyFrom(const Matrix<U, V>& rhs)
{
    assert((void*)(&rhs) != (void*)this and "Undefined behavior");

    rhs.invalidate(true);
    if (rhs.empty())
    {
        clear();
    }
    else
    {
        // resize the matrix
        resize(rhs.width, rhs.height);
        // copy raw values
        MatrixAutoFlush<MatrixType> autoflush(*this);
        for (uint x = 0; x != rhs.width; ++x)
        {
            ++autoflush;
            auto& column = entry[x];
            const auto& src = rhs.entry[x];

            // if the two types are strictly equal, we can perform some major
            // optimisations
            if (Yuni::Static::Type::StrictlyEqual<T, U>::Yes)
            {
#ifdef ANTARES_SWAP_SUPPORT
                column.copy(height, src);
#else
                (void)::memcpy((void*)column, (void*)src, sizeof(T) * height);
#endif
            }
            else
            {
                // ...otherwise we have to copy each item by hand in any cases
                for (uint y = 0; y != height; ++y)
                    column[y] = (T)src[y];
            }
        }

        // swap files
        flush();
        rhs.flush();
    }

    if (rhs.jit)
    {
        if (not jit)
        {
            jit = new JIT::Informations(*rhs.jit);
        }
        else
        {
            jit->options = rhs.jit->options;
            jit->minWidth = rhs.jit->minWidth;
            jit->maxHeight = rhs.jit->maxHeight;
        }
    }
    // mark the matrix as modified
    markAsModified();
}

template<class T, class ReadWriteT>
template<class U, class V>
inline void Matrix<T, ReadWriteT>::copyFrom(const Matrix<U, V>* rhs)
{
    if (rhs)
        copyFrom(*rhs);
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::swap(Matrix<T, ReadWriteT>& rhs) noexcept
{
    // argument deduction lookup (ADL)
    using std::swap;
    swap(this->width, rhs.width);
    swap(this->height, rhs.height);
    swap(this->entry, rhs.entry);
    swap(this->jit, rhs.jit);
}

template<class T, class ReadWriteT>
inline Matrix<T, ReadWriteT>& Matrix<T, ReadWriteT>::operator=(const Matrix<T, ReadWriteT>& rhs)
{
    copyFrom(rhs);
    return *this;
}

template<class T, class ReadWriteT>
inline Matrix<T, ReadWriteT>& Matrix<T, ReadWriteT>::operator=(Matrix<T, ReadWriteT>&& rhs) noexcept
{
    width = rhs.width;
    height = rhs.height;
    jit = rhs.jit;
    if (0 == width || 0 == height)
    {
        entry = nullptr;
        width = 0;
        height = 0;
    }
    else
    {
        entry = rhs.entry;
    }
    // Prevent spurious de-allocation from rhs's destructor
    rhs.entry = nullptr;
    return *this;
}

template<class T, class ReadWriteT>
template<class U>
inline Matrix<T, ReadWriteT>& Matrix<T, ReadWriteT>::operator=(const Matrix<U>& rhs)
{
    copyFrom(rhs);
    return *this;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::print() const
{
    std::cout << "DUMP:\n";
    if (empty())
    {
        std::cout << "\tempty\n";
        return;
    }
    for (uint y = 0; y != height; ++y)
    {
        std::cout << "\t[";
        for (uint x = 0; x != width; ++x)
        {
            if (x)
                std::cout << ",\t";
            else
                std::cout << '\t';
            std::cout << entry[x][y];
        }
        std::cout << "]\n";
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::unloadFromMemory() const
{
    if (jit)
    {
        if (jit->alreadyLoaded and not jit->modified and not jit->sourceFilename.empty())
        {
            // ugly, but to not break the whole code design
            auto& thisNotConst = const_cast<Matrix&>(*this);

            thisNotConst.clear();
            JIT::MarkAsNotLoaded(thisNotConst.jit);
        }
    }
}

template<class T, class ReadWriteT>
inline void Matrix<T, ReadWriteT>::EstimateMemoryUsage(Antares::Data::StudyMemoryUsage& u,
                                                       uint w,
                                                       uint h)
{
    MatrixEstimateMemoryUsage(sizeof(T), u, w, h);
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::estimateMemoryUsage(Antares::Data::StudyMemoryUsage& u,
                                                bool cond,
                                                uint w,
                                                uint h) const
{
    if (cond)
        EstimateMemoryUsage(u, w, h);
    else
        estimateMemoryUsage(u);
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::estimateMemoryUsage(Antares::Data::StudyMemoryUsage& u) const
{
    using namespace Yuni;

    // We already have the data. No need to estimate the value.
    if (JIT::IsReady(jit))
    {
        EstimateMemoryUsage(u, width, height);
        return;
    }

    // The data are actually not loaded. We will have to rely on the JIT structure
    MatrixEstimateMemoryUsageFromJIT(sizeof(T), u, jit);
}

template<class T1, class T2>
bool MatrixTestForAtLeastOnePositiveValue(const Matrix<T1, T2>& m)
{
    if (m.width and m.height)
    {
        uint y;
        for (uint x = 0; x < m.width; ++x)
        {
            auto& col = m.entry[x];
            for (y = 0; y < m.height; ++y)
            {
                if (col[y] > T1(0))
                    return true;
            }
        }
    }
    return false;
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::circularShiftRows(uint count)
{
    if (count != 0)
    {
        for (uint column = 0; column != width; ++column)
        {
            circularShiftRows(column, count);
#ifdef ANTARES_SWAP_SUPPORT
            entry[column].flush();
#endif
        }
        markAsModified();
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::reverseRows(uint column, uint start, uint end)
{
    if (height <= 1 or !(column < width) or !(start < end))
        return;

    // The values of the selected column
    auto& values = entry[column];
    // temporary value
    T swap;
    for (uint y = start; y < --end; ++y)
    {
        swap = values[y];
        values[y] = values[end];
        values[end] = swap;
    }
}

template<class T, class ReadWriteT>
void Matrix<T, ReadWriteT>::circularShiftRows(uint column, uint count)
{
    assert(column < width and "Column out of bounds");
    if (height <= 1 or !(column < width) or !count)
        return;

    // fits \p count into [0..height[
    count = (count % height + height) % height;
    if (count == 0 or (uint) count == height)
        return;

    // Algorithm in O(N)
    reverseRows(column, 0, count);
    reverseRows(column, count, height);
    reverseRows(column, 0, height);
    markAsModified();
}

template<class T, class ReadWriteT>
inline const typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::operator[](
  uint column) const
{
    assert(column < width);
    assert(Memory::RawPointer(entry[column]));
    return entry[column];
}

template<class T, class ReadWriteT>
inline typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::operator[](uint column)
{
    assert(column < width);
    assert(Memory::RawPointer(entry[column]));
    return entry[column];
}

template<class T, class ReadWriteT>
inline const typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::column(uint n) const
{
    assert(n < width);
    assert(Memory::RawPointer(entry[n]));
    return entry[n];
}

template<class T, class ReadWriteT>
inline typename Matrix<T, ReadWriteT>::ColumnType& Matrix<T, ReadWriteT>::column(uint n)
{
    assert(n < width);
    assert(Memory::RawPointer(entry[n]));
    return entry[n];
}

} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_MATRIX_HXX__
