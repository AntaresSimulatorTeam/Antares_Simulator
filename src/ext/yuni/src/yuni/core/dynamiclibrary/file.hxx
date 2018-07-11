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
#include "file.h"



namespace Yuni
{
namespace DynamicLibrary
{

	inline bool File::loaded() const
	{
		return (pHandle != NullHandle);
	}


	inline const String& File::filename() const
	{
		return pFilename;
	}


	inline File::Handle File::handle() const
	{
		return pHandle;
	}


	inline Symbol File::operator [] (const AnyString& name) const
	{
		return resolve(name);
	}




} // namespace DyanmicLibrary
} // namespace Yuni
