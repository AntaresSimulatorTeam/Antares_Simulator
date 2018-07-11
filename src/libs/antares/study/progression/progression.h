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
#ifndef __ANTARES_LIBS_SOLVER_SIMULATION_PROGRESSION_H__
# define __ANTARES_LIBS_SOLVER_SIMULATION_PROGRESSION_H__

# include <yuni/yuni.h>
# include <yuni/core/singleton.h>
# include <yuni/thread/timer.h>
# include <map>
# include <list>
# include <vector>
# include "../fwd.h"
#include <yuni/io/file.h>


namespace Antares
{
namespace Solver
{


	/*!
	** \brief Progress meter about any operation performed on the attached study
	*/
	class Progression final
	{
	public:
		enum Section
		{
			sectYear = 0,
			sectOutput,
			sectTSGLoad,
			sectTSGSolar,
			sectTSGWind,
			sectTSGHydro,
			sectTSGThermal,
			sectImportTS,
			sectMax
		};
		enum
		{
			npos = (uint) -1,
		};

		const char* SectionToCStr(Section section);

	private:
		class Part final
		{
		public:
			typedef std::map<Section, Part> MapPerSection;
			typedef std::map<uint, MapPerSection> Map;
			//typedef Yuni::LinkedList<Part*> ListRef;
			typedef std::list<Part*> ListRef;

		public:
			//! The total number of ticks to achieve
			int maxTickCount;
			//! The current number of ticks
			Yuni::Atomic::Int<32> tickCount;
			//! The last number of ticks, to reduce the log verbosity
			int lastTickCount;
			// Caption to use when displaying logs
			// Example: 'year: 10000, task: thermal'
			Yuni::CString<40, false> caption;
		};

	public:
		class Task final
		{
		public:
			Task(const Antares::Data::Study& study, Section section);
			Task(const Antares::Data::Study& study, uint year, Section section);

			~Task()
			{
				pProgression.end(pPart);
			}

			Task& operator ++ ()
			{
				++pPart.tickCount;
				return *this;
			}

			Task& operator += (int value)
			{
				pPart.tickCount += value;
				return *this;
			}

		private:
			Progression& pProgression;
			Part& pPart;
		};

	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \name Default constructor
		*/
		Progression();
		/*!
		** \brief Destructor
		*/
		~Progression();
		//@}

		/*!
		** \brief Declare a new part of the progression
		**
		** \param year The associated year
		** \param section The section in this year
		** \param nbTicks The total number of ticks to achieve
		**
		** \internal The number of ticks should remain an `int` because
		**   we can not use unsigned atomic integer
		*/
		void add(uint year, Section section, int nbTicks);

		void add(Section section, int nbTicks);

		bool saveToFile(const Yuni::String& filename);

		void setNumberOfParallelYears(uint nb);

		//! \name Thread management
		//@{
		/*!
		** \brief Start the thread dedicated to the progress notification
		*/
		void start();

		/*!
		** \brief Stop the thread dedicated to the progress notification
		*/
		void stop();
		//@}


	protected:
		Part& begin(uint year, Section section);
		void end(Part& part);

	private:
		class Meter final : public Yuni::Thread::Timer
		{
		public:
			Meter();
			virtual ~Meter()
			{
				if(logsContainer)
					delete[] logsContainer;
			}

			void allocateLogsContainer(uint nb);

			/*!
			** \brief Prepare enough space to allow @n simultaneous tasks
			*/
			void taskCount(uint n);

		protected:
			virtual bool onInterval(uint) override;

		public:
			//
			Progression::Part::Map parts;
			Part::ListRef inUse;
			Yuni::Mutex mutex;
			uint nbParallelYears;

			// Because writing something to the logs might be expensive, we have to
			// reduce the time spent in locking the mutex.
			// We will use a temp vector of string to delay the writing into the logs
			Yuni::CString<256,false> * logsContainer;

		}; // class Meter

	private:
		//! A dedicated thread for displaying the progression
		Meter pProgressMeter;
		//! Flag to indicate if the progress meter has been started
		// This variable has no need to be thread-safe because initialized at the very beginning
		bool pStarted;
		// Friend
		friend class Task;

	}; // class Progression





} // namespace Solver
} // namespace Antares

# include "progression.hxx"

#endif // __ANTARES_LIBS_SOLVER_SIMULATION_PROGRESSION_H__
