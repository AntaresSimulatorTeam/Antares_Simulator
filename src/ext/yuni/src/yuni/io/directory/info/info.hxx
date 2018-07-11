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
#include "info.h"



namespace Yuni
{
namespace IO
{
namespace Directory
{


	inline Info::Info(const AnyString& directory)
		: pDirectory(directory)
	{}


	inline bool Info::exists() const
	{
		return IO::Directory::Exists(pDirectory);
	}


	template<class StringT>
	inline void Info::normalize(StringT& tmp)
	{
		tmp = pDirectory;
		IO::Normalize(pDirectory, tmp);
	}

	inline Info::iterator Info::begin() const
	{
		return iterator(pDirectory);
	}


	inline Info::file_iterator Info::file_begin() const
	{
		return file_iterator(pDirectory);
	}

	inline Info::folder_iterator Info::folder_begin() const
	{
		return folder_iterator(pDirectory);
	}

	inline Info::recursive_iterator Info::recursive_begin() const
	{
		return recursive_iterator(pDirectory);
	}


	inline Info::recursive_file_iterator Info::recursive_file_begin() const
	{
		return recursive_file_iterator(pDirectory);
	}

	inline Info::recursive_folder_iterator Info::recursive_folder_begin() const
	{
		return recursive_folder_iterator(pDirectory);
	}




	inline Info::null_iterator Info::end() const
	{
		return null_iterator();
	}


	inline Info::null_iterator Info::recursive_end() const
	{
		return null_iterator();
	}


	inline Info::null_iterator Info::file_end() const
	{
		return null_iterator();
	}


	inline Info::null_iterator Info::folder_end() const
	{
		return null_iterator();
	}


	inline Info::null_iterator Info::recursive_file_end() const
	{
		return null_iterator();
	}


	inline Info::null_iterator Info::recursive_folder_end() const
	{
		return null_iterator();
	}


	inline String& Info::directory()
	{
		return pDirectory;
	}

	inline const String& Info::directory() const
	{
		return pDirectory;
	}


	inline bool Info::create(uint mode) const
	{
		return IO::Directory::Create(pDirectory, mode);
	}


	inline bool Info::remove() const
	{
		return IO::Directory::Remove(pDirectory);
	}


	inline bool Info::copy(const AnyString& destination) const
	{
		return IO::Directory::Copy(pDirectory, destination);
	}


	inline bool Info::setAsCurrentDirectory() const
	{
		return IO::Directory::Current::Set(pDirectory);
	}


	inline Info& Info::operator = (const Info& rhs)
	{
		pDirectory = rhs.pDirectory;
		return *this;
	}

	inline Info& Info::operator = (const AnyString& rhs)
	{
		pDirectory = rhs;
		return *this;
	}


	inline bool Info::operator == (const Info& rhs) const
	{
		return pDirectory == rhs.pDirectory;
	}


	template<class U>
	inline bool Info::operator == (const U& rhs) const
	{
		return pDirectory == rhs;
	}


	template<class U>
	inline Info& Info::operator += (const U& rhs)
	{
		pDirectory += rhs;
		return *this;
	}


	template<class U>
	inline Info& Info::operator << (const U& rhs)
	{
		pDirectory << rhs;
		return *this;
	}





} // namespace Directory
} // namespace IO
} // namespace Yuni

