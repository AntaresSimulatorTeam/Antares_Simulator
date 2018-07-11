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

	//! Variant internal data type
	enum VariantInnerType
	{
		//! Nil / Not assigned
		variantTNil,
		//! Bool
		variantTBool,
		//! char
		variantTChar,
		//! sint32
		variantTInt32,
		//! sint64
		variantTInt64,
		//! uint32
		variantTUInt32,
		//! uint64
		variantTUInt64,
		//! string
		variantTString,
		//! struct, with members
		variantTClass,
		//! array of variants
		variantTArray
	};


} // namespace Yuni
