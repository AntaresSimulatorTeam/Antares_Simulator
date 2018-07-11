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
namespace UI
{


	//! Multisampling is a full screen anti-aliasing algorithm
	class MultiSampling
	{
	public:
		//! Available types
		enum Type
		{
			//! No multi-sampling (1x)
			msNone,

			//! 2x multi-sampling
			ms2x,

			//! 4x multi-sampling
			ms4x,

			//! 8x multi-sampling
			ms8x,

			//! 16x multi-sampling
			ms16x

		}; // enum Type


		//! Get the multiplier for a given sampling type
		static uint Multiplier(Type type)
		{
			switch (type)
			{
				case msNone:
					return 1;
				case ms2x:
					return 2;
				case ms4x:
					return 4;
				case ms8x:
					return 8;
				case ms16x:
					return 16;
			}
			return 1;
		}

		//! Get the sampling type from a given multiplier
		static Type TypeFromMultiplier(uint multiplier)
		{
			switch (multiplier)
			{
				case 2:
					return ms2x;
				case 4:
					return ms4x;
				case 8:
					return ms8x;
				case 16:
					return ms16x;
			}
			return msNone;
		}

	}; // class MultiSampling



} // namespace UI
} // namespace Yuni
