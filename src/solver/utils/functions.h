/*
** Copyright 2007-2023 RTE
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
#ifndef ANTARES_FUNCTIONS_H
#define ANTARES_FUNCTIONS_H

#include "memory"
#include "functional"

/*!
 * Technical utilities related to function objects.
 */
namespace Antares::Functions
{

/**
 * Wraps a pointer to a function object into a std::function.
 * Type T must define operator().
 */
template<class T>
std::function<void()> MakeFunction(const std::shared_ptr<T>& functionObject);

/**
 * Creates a std::function wrapping a function object of type T,
 * constructed with the provided arguments.
 * Type T must define operator().
 * Allows to wrap non-copyable function objects into std::function.
 */
template<class T, typename... Args>
std::function<void()> MakeFunction(Args&& ... args);


namespace Detail { //implementation details

/*!
 * Utility class to wrap a callable object pointer
 * into a copyable callable object.
 *
 * @tparam T the underlying callable type
 */
template<class T>
class SharedCallable
{
public:
    explicit SharedCallable(const std::shared_ptr<T>& functionObject) :
            functionObject_(functionObject)
    {
    }

    void operator()()
    {
        functionObject_.operator->();
    }

private:
    std::shared_ptr<T> functionObject_;
};

}

template<class T>
std::function<void()> MakeFunction(const std::shared_ptr<T>& callable)
{
    return Detail::SharedCallable<T>(callable);
}

template<class T, typename... Args>
std::function<void()> MakeFunction(Args&& ... args)
{
    std::shared_ptr<T> callable = std::make_shared<T>(std::forward<Args>(args)...);
    return MakeFunction(callable);
}


}

#endif //ANTARES_FUNCTIONS_H
