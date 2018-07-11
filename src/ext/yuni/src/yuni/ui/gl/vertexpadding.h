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
#ifndef __YUNI_GFX3D_VERTEX_PADDING_H__
# define __YUNI_GFX3D_VERTEX_PADDING_H__

# include <yuni/yuni.h>
# include <yuni/core/static/assert.h>

namespace Yuni
{
namespace Gfx3D
{


	/*!
	** \brief Pad with useless data to reach a multiple of 32 bytes
	**
	** Template specializations cater to all possible cases
	*/
	template<uint size>
	class VertexPadding
	{
	private:
		enum // anonymous
		{
			PaddingSize = (size < 32) ? 32 - size : (size < 64) ? 64 - size : 96 - size

		}; // enum anonymous

		YUNI_STATIC_ASSERT(size <= 96, InvalidSizeForVertexPadding);

	protected:
		VertexPadding()
		{
			for (uint i = 0; i < PaddingSize; ++i)
				pPadding[i] = '\0';
		}
		VertexPadding(const VertexPadding& other):
			pPadding(other.pPadding)
		{}

	private:
		char pPadding[PaddingSize];

	}; // class VertexPadding


	// Size is 32. No Padding
	template<>
	class VertexPadding<32u>
	{
	};


	// Size is 64. No Padding
	template<>
	class VertexPadding<64u>
	{
	};


	// Size is 96. No Padding
	template<>
	class VertexPadding<96u>
	{
	};


} // namespace Gfx3D
} // namespace Yuni

#endif // __YUNI_GFX3D_VERTEX_PADDING_H__
