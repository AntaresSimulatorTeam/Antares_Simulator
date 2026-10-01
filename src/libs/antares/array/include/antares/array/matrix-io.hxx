// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef ANTARES_ARRAY_MATRIX_IO_HXX
#define ANTARES_ARRAY_MATRIX_IO_HXX

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

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
    tooLarge,
    failed,
};

using FileLoader = std::function<FileLoadError(BufferType&, const std::string&)>;

template<class T>
using MatrixType = Matrix<T>;

namespace // anonymous
{
constexpr std::uintmax_t matrixFileSizeLimit = 1536ULL * 1024ULL * 1024ULL;

struct PopBackGuard final
{
    std::string& value;

    ~PopBackGuard()
    {
        value.pop_back();
    }
};

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

template<class T>
class MatrixStringConverter final
{
public:
    enum
    {
        direct = 0
    };

    static bool Do(const std::string& str, T& out)
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

bool replaceFile(const std::filesystem::path& temporary,
                 const std::filesystem::path& filename,
                 std::error_code& error)
{
#ifdef _WIN32
    const bool targetExists = std::filesystem::exists(filename, error);
    if (error)
    {
        return false;
    }

    const bool replaced = targetExists ? ReplaceFileW(filename.c_str(),
                                                      temporary.c_str(),
                                                      nullptr,
                                                      REPLACEFILE_WRITE_THROUGH,
                                                      nullptr,
                                                      nullptr)
                                       : MoveFileExW(temporary.c_str(),
                                                     filename.c_str(),
                                                     MOVEFILE_WRITE_THROUGH);
    if (!replaced)
    {
        error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
        return false;
    }

    error.clear();
    return true;
#else
    std::filesystem::rename(temporary, filename, error);
    return !error;
#endif
}

std::filesystem::path createTemporaryDirectory(const std::filesystem::path& target,
                                               std::error_code& error)
{
    static std::atomic_uint64_t sequence = 0;
    const auto parent = target.parent_path().empty() ? std::filesystem::path(".")
                                                     : target.parent_path();
    const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto prefix = target.filename().string() + ".tmp-" + std::to_string(timestamp) + '-';

    for (unsigned int attempt = 0; attempt != 100; ++attempt)
    {
        const auto directory = parent / (prefix + std::to_string(sequence.fetch_add(1)));
        error.clear();
        if (std::filesystem::create_directory(directory, error))
        {
            return directory;
        }
        if (error != std::errc::file_exists)
        {
            return {};
        }
    }

    error = std::make_error_code(std::errc::file_exists);
    return {};
}

template<class T>
bool loadFromBuffer(Matrix<T>& matrix,
                    const std::string& filename,
                    const std::string& data,
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
                if (!(options & Matrix<T>::optQuiet))
                {
                    logs.warning() << '`' << filename << "`: Invalid header";
                }
                headerWidth = 1;
            }
            if (headerHeight < 1)
            {
                if (!(options & Matrix<T>::optQuiet))
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
                if (!(options & Matrix<T>::optQuiet) && !(options & Matrix<T>::optNoWarnIfEmpty))
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
    T cellValue{};
    bool result = true;

    const auto handleInvalidNumericValue =
      [&](unsigned int cellX, unsigned int cellY, unsigned int cellOffset)
    {
        result = false;
        if (!(options & Matrix<T>::optQuiet) && errorCount)
        {
            logs.warning() << '`' << filename << "`: Invalid numeric value (x:" << cellX
                           << ",y:" << cellY << ", offset: " << cellOffset << "byte), text: `"
                           << converter << " read:" << matrix[cellX][cellY] << '`';
            if (!(--errorCount))
            {
                logs.warning() << " ... (skipped)";
            }
        }
        MatrixData<T>::Init(matrix[cellX][cellY]);
    };

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
                    if (options & Matrix<T>::optNeverFails)
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
                        if (!(options & Matrix<T>::optQuiet) && errorCount > 0)
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

                if (MatrixStringConverter<T>::direct)
                {
                    MatrixData<T>::Copy(matrix[x][y], converter);
                }
                else if (!MatrixStringConverter<T>::Do(converter, cellValue))
                {
                    double fallback = 0;
                    if (!MatrixStringConverter<double>::Do(converter, fallback))
                    {
                        handleInvalidNumericValue(x, y, pos);
                    }
                    else
                    {
                        if constexpr (std::is_integral_v<T>)
                        {
                            if (!std::isfinite(fallback)
                                || fallback < static_cast<double>(std::numeric_limits<T>::min())
                                || (std::numeric_limits<T>::digits
                                        > std::numeric_limits<double>::digits
                                      ? fallback
                                          >= static_cast<double>(std::numeric_limits<T>::max())
                                      : fallback
                                          > static_cast<double>(std::numeric_limits<T>::max())))
                            {
                                handleInvalidNumericValue(x, y, pos);
                            }
                            else
                            {
                                matrix[x][y] = static_cast<T>(std::trunc(fallback));
                            }
                        }
                        else
                        {
                            matrix[x][y] = MatrixRound<T, double>::Value(fallback);
                        }
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
                if (!(options & Matrix<T>::optQuiet))
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
            if (!(options & Matrix<T>::optNeverFails))
            {
                result = false;
                if (!(options & Matrix<T>::optQuiet) && errorCount)
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
        if (!(options & Matrix<T>::optQuiet))
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

    return (options & Matrix<T>::optNeverFails) ? true : result;
}

} // anonymous namespace

template<class T>
bool load(MatrixType<T>& matrix,
          const std::string& filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T>::optNone,
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

            std::error_code ec;
            const auto size = std::filesystem::file_size(filename, ec);
            if (ec)
            {
                return FileLoadError::failed;
            }

            if (size > matrixFileSizeLimit)
            {
                return FileLoadError::tooLarge;
            }

            input->resize(static_cast<std::size_t>(size));
            file.read(input->data(), static_cast<std::streamsize>(size));
            return file ? FileLoadError::none : FileLoadError::failed;
        }();
        if (error != FileLoadError::none)
        {
            if (!(options & MatrixType<T>::optQuiet))
            {
                if (error == FileLoadError::notFound)
                {
                    logs.error() << "I/O Error: not found: '" << filename << "'";
                }
                else if (error == FileLoadError::tooLarge)
                {
                    logs.error() << filename << ": The file is too large (>"
                                 << (matrixFileSizeLimit / 1024 / 1024) << "Mo)";
                }
                else
                {
                    logs.error() << "I/O Error: failed to load '" << filename << "'";
                }
            }
            matrix.reset(minWidth, maxHeight);
            return false;
        }

        if (input->size() > matrixFileSizeLimit)
        {
            if (!(options & MatrixType<T>::optQuiet))
            {
                logs.error() << filename << ": The file is too large (>"
                             << (matrixFileSizeLimit / 1024 / 1024) << "Mo)";
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

    input->push_back('\n');
    PopBackGuard guard{*input};
    const bool result = loadFromBuffer(matrix,
                                       filename,
                                       *input,
                                       minWidth,
                                       maxHeight,
                                       (options & MatrixType<T>::optFixedSize) != 0,
                                       options);
    if (!result)
    {
        matrix.reset(minWidth, maxHeight);
    }
    return result;
}

template<class T>
bool load(MatrixType<T>& matrix,
          const std::filesystem::path& filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, filename.string(), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T>
bool load(MatrixType<T>& matrix,
          const char* filename,
          unsigned int minWidth = 1,
          unsigned int maxHeight = 0,
          unsigned int options = MatrixType<T>::optNone,
          BufferType* buffer = nullptr,
          const FileLoader& fileLoader = {})
{
    return load(matrix, std::string(filename), minWidth, maxHeight, options, buffer, fileLoader);
}

template<class T, class Predicate = std::identity>
void saveToBuffer(const MatrixType<T>& matrix,
                  std::string& data,
                  unsigned int precision = 6,
                  bool printDimensions = false,
                  Predicate predicate = {},
                  bool saveEvenIfAllZero = false)
{
    if (!printDimensions && !saveEvenIfAllZero && matrix.containsOnlyZero(predicate))
    {
        data.clear();
        return;
    }

    std::string serialized;
    matrix_to_buffer_dumper_factory factory;
    auto dumper = factory.get_dumper<T, Predicate>(&matrix, serialized, predicate);
    dumper->set_print_format(std::is_floating_point_v<T>, precision);

    serialized.reserve(matrix.width() * matrix.height() * 6);
    if (printDimensions)
    {
        serialized += "size:" + std::to_string(matrix.width()) + 'x'
                      + std::to_string(matrix.height()) + '\n';
    }
    dumper->run();
    data = std::move(serialized);
}

template<class T, class Predicate = std::identity>
bool save(const MatrixType<T>& matrix,
          const std::string& filename,
          unsigned int precision = 6,
          bool printDimensions = false,
          Predicate predicate = {},
          bool saveEvenIfAllZero = false)
{
    logs.debug() << "  :: writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';

    const std::filesystem::path target(filename);
    if (target.empty())
    {
        logs.error() << "I/O error: " << filename
                     << ": Impossible to write the file (not enough permission ?)";
        return false;
    }

    std::string data;
    if (matrix.width() && matrix.height())
    {
        saveToBuffer(matrix, data, precision, printDimensions, predicate, saveEvenIfAllZero);
    }

    if (data.size() > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
    {
        logs.error() << "I/O error: " << filename << ": Failed to write the file";
        return false;
    }

    std::error_code temporaryError;
    const auto temporaryDirectory = createTemporaryDirectory(target, temporaryError);
    if (temporaryError || temporaryDirectory.empty())
    {
        logs.error() << "I/O error: " << filename << ": Failed to create a temporary file";
        return false;
    }
    const auto temporary = temporaryDirectory / "data";

    bool writeSucceeded = false;
    {
        std::ofstream file(temporary, std::ios::binary | std::ios::trunc);
        if (file)
        {
            file.write(data.data(), static_cast<std::streamsize>(data.size()));
            file.close();
            writeSucceeded = static_cast<bool>(file);
        }
    }

    if (!writeSucceeded)
    {
        std::error_code cleanupError;
        std::filesystem::remove_all(temporaryDirectory, cleanupError);
        logs.error() << "I/O error: " << filename << ": Failed to write the file";
        return false;
    }

    std::error_code renameError;
    if (!replaceFile(temporary, target, renameError))
    {
        std::error_code cleanupError;
        std::filesystem::remove_all(temporaryDirectory, cleanupError);
        logs.error() << "I/O error: " << filename << ": Failed to replace the file";
        return false;
    }

    std::error_code cleanupError;
    std::filesystem::remove(temporaryDirectory, cleanupError);
    Statistics::HasWrittenToDisk(data.size());
    logs.debug() << "  :: [end] writing `" << filename << "' (" << matrix.width() << 'x'
                 << matrix.height() << ')';
    return true;
}

template<class T>
bool saveToCSVFile(const MatrixType<T>& matrix,
                   const std::string& filename,
                   unsigned int precision = 6,
                   bool printDimensions = false,
                   bool saveEvenIfAllZero = false)
{
    return save(matrix, filename, precision, printDimensions, std::identity{}, saveEvenIfAllZero);
}
} // namespace Antares::MatrixIO

#endif
