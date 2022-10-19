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
#ifndef __ANTARES_LIBS_MEMORY_MEMORY_H__
#define __ANTARES_LIBS_MEMORY_MEMORY_H__

#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/bit/array.h>


namespace Antares
{
namespace Data
{
    class StudyMemoryUsage;
} // namespace Data
} // namespace Antares

namespace Antares
{
/*!
** \brief Custom memory allocator for managed pointers
*/
class Memory final : public Yuni::Policy::ObjectLevelLockable<Memory>
{
public:
    template<class T>
    class Array final
    {
    public:
        //! \name Constructors
        //@{
        /*!
        ** \brief Default constructor
        */
        Array() = default;

        /*!
        ** \brief Constructor from null
        */
        explicit Array(const Yuni::NullPtr&);

        /*!
        ** \brief Constructor with an initial allocation size
        */
        explicit Array(size_t size);

        //! Copy constructor (must be empty)
        Array(const Array& copy);

        template<class U>
        Array(const Array<U>&);

        /*!
        ** \brief Destructor
        */
        ~Array();
        //@}

        /*!
        ** \brief
        */
        void allocate(size_t size);

        T& operator[](uint i);
        const T& operator[](uint i) const;

    private:
        T* pPointer = nullptr;

    }; // class Array

    template<class T>
    struct Stored final
    {
        using Type = T*;
        using ReturnType = T*;
        using ConstReturnType = const T*;

        static const T* NullValue()
        {
            return nullptr;
        }
    };

    template<class U>
    static U* RawPointer(U* array);

    template<class U>
    static void Zero(uint count, U* array);

    template<class U>
    static void Assign(uint count, U* array, const U& value);

public:

    template<class T>
    static void Allocate(T*& out, size_t size);

    template<class T>
    static bool Null(const T* out);

    template<class T>
    static bool StrictNull(const T*& out);

    /*!
    ** \brief Release a raw pointer
    */
    template<class T>
    static void Release(T*& pointer);

    /*!
    ** \brief Estimate the memory usage if our managed pointers are used
    **
    ** \param     bytes  A size in bytes
    ** \param     count  The number of element of size `bytes` to allocate
    */
    static void EstimateMemoryUsage(size_t bytes,
                                    uint count,
                                    Data::StudyMemoryUsage& u,
                                    bool duplicateForParallelYears);

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Memory() = default;
    /*!
    ** \brief Destructor
    */
    ~Memory() = default;
    //@}
    
    bool initializeTemporaryFolder();
    //@}

    //! \nane Cache Folder
    //@{
    const Yuni::String& cacheFolder() const;
    void cacheFolder(const AnyString& folder);
    //@}

    /*!
    ** \brief Get the amount of memory currently used
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Get the process ID of the application
    **
    ** This value is cached and detected at the creation of the instance.
    */
    Yuni::uint64 processID() const;

    //@}

    //! \name Informations
    //@{
    //! Display infos into the logs
    void displayInfo() const;
    //@}

private:
    //! The cache folder
    Yuni::String pCacheFolder;
    //! Get if the user is allowed to modify this value
    bool pAllowedToChangeCacheFolder;

    //! The process ID used
    Yuni::uint64 pProcessID;

    //! Avoid multiple initializations
    bool pAlreadyInitialized = false;

    // Friend
    template<class T>
    friend class Array;

}; // class Memory

//! The memory manager
extern Memory memory;

} // namespace Antares

#include "memory.hxx"

#endif // __ANTARES_LIBS_MEMORY_MEMORY_H__
