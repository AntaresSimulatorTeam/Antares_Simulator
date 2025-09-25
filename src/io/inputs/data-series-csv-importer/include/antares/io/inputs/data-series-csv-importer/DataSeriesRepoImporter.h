/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <antares/optimisation/linear-problem-data-impl/dataSeriesRepo.h>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
/**
 * Reads a DataSeriesRepo from a directory
 * Every csv file found represents a DataSeries, its id will be the name of the file
 */
namespace Antares::IO::Inputs::DataSeriesCsvImporter
{

class MappedFile
{
public:
    explicit MappedFile(const char* filename)
    {
        if (!filename)
        {
            throw std::invalid_argument("Filename cannot be null");
        }

#if defined(_WIN32)
        handle_ = CreateFileA(filename,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL,
                              nullptr);
        if (handle_ == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Failed to open file: " + std::string(filename));
        }

        LARGE_INTEGER size;
        if (!GetFileSizeEx(handle_, &size))
        {
            CloseHandle(handle_);
            throw std::runtime_error("Failed to get file size");
        }
        size_ = static_cast<size_t>(size.QuadPart);

        // Handle empty files
        if (size_ == 0)
        {
            data_ = nullptr;
            return;
        }

        mapping_ = CreateFileMappingA(handle_, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (!mapping_)
        {
            CloseHandle(handle_);
            throw std::runtime_error("Failed to create file mapping");
        }

        data_ = static_cast<const char*>(MapViewOfFile(mapping_, FILE_MAP_READ, 0, 0, size_));
        if (!data_)
        {
            CloseHandle(mapping_);
            CloseHandle(handle_);
            throw std::runtime_error("Failed to map file");
        }
#else
        fd_ = open(filename, O_RDONLY);
        if (fd_ < 0)
        {
            throw std::runtime_error("Failed to open file: " + std::string(filename));
        }

        struct stat sb{};
        if (fstat(fd_, &sb) < 0)
        {
            close(fd_);
            throw std::runtime_error("Failed to stat file");
        }
        size_ = static_cast<size_t>(sb.st_size);

        // Handle empty files
        if (size_ == 0)
        {
            data_ = nullptr;
            return;
        }

        data_ = static_cast<const char*>(mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0));
        if (data_ == MAP_FAILED)
        {
            close(fd_);
            throw std::runtime_error("Failed to mmap file");
        }

// Optional: Hint for sequential access
#ifdef MADV_SEQUENTIAL
        madvise(const_cast<void*>(static_cast<const void*>(data_)), size_, MADV_SEQUENTIAL);
#endif
#endif
    }

    ~MappedFile()
    {
#if defined(_WIN32)
        if (data_)
        {
            UnmapViewOfFile(data_);
        }
        if (mapping_)
        {
            CloseHandle(mapping_);
        }
        if (handle_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle_);
        }
#else
        if (data_ && data_ != MAP_FAILED)
        {
            munmap(const_cast<void*>(static_cast<const void*>(data_)), size_);
        }
        if (fd_ >= 0)
        {
            close(fd_);
        }
#endif
    }

    // Delete copy constructor and assignment operator
    MappedFile(const MappedFile&) = delete;
    MappedFile& operator=(const MappedFile&) = delete;

    // Move constructor and assignment
    MappedFile(MappedFile&& other) noexcept:
        data_(other.data_),
        size_(other.size_)
    {
#if defined(_WIN32)
        handle_ = other.handle_;
        mapping_ = other.mapping_;
        other.handle_ = INVALID_HANDLE_VALUE;
        other.mapping_ = nullptr;
#else
        fd_ = other.fd_;
        other.fd_ = -1;
#endif
        other.data_ = nullptr;
        other.size_ = 0;
    }

    MappedFile& operator=(MappedFile&& other) noexcept
    {
        if (this != &other)
        {
            // Clean up current resources
            this->~MappedFile();

            // Move from other
            data_ = other.data_;
            size_ = other.size_;
#if defined(_WIN32)
            handle_ = other.handle_;
            mapping_ = other.mapping_;
            other.handle_ = INVALID_HANDLE_VALUE;
            other.mapping_ = nullptr;
#else
            fd_ = other.fd_;
            other.fd_ = -1;
#endif
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    const char* data() const
    {
        return data_;
    }

    size_t size() const
    {
        return size_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    // Iterator-style interface
    template<typename Func>
    void for_each_line(Func&& processor) const
    {
        if (empty())
        {
            return;
        }

        const char* start = data_;
        const char* end = data_ + size_;

        while (start < end)
        {
            const char* newline = static_cast<const char*>(memchr(start, '\n', end - start));
            if (!newline)
            {
                newline = end;
            }

            // Handle Windows line endings (\r\n)
            size_t line_len = newline - start;
            if (line_len > 0 && start[line_len - 1] == '\r')
            {
                line_len--;
            }

            // std::string_view line(start, line_len);
            std::string line(start, line_len);
            processor(line);

            start = newline + 1;
        }
    }

private:
#if defined(_WIN32)
    HANDLE handle_{INVALID_HANDLE_VALUE};
    HANDLE mapping_{nullptr};
#else
    int fd_{-1};
#endif
    const char* data_{nullptr};
    size_t size_{0};
};

class DataSeriesRepoImporter final
{
public:
    DataSeriesRepoImporter() = delete; // must not be used
    static Optimisation::LinearProblemDataImpl::DataSeriesRepository importFromDirectory(
      const std::filesystem::path&,
      char csvSeparator = '\t');
};

} // namespace Antares::IO::Inputs::DataSeriesCsvImporter
