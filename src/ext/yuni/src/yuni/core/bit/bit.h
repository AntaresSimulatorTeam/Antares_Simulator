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
#include "../../yuni.h"



/*!
** \brief Get if the Nth bit is set in a raw char* buffer
** \ingroup Bit
*/
# define YUNI_BIT_GET(DATA,INDEX)   \
	((reinterpret_cast<const unsigned char*>(DATA))[(INDEX) >> 3] & (1 << (7 - ((INDEX) & 7))))

/*!
** \brief Set the Nth bit in a raw char* buffer
** \ingroup Bit
*/
# define YUNI_BIT_SET(DATA,INDEX)  \
	((reinterpret_cast<unsigned char*>(DATA))[(INDEX) >> 3] |= static_cast<unsigned char>(1 << (7 - ((INDEX) & 7))))

/*!
** \brief Unset the Nth bit in a raw char* buffer
** \ingroup Bit
*/
# define YUNI_BIT_UNSET(DATA,INDEX)  \
	((reinterpret_cast<unsigned char*>(DATA))[(INDEX) >> 3] &= static_cast<unsigned char>(~(1 << (7 - ((INDEX) & 7)))))




namespace Yuni
{
/*!
** \brief Bit manipulation
** \ingroup Bit
*/
namespace Bit
{

	/*!
	** \brief Get the number of bits that are set
	** \ingroup Bit
	*/
	template<class T> uint Count(T data);
	yuint32 Count(yuint32 data);

	/*!
	** \brief Get if the bit at a given index
	** \ingroup Bit
	*/
	bool Get(const char*, uint index);

	/*!
	** \brief Set the Nth bit in a raw buffer
	** \ingroup Bit
	*/
	void Set(char* data, uint index);

	/*!
	** \brief Unset the Nth bit in a raw buffer
	** \ingroup Bit
	*/
	void Unset(char* data, uint index);





} // namespace Bit
} // namespace Yuni

#include "bit.hxx"

