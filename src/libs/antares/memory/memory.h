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
namespace Private
{
namespace Memory
{
// Forward declaration
class SwapFileInfo;

} // namespace Memory
} // namespace Private
} // namespace Antares

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
    //! Threading policy
    using ThreadingPolicy = Yuni::Policy::ObjectLevelLockable<Memory>;
    //! Internal handle
    using Handle = uint;

    //! Information about a single swap file
    using SwapFileInfo = Antares::Private::Memory::SwapFileInfo;

    enum
    {
        reuseMappingBlockCapacity = 10000,
    };

    struct Mapping
    {
    public:
        //! Reference to the original pointer
        volatile void** pointer;

        //! Pointer to the swap file info
        SwapFileInfo* swapFile;
        //! Offset within the swap file
        uint offset;
        //! The number of block used by this data block
        uint nbBlocks;
    };

    enum // anonymous
    {
/*!
** \brief Size of a single memory block (must be a power of 2)
*/
#ifdef YUNI_OS_WINDOWS
        blockSize = 64 * 1024, // On windows, the offset must be a multiple of the granularity
#else
        blockSize = 32 * 1024, // man getpagesize()
#endif

        //! Size of a single swap file (must be a power of 2)
        swapSize = 512 * 1024 * 1024,
        //! Blocks per swap file
        blockPerSwap = swapSize / blockSize,
        /*!
        ** \brief Minimal allocation size required to be managed by our custom memory cache
        **
        ** Below this value, the system allocator will be used
        */
        minimalAllocationSize = blockSize - (blockSize / 4),

        //! Invalid handle
        invalidHandle = 0,
    };

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
        Array();

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

        /*!
        ** \brief Try to acquire the inner pointer
        */
        void acquire();

        /*!
        ** \brief Tell if the data need to be flushed
        */
        bool needFlush() const;

        bool valid() const;

        bool strictNull() const;

        /*!
        ** \brief Flush the data into the swap file
        */
        void flush() const;

        /*!
        ** \brief Release the inner data
        */
        void release();

        //! \name Common operations on Array
        //@{
        /*!
        ** \brief Reset 'count' elements to zero
        */
        void assign(uint count);

        /*!
        ** \brief Reset 'count' elements to a given value
        */
        void assign(uint count, const T& value);

        /*!
        ** \brief Copy a whole array
        */
        void copy(uint count, const T* value);
        void copy(uint count, const Memory::Array<T>& value);

        /*!
        ** \brief Increment @count elements with a given value
        */
        void increment(uint count, const T& value);
        /*!
        ** \brief Increment @count elements with another array of @count elements
        */
        void increment(uint count, const T* value);
        /*!
        ** \brief Increment @count elements with another array of @count elements
        */
        void increment(uint count, const Memory::Array<T>& value);

        /*!
        ** \brief Multiply @count elements with a given value
        */
        void multiply(uint count, const T& value);
        /*!
        ** \brief Multiply @count elements with another array of @count elements
        */
        void multiply(uint count, const T* value);
        /*!
        ** \brief Multiply @count elements with another array of @count elements
        */
        void multiply(uint count, const Memory::Array<T>& value);
        //@}

        T* rawptr();
        const T* rawptr() const;

        //! \name Operators
        //@{
        operator void*();
        operator const void*() const;

        T* operator->();
        const T* operator->() const;
        const T& operator*() const;
        T& operator*();
        T& operator[](uint i);
        const T& operator[](uint i) const;
        bool operator!() const;
        Array& operator=(const Array<T>& copy);
        Array& operator=(const Yuni::NullPtr&);
        //@}

    private:
#ifdef ANTARES_SWAP_SUPPORT
        mutable volatile T* pPointer;
        Memory::Handle pHandle;
#else
        T* pPointer;
#endif

    }; // class Array

    template<class T>
    struct Stored final
    {
#ifdef ANTARES_SWAP_SUPPORT
        using Type = Array<T>;
        using ReturnType = Array<T>&;
        using ConstReturnType = const Array<T>&;

        static const Type& NullValue()
        {
            static const Type value;
            return value;
        }
#else
        using Type = T*;
        using ReturnType = T*;
        using ConstReturnType = const T*;

        static const T* NullValue()
        {
            return nullptr;
        }
#endif
    };

    template<class U>
    static U* RawPointer(U* array);

    template<class U>
    static U* RawPointer(Array<U>& array);

    template<class U>
    static const U* RawPointer(const Array<U>& array);

    template<class U>
    static void Zero(uint count, U* array);

    template<class U>
    static void Zero(uint count, Array<U>& array);

    template<class U>
    static void Assign(uint count, U* array);

    template<class U>
    static void Assign(uint count, Array<U>& array);

    template<class U>
    static void Assign(uint count, U* array, const U& value);

    template<class U>
    static void Assign(uint count, Array<U>& array, const U& value);

public:
#ifdef ANTARES_SWAP_SUPPORT
    template<class T>
    static void Allocate(Antares::Memory::Array<T>& out, size_t size);
#else
    template<class T>
    static void Allocate(T*& out, size_t size);
#endif

#ifdef ANTARES_SWAP_SUPPORT
    template<class T>
    static void Flush(Antares::Memory::Array<T>& out);
#else
    template<class T>
    static void Flush(T*&);
#endif

#ifdef ANTARES_SWAP_SUPPORT
    template<class T>
    static void Acquire(Antares::Memory::Array<T>& out);
#else
    template<class T>
    static void Acquire(T*&);
#endif

#ifdef ANTARES_SWAP_SUPPORT
    template<class T>
    static bool Null(const Antares::Memory::Array<T>& out);
#else
    template<class T>
    static bool Null(const T* out);
#endif

#ifdef ANTARES_SWAP_SUPPORT
    template<class T>
    static bool StrictNull(const Antares::Memory::Array<T>& out);
#else
    template<class T>
    static bool StrictNull(const T*& out);
#endif

    /*!
    ** \brief Release a raw pointer
    */
    template<class T>
    static void Release(T*& pointer);

    /*!
    ** \brief Release a managed pointer
    */
    template<class T>
    static void Release(Array<T>& pointer);

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
    Memory();
    /*!
    ** \brief Destructor
    */
    ~Memory();
    //@}

    //! \name Initialize
    //@{
    /*!
    ** \brief Initialize the underlying engine
    **
    ** This method must not be called from the constructor.. There is a circular
    ** reference issue when initializing global variables.
    */
    bool initialize();
    //@}

    //! \name Cleanup
    //@{
    /*!
    ** \brief Try to remove all no longer needed swap file to recover disk space
    */
    void cleanupCacheFolder() const;
    //@}

    //! \nane Cache Folder
    //@{
    const Yuni::String& cacheFolder() const;
    void cacheFolder(const AnyString& folder);
    //@}

    //! \name Memory management
    //@{
    /*!
    ** \brief Flush all memory into swap files
    */
    void flushAll() const;

    /*!
    ** \brief Get the amount of memory currently used
    */
    Yuni::uint64 memoryUsage() const;

    /*!
    ** \brief Get the amount of memory currently used by the swap
    */
    Yuni::uint64 memoryUsageInSwap() const;

    /*!
    ** \brief Get the current capacity
    */
    Yuni::uint64 memoryCapacity() const;

    /*!
    ** \brief Remove all unused swap files
    **
    ** In some cases, one or more swap files may remain to avoid useless
    ** allocation/deallocation.
    */
    void removeAllUnusedSwapFiles();

    /*!
    ** \brief Release the whole memory used by the memory cache
    **
    ** This method will destroyed all swap files. All futur calls to `release()`
    ** will produce a double free error (except for all newly allocated memory blocks)
    ** If unsure, This method should not be called.
    */
    void releaseAll();

    /*!
    ** \brief Get the process ID of the application
    **
    ** This value is cached and detected at the creation of the instance.
    */
    Yuni::uint64 processID() const;

    /*!
    ** \brief Ensure that at least one swap file is available
    */
    void ensureOneSwapFile();
    //@}

    //! \name Informations
    //@{
    //! Display infos into the logs
    void displayInfo() const;

    //! Dump to the logs informations about all swap files
    void dumpSwapFilesInfos() const;
    //@}

private:
    /*!
    ** \brief Allocate a new memory data block
    **
    ** \param count The number of object T to allocate
    ** \return A internal handle
    */
    template<class T>
    Handle allocate(size_t count);

    /*!
    ** \brief Release
    */
    void release(Handle handle);

    void releaseWL(Mapping& mapping);

    void* acquireMapping(Handle handle, volatile void* pointer);

    void flush(Handle handle);

    void dumpSwapFilesInfosWL() const;

    /*!
    ** \brief Create a new swap file
    */
    bool createNewSwapFileWL();

    /*!
    ** \brief Allocate a new region into a swap file
    **
    ** \return An internal handle to retrieve a real pointer for R/W access
    */
    Handle internalAllocate(size_t size);

    /*!
    ** \brief Initialize the prefix to use for swap filename
    */
    void initializeSwapFilePrefix();

private:
    //! The cache folder
    Yuni::String pCacheFolder;
    //! Get if the user is allowed to modify this value
    bool pAllowedToChangeCacheFolder;
    //! Swap file prefix
    Yuni::String pSwapFilePrefix;
    Yuni::CString<10, false> pSwapFilePrefixProcessID;

    /*!
    ** \brief Flag to disable the creation of a new swap file
    **
    ** When we have detected that it is impossible to allocate a new
    ** swap file, it is useless to try each time. We would prefer
    ** to wait for the next deletion of a swap file.
    */
    bool waitForSwapFileDeletion;

    //! The process ID used
    Yuni::uint64 pProcessID;

    //! Avoid multiple initializations
    bool pAlreadyInitialized;

    // Friend
    template<class T>
    friend class Array;

}; // class Memory

//! The memory manager
extern Memory memory;

} // namespace Antares

#include "memory.hxx"

#endif // __ANTARES_LIBS_MEMORY_MEMORY_H__
