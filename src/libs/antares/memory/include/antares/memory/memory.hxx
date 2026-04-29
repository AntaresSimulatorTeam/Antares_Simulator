// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_MEMORY_MEMORY_HXX__
#define __ANTARES_LIBS_MEMORY_MEMORY_HXX__

namespace Antares
{
inline uint64_t Memory::processID() const
{
    return pProcessID;
}

template<class T>
inline void Memory::Release(T*& pointer)
{
    delete[] pointer;
    pointer = nullptr;
}

template<class T>
inline void Memory::Allocate(T*& out, size_t size)
{
    assert(size > 0);
    out = new T[size];
}

template<class T>
inline bool Memory::Null(const T* out)
{
    return !out;
}

template<class T>
inline bool Memory::StrictNull(const T* out)
{
    return !out;
}

template<class U>
inline U* Memory::RawPointer(U* array)
{
    return array;
}

template<class U>
inline void Memory::Zero(uint count, U* array)
{
    (void)::memset(array, 0, sizeof(U) * count);
}

template<class U>
inline void Memory::Assign(uint count, U* array, const U& value)
{
    for (uint i = 0; i != count; ++i)
    {
        array[i] = value;
    }
}

} // namespace Antares

#endif // __ANTARES_LIBS_MEMORY_MEMORY_HXX__
