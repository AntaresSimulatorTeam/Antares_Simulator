/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#ifndef __ANTARES_LIBS_MEMORY_MEMORY_H__
#define __ANTARES_LIBS_MEMORY_MEMORY_H__

#include <yuni/core/bit/array.h>
#include <yuni/core/string.h>
#include <yuni/yuni.h>

namespace Antares
{
/*!
** \brief Custom memory allocator for managed pointers
*/
class Memory final: public Yuni::Policy::ObjectLevelLockable<Memory>
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
    static bool StrictNull(const T* out);

    /*!
    ** \brief Release a raw pointer
    */
    template<class T>
    static void Release(T*& pointer);

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
    uint64_t memoryUsage() const;

    /*!
    ** \brief Get the process ID of the application
    **
    ** This value is cached and detected at the creation of the instance.
    */
    uint64_t processID() const;

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
    uint64_t pProcessID;

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
