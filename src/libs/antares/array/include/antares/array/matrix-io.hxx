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
#include <fstream>
#include <functional>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include <antares/io/statistics.h>
#include <antares/logs/logs.h>
#include <antares/utils/utils.h>

#include "matrix-to-buffer.h"

namespace Antares::MatrixIO
{
using BufferType = std::string;

enum class FileLoadError
{
    none,
    notFound,
    failed,
};

using FileLoader = std::function<FileLoadError(BufferType&, const std::string&)>;

template<class T, class ReadWriteT = T>
using MatrixType = Matrix<T, ReadWriteT>;

namespace // anonymous
{
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

    static void Copy(T&, const std::string&)
    {
        // This overload prevents an accidental numeric cast on the direct path.
        logs.error() << "internal error: matrix data conversion";
    }
};

template<>
class MatrixData<std::string> final
{
public:
    static void Init(std::string& data)
    {
        data.clear();
    }

    template<class U>
    static void Copy(std::string& data, const U& value)
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

    static bool Do(const std::string& str, ReadWriteT& out)
    {
        std::istringstream stream(str);
        stream >> out;
        stream >> std::ws;
        return !stream.fail() && stream.eof();
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

    static bool Do(const std::string& str, double& out)
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

    static bool Do(const std::string& str, float& out)
    {
        char* end = nullptr;
        out = static_cast<float>(::strtod(str.c_str(), &end));
        return end != nullptr && *end == '\0';
    }
};

template<>
class MatrixStringConverter<std::string> final
{
public:
    enum
    {
        direct = 1
    };

    static bool Do(const std::string& str, std::string& out)
    {
        out.assign(str);
        return true;
    }
};

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

bool detectEncoding(const std::string& filename, const std::string& data, size_t& offset)
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
                    const std::string& filename,
                    std::string data,
                    unsigned int minWidth,
                    unsigned int maxHeight,
                    bool fixedSize,
                    unsigned int options)
{
    logs.debug() << "  :: loading `" << filename << "`";

    size_t bom = 0;
    if (!detectEncoding(filename, data, bom))
    {
        matrix.reset(minWidth > 0 ? minWidth : 1, maxHeight);
        return false;
    }

    unsigned int offset = static_cast<unsigned int>(bom);
    unsigned int x = 0;

    if (fixedSize)
    {
        matrix.reset(minWidth, maxHeight);
    }
    else
    {
        if (!maxHeight)
        {
            maxHeight = static_cast<unsigned int>(std::count(data.begin(), data.end(), '\n'));
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

        offset = static_cast<unsigned int>(max + 1);
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
#ifdef _MSC_VER
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
            maxHeight = static_cast<unsigned int>(headerHeight);
            matrix.resize(static_cast<unsigned int>(headerWidth),
                          static_cast<unsigned int>(headerHeight));
        }
        else
        {
            offset = 0;
            x = max > 0 ? 1 : 0;

            if (max > 0)
            {
                while ((offset = static_cast<unsigned int>(data.find_first_of("\t;,", offset)))
                       < max)
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

            offset = static_cast<unsigned int>(bom);
        }
    }

    unsigned int y = 0;
    unsigned int pos = 0;
    int errorCount = 6;
    char separator = '\0';
    std::string converter;
    ReadWriteT cellValue{};
    bool result = true;

    while (y < maxHeight && offset < data.size())
    {
        x = 0;
        pos = offset;
        const unsigned int lineOffset = offset;

        while ((offset = static_cast<unsigned int>(data.find_first_of("\t\r\n;,", offset)))
               != static_cast<unsigned int>(std::string::npos))
        {
            separator = data[offset];
            converter.assign(data, pos, offset - pos);

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
                            unsigned int newOffset = offset;
                            unsigned int newWidth = matrix.width() + 1;
                            while ((newOffset = static_cast<unsigned int>(
                                      data.find_first_of("\t\r\n;,", newOffset)))
                                   != static_cast<unsigned int>(std::string::npos))
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
    for (unsigned int x = 0; x < matrix.width(); ++x)
    {
        for (unsigned int y = 0; y < matrix.height(); ++y)
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
          const std::string& filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    assert(!filename.empty());

    BufferType owned;
    BufferType* input = buffer ? buffer : &owned;
    const bool readFromDisk = buffer == nullptr || static_cast<bool>(fileLoader);

    if (readFromDisk)
    {
        const auto error = fileLoader ? fileLoader(*input, filename) : [&input, &filename]
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file)
            {
                return FileLoadError::notFound;
            }

            input->assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            return file.bad() ? FileLoadError::failed : FileLoadError::none;
        }();
        if (error != FileLoadError::none)
        {
            if (!(options & MatrixType<T, ReadWriteT>::optQuiet))
            {
                if (error == FileLoadError::notFound)
                {
                    logs.error() << "I/O Error: not found: '" << filename << "'";
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

    std::string data = *input;
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
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, filename.string(), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T, class ReadWriteT>
bool load(MatrixType<T, ReadWriteT>& matrix,
          const char* filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T, ReadWriteT>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, std::string(filename), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T, class ReadWriteT = T, class Predicate = std::identity>
void saveToBuffer(const MatrixType<T, ReadWriteT>& matrix,
                  std::string& data,
                  unsigned int precision = 6,
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
    dumper->set_print_format(std::is_floating_point_v<ReadWriteT>, precision);

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
          const std::string& filename,
          unsigned int precision = 6,
          bool printDimensions = false,
          Predicate predicate = {},
          bool saveEvenIfAllZero = false)
{
    logs.debug() << "  :: writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';

    std::ofstream file(filename, std::ios::binary | std::ios::trunc);
    if (!file)
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
        if (!file)
        {
            logs.error() << "I/O error: " << filename << ": Failed to write the file";
            return false;
        }
    }

    logs.debug() << "  :: [end] writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';
    return true;
}

template<class T, class ReadWriteT = T>
bool saveToCSVFile(const MatrixType<T, ReadWriteT>& matrix,
                   const std::string& filename,
                   unsigned int precision = 6,
                   bool printDimensions = false,
                   bool saveEvenIfAllZero = false)
{
    return save(matrix, filename, precision, printDimensions, std::identity{}, saveEvenIfAllZero);
}
} // namespace Antares::MatrixIO

#endif
