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
#include <algorithm> // std::swap



namespace Yuni
{
namespace System
{

/*!
** \brief Endianness
** \ingroup Endianness
*/
namespace Endianness
{



	/*!
	** \brief Information about the CPU byte order
	** \ingroup Endianness
	*/
	enum ByteOrder
	{
		/*! Little-endian order */ LittleEndian = 0,
		/*! Big-endian order    */ BigEndian
	};



	/*!
	** \brief Determine the CPU byte order
	** \ingroup Endianness
	**
	** \return LittleEndian if the cpu seems to use the Little-endian order, BigEndian otherwise
	*/
	inline ByteOrder FindByteOrder()
	{
		static char big_endian_value_1[2] = { 0, 1 };
		return  ( (*((short*) big_endian_value_1)) == 1) ? BigEndian : LittleEndian;
	}



	/*!
	** \brief Swap bytes for a `N` bytes long variable
	** \ingroup Endianness
	**
	** \param[in,out] in A pointer to the variable to swap
	** \param size Size of the data
	*/
	inline void SwapBytes(char* in, const unsigned int size)
	{
		char* start = in;
		char* end = start + size - 1;
		while (start < end)
			std::swap(*start++,*end--);
	}


	/*!
	** \brief Swap bytes for a 2 bytes long variable
	** \ingroup Endianness
	**
	** \param[in,out] in The variable
	*/
	inline void SwapBytes2(char* in) { std::swap(in[0], in[1]); }


	/*!
	** \brief Swap bytes for a 4 bytes long variable
	** \ingroup Endianness
	**
	** \param[in,out] in The variable
	*/
	inline void SwapBytes4(char* in) { std::swap(in[0], in[3]); std::swap(in[1], in[2]); }


	/*!
	** \brief Swap bytes for a 8 bytes long variable
	** \ingroup Endianness
	**
	** \param[in,out] in The variable
	*/
	inline void SwapBytes8(char* in)
	{
		std::swap(in[0], in[7]);
		std::swap(in[1], in[6]);
		std::swap(in[2], in[5]);
		std::swap(in[3], in[4]);
	}


	/*!
	** \brief Swap bytes for a 16 bytes long variable
	** \ingroup Endianness
	**
	** \param[in,out] in The variable
	*/
	inline void SwapBytes16(char* in)
	{
		std::swap(in[0], in[15]);
		std::swap(in[1], in[14]);
		std::swap(in[2], in[13]);
		std::swap(in[3], in[12]);
		std::swap(in[4], in[11]);
		std::swap(in[5], in[10]);
		std::swap(in[6], in[9]);
		std::swap(in[7], in[8]);
	}






} // namespace Endian
} // namespace System
} // namespace Yuni

