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

	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::utf8iterator
	CString<ChunkSizeT,ExpandableT>::utf8begin(uint offset)
	{
		return utf8iterator(*this, offset);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::const_utf8iterator
	CString<ChunkSizeT,ExpandableT>::utf8begin(uint offset) const
	{
		return const_utf8iterator(*this, offset);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::null_iterator
	CString<ChunkSizeT,ExpandableT>::utf8end()
	{
		return null_iterator(*this);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::null_iterator
	CString<ChunkSizeT,ExpandableT>::utf8end() const
	{
		return null_iterator(*this);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::null_iterator
	CString<ChunkSizeT,ExpandableT>::end()
	{
		return null_iterator(*this);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::null_iterator
	CString<ChunkSizeT,ExpandableT>::end() const
	{
		return null_iterator(*this);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::iterator
	CString<ChunkSizeT,ExpandableT>::begin()
	{
		return iterator(*this, 0u);
	}


	template<uint ChunkSizeT, bool ExpandableT>
	inline typename CString<ChunkSizeT,ExpandableT>::const_iterator
	CString<ChunkSizeT,ExpandableT>::begin() const
	{
		return const_iterator(*this, 0u);
	}




} // namespace Yuni
