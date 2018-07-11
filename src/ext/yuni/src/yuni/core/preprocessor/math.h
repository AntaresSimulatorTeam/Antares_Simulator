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
#pragma once


/*!
** \brief Check if a number if a power of 2
**
** The method used is "Complement and Compare"
*/
#define YUNI_IS_POWER_OF_TWO(x)  ((x != 0) && ((x & (~x + 1)) == x))


/*!
** \brief Compute x + y
*/
#define YUNI_ADD(x, y)  ((x) + (y))

/*!
** \brief Compute x - y
*/
#define YUNI_SUB(x, y)  ((x) - (y))

/*!
** \brief Compute x % y
*/
#define YUNI_MOD(x, y)  ((x) % (y))

/*!
** \brief Compute x / y
*/
#define YUNI_DIV(x, y)  ((x) / (y))

/*!
** \brief Compute x * y
*/
#define YUNI_MUL(x, y)  ((x) * (y))





/*!
** \brief Logic: x && y
*/
#define YUNI_AND(x, y)  ((x) && (y))

/*!
** \brief Logic: x || y
*/
#define YUNI_OR(x, y)  ((x) || (y))

/*!
** \brief Logic: !(x || y)
*/
#define YUNI_NOR(x, y)  (!((x) || (y)))

/*!
** \brief Logic: not
*/
#define YUNI_NOT(x)  (!((x)))





/*!
** \brief Logic: x & y
*/
#define YUNI_BITAND(x, y)  ((x) & (y))

/*!
** \brief Logic: x | y
*/
#define YUNI_BITOR(x, y)  ((x) | (y))

/*!
** \brief Logic: !(x | y)
*/
#define YUNI_BITNOR(x, y)  (!((x) | (y)))
