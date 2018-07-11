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
#include "distribution.h"



namespace Yuni
{
namespace Math
{
namespace Random
{



	/*!
	** \brief Constant random number generator
	**
	** Provides always the same value, given through the constructor
	*/
	template<typename T>
	class YUNI_DECL Constant final : public Distribution<T, Constant>
	{
	public:
		// Name of the distribution
		static const char* Name() {return "Constant";}

	public:
		Constant(const T v) :pValue(v) {}
		~Constant() {}

		void reset() {}

		const Value next() const {return pValue;}

	private:
		//! The constant value
		const T pValue;

	}; // class Constant




} // namespace Random
} // namespace Math
} // namespace Yuni

#include "default.hxx"
