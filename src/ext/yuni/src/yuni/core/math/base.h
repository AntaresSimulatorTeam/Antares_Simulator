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



namespace Yuni
{
namespace Math
{
namespace Base
{


	template<int Nth = 10>
	struct N final
	{
		//! Digits
		static const char* Digit() {return "0123456789abcdefghijklmnopqrstuvwxyz";}
		enum
		{
			//! the nth power of b
			n = Nth,
		};
	};


	typedef N<10>  Decimal;
	typedef N<8>   Octal;
	typedef N<16>  Hexa;



	struct HexaLowercase final
	{
		//! Digits
		static const char* Digit() {return "0123456789abcdef";}
		enum
		{
			//! the nth power of b
			n = 16,
		};
	};


	struct HexaUppercase final
	{
		//! Digits
		static const char* Digit() {return "0123456789ABCDEF";}
		enum
		{
			//! the nth power of b
			n = 16,
		};
	};





} // namespace Base
} // namespace Math
} // namespace Yuni
