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
#include "../yuni.h"
#include "object.h"



namespace Yuni
{
namespace Marshal
{

	/*!
	** \brief Document which may contain several objects
	*/
	class Document
	{
	public:
		Document();
		~Document();

	public:
		//! Root object
		Object root;

	}; // class Document




} // namespace Marshal
} // namespace Yuni
