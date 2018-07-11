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
#include "../../../yuni.h"
#include "../../../core/string.h"
#include "../resolution.h"
#include "../monitor.h"

//! Name to use for the fail-safe device
#define YUNI_DEVICE_DISPLAY_LIST_FAIL_SAFE_NAME   "Fail-safe Device"



namespace Yuni
{
namespace Device
{
namespace Display
{

	/*!
	** \brief Informations about all available display
	**
	** The best way to enumerate all available resolutions of all monitors :
	** \code
	** int main()
	** {
	**	typedef Device::Display::List MonitorIterator;
	**	Device::Display::List monitors;
	**	monitors.refresh();
	**	for (MonitorIterator i = monitors.begin(); i != monitors.end(); ++i)
	**	{
	**		std::cout << " * Monitor : " << (*i)->name() << std::endl;
	**		// All resolutions for this monitor
	**		for (Display::Resolution::Vector::iterator r = (*i)->resolutions().begin();
	**			r != (*i)->resolutions().end(); ++r)
	**		{
	**			std::cout << "    . " << *r << std::endl;
	**		}
	**	}
	** }
	** \endcode
	** \warning This class is not thread-safe
	*/
	class List final
	{
	private:
		//! Vector of Monitor
		typedef std::vector<Monitor::Ptr> MonitorVector;

	public:
		//! An interator
		typedef MonitorVector::iterator iterator;
		//! A const iterator
		typedef MonitorVector::const_iterator const_iterator;

	public:
		//! \name Constructors &* Destructor
		//@{
		//! Default constructor
		List();
		//! Copy constructor (the copy will share the same informations)
		List(const List& c);
		//! Destructor
		~List() {}
		//@}

		//! \name Refresh
		//@{
		/*!
		** \brief Refresh informations about the monitors
		**
		** In any cases, this method ensures to provide a valid primary display,
		** and an not empty list even if monitors could not be found. (it would
		** provide a default monitor with default resolutions in this case)
		**
		** \param minWidth The minimum acceptable width
		** \param minHeight The minimum acceptable height
		** \param minDepth The minimum acceptable depth
		** \return True if the operation succeeded
		*/
		bool refresh(uint32 minWidth = 640, uint32 minHeight = 480, uint8 minDepth = 8);
		//@}

		//! \name Primary display
		//@{
		/*!
		** \brief Get the primary display
		**
		** The result is guaranteed to be valid for use.
		*/
		Monitor::Ptr primary() const;
		//@}

		//! \name Searching
		//@{
		//! Get an iterator at the beginning of the list
		iterator begin();
		//! Get an iterator at the beginning of the list
		const_iterator begin() const;

		//! Get an iterator at the end of the list
		iterator end();
		//! Get an iterator at the end of the list
		const_iterator end() const;

		/*!
		** \brief Find a monitor by its handle
		**
		** \return A reference to a monitor. Use the method `valid()` to know
		** if the monitor has been found
		**
		** \param hwn The handle to find
		**
		** \see Monitor::valid()
		*/
		Monitor::Ptr findByHandle(const Monitor::Handle hwn) const;

		/*!
		** \brief Find a monitor by its guid
		**
		** \return A reference to a monitor. Use the method `valid()` to know
		** if the monitor has been found
		**
		** \param guid The guid to find
		**
		** \see Monitor::valid()
		*/
		Monitor::Ptr findByGUID(const String& guid) const;
		//@}


		//! Get the number of monitors
		size_t size() const;

		//! \name Operators
		//@{
		//! Operator `=` (the copy will share the same information)
		List& operator = (const List& rhs);
		//@}

	private:
		//! All available monitors
		MonitorVector  pMonitors;
		//! The primary display
		Monitor::Ptr pPrimary;
		//! A null monitor
		Monitor::Ptr pNullMonitor;

	}; // class List





} // namespace Display
} // namespace Device
} // namespace Yuni


/*!
** \brief The hard limit for monitor count
**
** \internal This define is especially usefull where a limited is required
** by the operating system
*/
#define YUNI_DEVICE_MONITOR_COUNT_HARD_LIMIT   64


#include "list.hxx"
