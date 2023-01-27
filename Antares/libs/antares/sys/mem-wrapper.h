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
#ifndef __ANTARES_LIBS_SYS_MEM_WRAPPER_H__
#define __ANTARES_LIBS_SYS_MEM_WRAPPER_H__

#include <yuni/yuni.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*!
    ** \brief Allocate unused space and fill it with zeros
    **
    ** The routine ensures that you have a valid pointer.
    **
    ** \code
    ** void* p = malloc(size);
    ** memset(p, 0, size);
    ** \endcode
    **
    ** \param blocksize Size of the block to allocate
    ** \return A pointer to the allocated block
    */
    void* MemAllocMemset(const size_t blocksize);

    /*!
    ** \brief Allocate unused space
    **
    ** The routine ensures that you have a valid pointer.
    **
    ** The block will not be initialized with zero values
    ** (consequently it has the same behavior than malloc())
    **
    ** Equivalent code :
    ** \code
    ** malloc(size);
    ** \endcode
    **
    ** \param blocksize Size of the block to allocate
    ** \return A pointer to the allocated block
    */
    void* MemAlloc(const size_t blocksize);

    /*!
    ** \brief Change the size of an allocated space
    **
    ** The routine ensures that you have a valid pointer.
    **
    ** \param p The original pointer allocated by MemAlloc()
    ** \param blsize Its new size
    */
    void* MemRealloc(void* p, const size_t blsize);

/*!
** \brief Release a resource previously allocated by MEM_Malloc()
**
** \see MemAlloc()
** \see MemRealloc()
*/
#define MemFree(P) ::free(P)

#ifdef __cplusplus
}
#endif

#endif /* __ANTARES_LIBS_SYS_MEM_WRAPPER_H__ */
