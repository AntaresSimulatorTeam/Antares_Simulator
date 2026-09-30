// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_HXX
#define ANTARES_ARRAY_MATRIX_IO_HXX

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <yuni/core/static/types.h>
#include <yuni/io/file.h>

#include <antares/io/statistics.h>
#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

#include "matrix-to-buffer.h"

namespace Antares::MatrixIO
{
using BufferType = Yuni::Clob;
using FileLoader = std::function<Yuni::IO::Error(BufferType&, const AnyString&)>;

template<class T, class ReadWriteT = T>
using MatrixType = Matrix<T, ReadWriteT>;

namespace // anonymous
{
constexpr uint64_t matrixFilesizeHardLimit = 1536ULL * 1024ULL * 1024ULL;

template<class T>
class MatrixData final
{
public:
    static void Init(T& data)
    {
        data = T();
    }

    template<class U>
    static void Copy(T& data, const U& value)
    {
        data = static_cast<T>(value);
    }

    static void Copy(T&, const AnyString&)
    {
        // This overload prevents an accidental numeric cast on the direct path.
        logs.error() << "internal error: matrix data conversion";
    }
};

template<uint ChunkSizeT, bool ExpandableT>
class MatrixData<Yuni::CString<ChunkSizeT, ExpandableT>> final
{
public:
    using StringType = Yuni::CString<ChunkSizeT, ExpandableT>;

    static void Init(StringType& data)
    {
        data.clear();
    }

    template<class U>
    static void Copy(StringType& data, const U& value)
    {
        data = value;
    }
};

template<class ReadWriteT>
class MatrixStringConverter final
{
public:
    enum
    {
        direct = 0
    };

    static bool Do(const AnyString& str, ReadWriteT& out)
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

    static bool Do(const AnyString& str, double& out)
    {
        char* end = nullptr;
        out = ::strtod(str.c_str(), &end);
        return end != nullptr && *end == '\0';
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

    static bool Do(const AnyString& str, float& out)
    {
        char* end = nullptr;
        out = static_cast<float>(::strtod(str.c_str(), &end));
        return end != nullptr && *end == '\0';
    }
};

template<uint ChunkSizeT, bool ExpandableT>
class MatrixStringConverter<Yuni::CString<ChunkSizeT, ExpandableT>> final
{
public:
    enum
    {
        direct = 1
    };

    using StringType = Yuni::CString<ChunkSizeT, ExpandableT>;

    static bool Do(const AnyString& str, StringType& out)
    {
        out.assign(str);
        return true;
    }
};

template<unsigned A, bool B>
Yuni::CString<A, B> trunc(Yuni::CString<A, B>& str)
{
    return str;
}

template<class T>
static T trunc(T& in)
{
    return static_cast<T>(std::trunc(in));
}

template<class T, class P>
class MatrixRound final
{
public:
    static T Value(P value)
    {
        return static_cast<T>(trunc(value));
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

bool detectEncoding(const AnyString& filename, const std::string& data, size_t& offset)
{
    if (data.size() > 1)
    {
        if (static_cast<unsigned char>(data[0]) == 0xFE
            && static_cast<unsigned char>(data[1]) == 0xFF)
        {
            if (data.size() > 3 && data[2] == 0 && data[3] == 0)
            {
                logs.error() << '`' << filename
                             << "`: UTF-32 Little Endian encoding detected. ASCII/UTF-8 required.";
            }
            else
            {
                logs.error() << '`' << filename
                             << "`: UTF-16 Big Endian encoding detected. ASCII/UTF-8 required.";
            }
            return false;
        }

        if (static_cast<unsigned char>(data[0]) == 0xFF
            && static_cast<unsigned char>(data[1]) == 0xFE)
        {
            logs.error() << '`' << filename
                         << "`: UTF-16 Little Endian encoding detected. ASCII/UTF-8 required.";
            return false;
        }

        if (data.size() > 2 && static_cast<unsigned char>(data[0]) == 0xEF
            && static_cast<unsigned char>(data[1]) == 0xBB
            && static_cast<unsigned char>(data[2]) == 0xBF)
        {
            offset = 3;
        }

        if (data.size() > 3 && data[0] == 0 && data[1] == 0
            && static_cast<unsigned char>(data[2]) == 0xFE
            && static_cast<unsigned char>(data[3]) == 0xFF)
        {
            logs.error() << '`' << filename
                         << "`: UTF-32 Big Endian encoding detected. ASCII/UTF-8 required.";
            return false;
        }
    }
    return true;
}

template<class T, class ReadWriteT>
bool loadFromBuffer(Matrix<T, ReadWriteT>& matrix,
                    const AnyString& filename,
                    std::string data,
                    uint minWidth,
                    uint maxHeight,
                    bool fixedSize,
                    uint options)
{
    logs.debug() << "  :: loading `" << filename << "`";

    size_t bom = 0;
    if (!detectEncoding(filename, data, bom))
    {
        matrix.reset(minWidth > 0 ? minWidth : 1, maxHeight);
        return false;
    }

    uint offset = static_cast<uint>(bom);
    uint x = 0;

    if (fixedSize)
    {
        matrix.reset(minWidth, maxHeight);
    }
    else
    {
        if (!maxHeight)
        {
            maxHeight = static_cast<uint>(std::count(data.begin(), data.end(), '\n'));
            if (data.back() == '\n')
            {
                --maxHeight;
            }
            else
            {
                ++maxHeight;
            }
        }

        size_t max = data.find('\n');
        if (max == std::string::npos)
        {
            logs.error() << filename << ": At least one line-return must be available";
            matrix.reset(minWidth, maxHeight);
            return false;
        }

        offset = static_cast<uint>(max + 1);
        if (max > 0)
        {
            do
            {
                const char c = data[max - 1];
                if (c == '\r' || c == '\t' || c == ' ')
                {
                    if (!(--max))
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            } while (true);
        }

        if (max > 7 && data.compare(0, 5, "size:") == 0)
        {
            std::string header = data.substr(0, max);
            int headerWidth = 0;
            int headerHeight = 0;
#ifdef YUNI_OS_MSVC
            const int parsed = sscanf_s(header.c_str(), "size:%dx%d", &headerWidth, &headerHeight);
#else
            const int parsed = sscanf(header.c_str(), "size:%dx%d", &headerWidth, &headerHeight);
#endif
            if (parsed != 2)
            {
                headerWidth = 0;
                headerHeight = 0;
            }

            if (headerWidth < 1)
            {
                if (!(options & Matrix<T, ReadWriteT>::optQuiet))
                {
                    logs.warning() << '`' << filename << "`: Invalid header";
                }
                headerWidth = 1;
            }
            if (headerHeight < 1)
            {
                if (!(options & Matrix<T, ReadWriteT>::optQuiet))
                {
                    logs.warning() << '`' << filename << "`: Invalid header";
                }
                headerHeight = static_cast<int>(maxHeight);
            }
            maxHeight = static_cast<uint>(headerHeight);
            matrix.resize(static_cast<uint>(headerWidth), static_cast<uint>(headerHeight));
        }
        else
        {
            offset = 0;
            x = max > 0 ? 1 : 0;

            if (max > 0)
            {
                while ((offset = static_cast<uint>(data.find_first_of("\t;,", offset))) < max)
                {
                    ++offset;
                    ++x;
                }
            }

            matrix.resize(x < minWidth ? minWidth : x, maxHeight);
            if (!x)
            {
                if (!(options & Matrix<T, ReadWriteT>::optQuiet)
                    && !(options & Matrix<T, ReadWriteT>::optNoWarnIfEmpty))
                {
                    logs.warning() << '`' << filename << "`: Invalid format: The file seems empty";
                }
                matrix.zero();
                return false;
            }

            offset = static_cast<uint>(bom);
        }
    }

    uint y = 0;
    uint pos = 0;
    int errorCount = 6;
    char separator = '\0';
    AnyString converter;
    ReadWriteT cellValue{};
    bool result = true;

    while (y < maxHeight && offset < data.size())
    {
        x = 0;
        pos = offset;
        const uint lineOffset = offset;

        while ((offset = static_cast<uint>(data.find_first_of("\t\r\n;,", offset)))
               != static_cast<uint>(std::string::npos))
        {
            separator = data[offset];
            data[offset] = '\0';
            converter = data.c_str() + pos;

            if (!converter.empty())
            {
                if (x >= matrix.width())
                {
                    if (options & Matrix<T, ReadWriteT>::optNeverFails)
                    {
                        if (separator == '\n')
                        {
                            matrix.resizeWithoutDataLost(x + 1, matrix.height());
                        }
                        else
                        {
                            uint newOffset = offset;
                            uint newWidth = matrix.width() + 1;
                            while ((newOffset = static_cast<uint>(
                                      data.find_first_of("\t\r\n;,", newOffset)))
                                   != static_cast<uint>(std::string::npos))
                            {
                                if (data[newOffset] == '\n')
                                {
                                    ++newWidth;
                                    break;
                                }
                                if (data[newOffset] != '\r')
                                {
                                    ++newWidth;
                                }
                                ++newOffset;
                            }
                            matrix.resizeWithoutDataLost(newWidth, matrix.height());
                        }
                    }
                    else
                    {
                        result = false;
                        if (!(options & Matrix<T, ReadWriteT>::optQuiet) && errorCount > 0)
                        {
                            logs.warning() << '`' << filename
                                           << "`: Invalid format: Too many columns_ for the row "
                                           << y << " (offset: " << pos << "byte)";
                            if (!(--errorCount))
                            {
                                logs.warning() << " ... (skipped)";
                            }
                        }
                        break;
                    }
                }

                if (MatrixStringConverter<ReadWriteT>::direct)
                {
                    MatrixData<T>::Copy(matrix[x][y], converter);
                }
                else if (!MatrixStringConverter<ReadWriteT>::Do(converter, cellValue))
                {
                    double fallback = 0;
                    if (!MatrixStringConverter<double>::Do(converter, fallback))
                    {
                        result = false;
                        if (!(options & Matrix<T, ReadWriteT>::optQuiet) && errorCount)
                        {
                            logs.warning() << '`' << filename << "`: Invalid numeric value (x:" << x
                                           << ",y:" << y << ", offset: " << pos << "byte), text: `"
                                           << converter << " read:" << matrix[x][y] << '`';
                            if (!(--errorCount))
                            {
                                logs.warning() << " ... (skipped)";
                            }
                        }
                        MatrixData<T>::Init(matrix[x][y]);
                    }
                    else
                    {
                        matrix[x][y] = MatrixRound<T, ReadWriteT>::Value(
                          static_cast<ReadWriteT>(fallback));
                    }
                }
                else
                {
                    MatrixData<T>::Copy(matrix[x][y], cellValue);
                }
            }
            else if (x < matrix.width())
            {
                MatrixData<T>::Init(matrix[x][y]);
                if (!(options & Matrix<T, ReadWriteT>::optQuiet))
                {
                    logs.debug() << "  empty value at " << (x + 1) << 'x' << (y + 1)
                                 << " (line offset: " << (offset - lineOffset) << ")";
                }
            }

            pos = ++offset;
            ++x;

            if (separator == '\r')
            {
                if (offset < data.size() && data[offset] == '\n')
                {
                    pos = ++offset;
                    break;
                }
            }
            else if (separator == '\n')
            {
                break;
            }
        }

        if (x < matrix.width())
        {
            if (!(options & Matrix<T, ReadWriteT>::optNeverFails))
            {
                result = false;
                if (!(options & Matrix<T, ReadWriteT>::optQuiet) && errorCount)
                {
                    logs.warning()
                      << filename << ": at line " << (y + 1) << ", not enough columns (expected "
                      << matrix.width() << ", got " << x << ')';
                    if (!(--errorCount))
                    {
                        logs.warning() << " ... (skipped)";
                    }
                }
            }
            while (x < matrix.width())
            {
                MatrixData<T>::Init(matrix[x][y]);
                ++x;
            }
        }

        ++y;
    }

    if (y < matrix.height())
    {
        result = false;
        if (!(options & Matrix<T, ReadWriteT>::optQuiet))
        {
            logs.warning() << filename << ": not enough rows (expected " << matrix.height()
                           << ", got " << y << ')';
        }
        while (y < matrix.height())
        {
            for (x = 0; x < matrix.width(); ++x)
            {
                MatrixData<T>::Init(matrix[x][y]);
            }
            ++y;
        }
    }

    return (options & Matrix<T, ReadWriteT>::optNeverFails) ? true : result;
}

template<class T, class ReadWriteT, class Predicate>
bool containsOnlyZero(const Matrix<T, ReadWriteT>& matrix, Predicate& predicate)
{
    for (uint x = 0; x < matrix.width(); ++x)
    {
        for (uint y = 0; y < matrix.height(); ++y)
        {
            if (!Utils::isZero(static_cast<T>(predicate(matrix[x][y]))))
            {
                return false;
            }
        }
    }
    return true;
}
} // anonymous namespace

template<class T, class ReadWriteT>
bool load(MatrixType<T, ReadWriteT>& matrix,
          const AnyString& filename,
          uint minWidth = 1,
          uint maxHeight = 0,
          uint options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    assert(!filename.empty());

    BufferType owned;
    BufferType* input = buffer ? buffer : &owned;
    const bool readFromDisk = buffer == nullptr || static_cast<bool>(fileLoader);

    if (readFromDisk)
    {
        const auto error = fileLoader ? fileLoader(*input, filename)
                                      : Yuni::IO::File::LoadFromFile(*input,
                                                                     filename,
                                                                     matrixFilesizeHardLimit);
        if (error != Yuni::IO::errNone)
        {
            if (!(options & MatrixType<T, ReadWriteT>::optQuiet))
            {
                if (error == Yuni::IO::errNotFound)
                {
                    logs.error() << "I/O Error: not found: '" << filename << "'";
                }
                else if (error == Yuni::IO::errMemoryLimit)
                {
                    logs.error() << filename << ": The file is too large (>"
                                 << (matrixFilesizeHardLimit / 1024 / 1024) << "Mo)";
                }
                else
                {
                    logs.error() << "I/O Error: failed to load '" << filename << "'";
                }
            }
            matrix.reset(minWidth, maxHeight);
            return false;
        }
    }

    if (input->empty())
    {
        if (minWidth && maxHeight)
        {
            matrix.reset(minWidth, maxHeight);
        }
        else
        {
            matrix.clear();
        }
        return true;
    }

    if (readFromDisk)
    {
        Statistics::HasReadFromDisk(input->size());
    }

    std::string data(input->c_str(), input->size());
    data += '\n';
    const bool result = loadFromBuffer(matrix,
                                       filename,
                                       std::move(data),
                                       minWidth,
                                       maxHeight,
                                       (options & MatrixType<T, ReadWriteT>::optFixedSize) != 0,
                                       options);
    if (!result)
    {
        matrix.reset(minWidth, maxHeight);
    }
    return result;
}

template<class T, class ReadWriteT>
bool load(MatrixType<T, ReadWriteT>& matrix,
          const std::filesystem::path& filename,
          uint minWidth = 1,
          uint maxHeight = 0,
          uint options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix,
                AnyString(filename.string()),
                minWidth,
                maxHeight,
                options,
                buffer,
                fileLoader);
}

template<class T, class ReadWriteT>
bool load(MatrixType<T, ReadWriteT>& matrix,
          const std::string& filename,
          uint minWidth = 1,
          uint maxHeight = 0,
          uint options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, AnyString(filename), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T, class ReadWriteT>
bool load(MatrixType<T, ReadWriteT>& matrix,
          const char* filename,
          uint minWidth = 1,
          uint maxHeight = 0,
          uint options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, AnyString(filename), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T, class ReadWriteT = T, class Predicate = std::identity>
void saveToBuffer(const MatrixType<T, ReadWriteT>& matrix,
                  std::string& data,
                  uint precision = 6,
                  bool printDimensions = false,
                  Predicate predicate = {},
                  bool saveEvenIfAllZero = false)
{
    if (!printDimensions && !saveEvenIfAllZero && containsOnlyZero(matrix, predicate))
    {
        return;
    }

    matrix_to_buffer_dumper_factory factory;
    auto dumper = factory.get_dumper<T, ReadWriteT, Predicate>(&matrix, data, predicate);
    dumper->set_print_format(Yuni::Static::Type::IsDecimal<ReadWriteT>::Yes, precision);

    data.reserve(matrix.width() * matrix.height() * 6);
    if (printDimensions)
    {
        data += "size:" + std::to_string(matrix.width()) + 'x' + std::to_string(matrix.height())
                + '\n';
    }
    dumper->run();
}

template<class T, class ReadWriteT = T, class Predicate = std::identity>
bool save(const MatrixType<T, ReadWriteT>& matrix,
          const AnyString& filename,
          uint precision = 6,
          bool printDimensions = false,
          Predicate predicate = {},
          bool saveEvenIfAllZero = false)
{
    logs.debug() << "  :: writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';

    Yuni::IO::File::Stream file;
    if (!file.openRW(filename))
    {
        logs.error() << "I/O error: " << filename
                     << ": Impossible to write the file (not enough permission ?)";
        return false;
    }

    if (matrix.width() && matrix.height())
    {
        std::string data;
        saveToBuffer(matrix, data, precision, printDimensions, predicate, saveEvenIfAllZero);
        Statistics::HasWrittenToDisk(data.size());
        file << data;
    }

    logs.debug() << "  :: [end] writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';
    return true;
}

template<class T, class ReadWriteT = T>
bool saveToCSVFile(const MatrixType<T, ReadWriteT>& matrix,
                   const AnyString& filename,
                   uint precision = 6,
                   bool printDimensions = false,
                   bool saveEvenIfAllZero = false)
{
    return save(matrix, filename, precision, printDimensions, std::identity{}, saveEvenIfAllZero);
}
} // namespace Antares::MatrixIO

#endif
