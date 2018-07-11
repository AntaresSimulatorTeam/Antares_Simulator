/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_HXX__
# define __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_HXX__

# include <cassert>


namespace Antares
{

	template<class ChildT, template<class> class TP>
	inline IIntrusiveReference<ChildT,TP>::IIntrusiveReference() :
		pRefCount(0)
	{
	}


	template<class ChildT, template<class> class TP>
	inline IIntrusiveReference<ChildT,TP>::~IIntrusiveReference()
	{
	}


	template<class ChildT, template<class> class TP>
	inline IIntrusiveReference<ChildT,TP>::IIntrusiveReference(const IIntrusiveReference<ChildT,TP>& rhs) :
		pRefCount(0)
	{
	}


	template<class ChildT, template<class> class TP>
	inline IIntrusiveReference<ChildT,TP> &
	IIntrusiveReference<ChildT,TP>::operator = (const IIntrusiveReference& rhs) const
	{
		// Does nothing
		return *this;
	}


	template<class ChildT, template<class> class TP>
	inline void IIntrusiveReference<ChildT,TP>::onRelease()
	{
		static_cast<ChildT*>(this)->onRelease();
	}


	template<class ChildT, template<class> class TP>
	inline bool IIntrusiveReference<ChildT,TP>::unique() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		return pRefCount == 1;
	}


	template<class ChildT, template<class> class TP>
	inline void	IIntrusiveReference<ChildT,TP>::addRef() const
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		++pRefCount;
	}


	template<class ChildT, template<class> class TP>
	inline bool IIntrusiveReference<ChildT,TP>::release() const
	{
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			assert(pRefCount > 0);

			if (--pRefCount != 0)
				return false;
		}
		static_cast<ChildT*>(const_cast<IIntrusiveReference<ChildT,TP>*>(this))->onRelease();
		return true;
	}






} // namespace Antares

#endif	// __ANTARES_LIB_ASSEMBLY_INTRUSIVEREFERENCE_HXX__
