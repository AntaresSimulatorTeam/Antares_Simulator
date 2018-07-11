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
#include "../string.h"



namespace Yuni
{
namespace Core
{
namespace Utils
{

	/*!
	** \brief A simple hexadecimal buffer dumper.
	**
	** This dumper can dump any buffer (with start address and length),
	** or any class having a data() and size() member function.
	** (for example, String).
	** The output looks like the following:
	**
	** Sample usage:
	** \code
	** // Either call it with a memory buffer
	** std::cout << Core::Utils::Hexdump(myBuffer);
	**
	** // Or with a plain old char buffer.
	** char buf[256];
	** std::cout << Core::Utils::Hexdump(buf, sizeof(buf));
	** \endcode
	**
	** Sample output:
	** \code
	** 0x00604010: 0001 0203 0405 0607 0809 0a0b 0c0d 0e0f |................|
	** 0x00604020: 1011 1213 1415 1617 1819 1a1b 1c1d 1e1f |................|
	** 0x00604030: 2021 2223 2425 2627 2829 2a2b 2c2d 2e2f | !"#$%&'()*+,-./|
	** 0x00604040: 3031 3233 3435 3637 3839 3a3b 3c3d 3e3f |0123456789:;<=>?|
	** 0x00604050: 4041 4243 4445 4647 4849 4a4b 4c4d 4e4f |@ABCDEFGHIJKLMNO|
	** \endcode
	*/
	class YUNI_DECL Hexdump final
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Construct from a simple buffer.
		**
		** \param[in] buffer A pointer to a raw buffer.
		** \param[in] size The size to dump.
		*/
		Hexdump(const char* buffer, uint size);

		/*!
		** \brief Copy ctor.
		**
		** \param[in] rhs The original Hexdump instance
		*/
		Hexdump(const Hexdump& rhs);

		/*!
		** \brief Construct from a string (or like) object.
		**
		** \param[in] buffer The stream to output the hexdump on.
		*/
		template<class U> Hexdump(const U& buffer);
		//@}


		//! \name Dump functions
		//@{
		/*!
		** \brief Dumps the current buffer to a stream.
		** \param[in,out] outStream The buffer will dumped to this stream.
		*/
		template<class U> void dump(U& outStream) const;

		/*!
		** \brief Dumps the current buffer to a string.
		** \return The dump contents.
		*/
		String dump() const;
		//@}

		//! \name Operators
		//@{
		/*!
		** \brief Operator =
		** \param[in] rhs The source Hexdump instance
		*/
		Hexdump& operator = (const Hexdump& rhs);
		//@}


	private:
		/*!
		** \brief Dumps the hexadecimal-version of a sub-buffer into a string
		**
		** \param[in] line The string to append to.
		** \param[in] buffer The start address of the sub-buffer.
		** \param[in] size The size of the sub-buffer
		*/
		void dumpHexadecimal(String& line, const char* buffer, uint size) const;

		/*!
		** \brief Dumps the printable-version of a sub-buffer into a string
		**
		** \param[in] line The string to append to.
		** \param[in] buffer The start address of the sub-buffer.
		** \param[in] size The size of the sub-buffer
		*/
		void dumpPrintable(String& line, const char* buffer, uint size) const;

	private:
		//! Pointer to the current buffer (not owned by Hexdump)
		char const* pBuffer;
		//! The current dump size.
		uint pSize;

	}; // class HexDump






} // namespace Utils
} // namespace Core
} // namespace Yuni



//! Operator overloads for printing
//@{
/*!
** \brief std::ostring print operator
** \param[in] outStream The stream to output the hexdump on.
** \param[in] hexDumper The Hexdump instance to dump.
*/
std::ostream& operator<< (std::ostream& outStream, const Yuni::Core::Utils::Hexdump& hexDumper);
//@}


#include "hexdump.hxx"
