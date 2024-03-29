/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#ifndef __YUNI_CORE_VALIDATOR_H__
#define __YUNI_CORE_VALIDATOR_H__

namespace Yuni
{
namespace Validator
{
enum DefaultPolicy
{
    //! Deny all by default
    denyByDefault = 0,
    //! All all by default
    allowByDefault = 1,
};

} // namespace Validator
} // namespace Yuni

#endif // __YUNI_CORE_VALIDATOR_H__
