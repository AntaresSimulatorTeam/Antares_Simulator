// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_PLEASE_WAIT_H__
#define __ANTARES_APPLICATION_PLEASE_WAIT_H__

namespace Antares
{
/*!
** \brief Work In Progress
*/
namespace WIP
{
/*!
** \brief Enter into a new blocking piece of code
*/
void Enter(bool immediate = true);

/*!
** \brief Lease a blocking piece of code
*/
void Leave();

class Locker final
{
public:
    Locker()
    {
        Enter();
    }

    ~Locker()
    {
        Leave();
    }
};

} // namespace WIP
} // namespace Antares

#endif // __ANTARES_APPLICATION_PLEASE_WAIT_H__
