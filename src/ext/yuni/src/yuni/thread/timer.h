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
#include "../core/atomic/bool.h"
#include "thread.h"



namespace Yuni
{
namespace Thread
{

	/*!
	** \brief A thread that executes a method every X milliseconds
	**
	** \ingroup Threads
	**
	**
	** \code
	** using namespace Yuni;
	**
	** class MyTimer : public Thread::Timer
	** {
	** public:
	**	 Timer() {}
	**	 virtual ~Timer() {stop();} // Absolutely needed
	**
	** protected:
	**	 virtual bool onInterval();
	** };
	**
	** bool Timer::onInterval(uint cycle)
	** {
	**	std::cout << cycle << std::endl;
	**	return true;
	** }
	**
	** int main(void)
	** {
	**	 MyTimer timer;
	**	 timer.cycleCount(5);
	**	 timer.interval(1000); // 1 seconds
	**
	**	 // Run the timer
	**	 timer.start();
	**
	**	 // Waiting for max. 10 seconds
	**	 timer.stop(10000);
	**	 return 0;
	** }
	** \endcode
	*/
	class YUNI_DECL Timer : public IThread
	{
	public:
		//! The Threading policy
		typedef IThread::ThreadingPolicy  ThreadingPolicy;
		enum
		{
			//! Special value to use for an infinite loop
			infinite = (uint) -1,
			//! The default time interval (in milliseconds)
			defaultInterval = 1000,
		};
		//! The most suitable smart pointer for the class
		typedef IThread::Ptr::Promote<Timer>::Ptr Ptr;

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		**
		** By default, the constant `defaultInterval` will be used for the time
		** interval with an infinite number of cycles.
		*/
		Timer();
		/*!
		** \brief Constructor with a time interval
		**
		** An infinite number of cycles will be used by default.
		*/
		explicit Timer(uint interval);
		/*!
		** \brief Constructor with a time interval + cycle count
		**
		** \param interval The new time interval (in milliseconds)
		** \param cycles The number of cycles [0 .. n[
		*/
		Timer(uint interval, uint cycles);
		/*!
		** \brief Copy constructor
		**
		** The given timer will be locked to copy its settings.
		*/
		Timer(const Timer& rhs);
		/*!
		** \brief Destructor
		*/
		virtual ~Timer();
		//@}


		//! \name Settings (Time interval & Cycles)
		//@{
		/*!
		** \brief Reload the settings
		**
		** The timer will reload its settings (time interval and number of cycles)
		** if it is already started. It has not effect otherwise.
		** Prefer to use this method instead of restarting the timer.
		*/
		void reload();

		/*!
		** \brief Modify then Reload the settings
		**
		** Modify the settings (time interval only) with a single lock
		** and ask to reload them if the timer is started.
		** This method is a convenient and faster replacement for the following code:
		** (assuming we want one second for the time interval)
		** \code
		** timer.interval(1000); // 1 second
		** timer.reload();
		** \endcode
		**
		** \param milliseconds The new time interval (in milliseconds)
		*/
		void reload(uint milliseconds);

		/*!
		** \brief Modify then Reload the settings
		**
		** Modify the settings (time interval and number of cycles) with a single lock
		** and ask to reload them if the timer is started.
		** This method is a convenient and faster replacement for the following code:
		** (assuming we want one second for the time interval and three cycles)
		**
		** \code
		** timer.interval(1000); // 1 second
		** timer.cycleCount(3);
		** timer.reload();
		** \endcode
		**
		** \param milliseconds The new time interval (in milliseconds)
		** \param cycles The number of cycles [0 .. n[
		*/
		void reload(uint milliseconds, uint cycles);

		/*!
		** \brief Get the time interval (in milliseconds)
		*/
		uint interval() const;

		/*!
		** \brief Set the time interval (in `ms`)
		**
		** Use `reload()` to apply the changes if the timer is already started.
		** \param milliseconds The new time interval (in milliseconds)
		*/
		void interval(uint milliseconds);

		/*!
		** \brief Get the number of cycle to run
		** \see Timer::infinite
		*/
		uint cycleCount() const;

		/*!
		** \brief Set the number of cycle to run
		**
		** Use `reload()` to apply the changes if the timer is already started.
		** \param n The number of cycles [0 .. n[
		** \see Timer::infinite
		*/
		void cycleCount(uint n = infinite);
		//@}


		//! \name Operators
		//@{
		/*!
		** \brief The assignment operator (=)
		**
		** Both the given timer and this object will be locked to copy
		** the settings.
		** Use `reload()` to apply the changes if the timer is already started.
		*/
		Timer& operator = (const Timer& rhs);
		//@}


	protected:
		/*!
		** \brief Event: Execute a new cycle
		**
		** \note There is no real need to use suspend() in this method, it is already done
		** for you.
		**
		** \param cycle The current cycle number. Always 0 in the case of an infinite timer
		** \return True to continue the execution of the timer, False otherwise
		*/
		virtual bool onInterval(uint cycle) = 0;


	private:
		/*!
		** \brief Event: Execute a new cycle
		**
		** \note There is no real need to use suspend() in this method, it will already be
		** done for you.
		**
		** \return True to stop and to destroy the thread
		*/
		virtual bool onExecute() final;

		/*!
		** \brief Run an infinite loop
		** \return True to stop the loop, False to reload the settings
		*/
		bool internalRunInfiniteLoop();
		/*!
		** \brief Run a fixed number of cycles
		** \return True to stop the loop, False to reload the settings
		*/
		bool internalRunFixedNumberOfCycles();

	private:
		//! The time interval
		volatile uint pTimeInterval;
		//! The maximum number of cycles before stopping, 0 means infinite
		volatile uint pCycleCount;
		//! A bool value to indicate if the settings should be reloaded
		Atomic::Bool pShouldReload;
		//! Mutex
		mutable Mutex pTimerMutex;

	}; // class Thread::Timer






} // namespace Thread
} // namespace Yuni

# include "timer.hxx"

