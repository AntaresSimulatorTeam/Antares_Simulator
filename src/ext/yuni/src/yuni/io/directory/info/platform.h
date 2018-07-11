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
#include "../../../core/string.h"


namespace Yuni
{
namespace Private
{
namespace IO
{
namespace Directory
{

	//! Platform-dependant data implementation
	class IteratorData;



	IteratorData* IteratorDataCreate(const AnyString& folder, uint flags);

	IteratorData* IteratorDataCopy(const IteratorData*);

	void IteratorDataFree(const IteratorData*);

	bool IteratorDataValid(const IteratorData*);

	IteratorData* IteratorDataNext(IteratorData*);

	const String& IteratorDataFilename(const IteratorData*);

	const String& IteratorDataParentName(const IteratorData*);

	const String& IteratorDataName(const IteratorData*);

	uint64 IteratorDataSize(const IteratorData*);

	sint64 IteratorDataModified(const IteratorData*);

	bool IteratorDataIsFolder(const IteratorData*);

	bool IteratorDataIsFile(const IteratorData*);




} // namespace Directory
} // namespace IO
} // namespace Private
} // namespace Yuni
