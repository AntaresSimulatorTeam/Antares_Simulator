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
#include "../system/console.h"



namespace Yuni
{
namespace Logs
{
namespace Verbosity
{


	struct YUNI_DECL Unknown final
	{
		static const char* Name() {return "";}
		template<class U> static void AppendName(U&) {}
		enum
		{
			level = 0,
			shouldUsesStdCerr = 0,
			hasName = 0,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::none;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Unknown


	struct YUNI_DECL Quiet final
	{
		static const char* Name() {return "quiet";}
		template<class U> static void AppendName(U& u) {u << "[quiet]";}
		enum
		{
			level = 100000, // equivalent to infinite
			shouldUsesStdCerr = 1,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::none;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Quiet



	struct YUNI_DECL Fatal final
	{
		static const char* Name() {return "fatal";}
		template<class U> static void AppendName(U& u) {u << "[fatal]";}
		enum
		{
			level = 1000,
			shouldUsesStdCerr = 1,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::red;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Fatal



	struct YUNI_DECL Error final
	{
		static const char* Name() {return "error";}
		template<class U> static void AppendName(U& u) {u << "[error]";}
		enum
		{
			level = 2000,
			shouldUsesStdCerr = 1,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::red;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Error



	struct YUNI_DECL Warning final
	{
		static const char* Name() {return "warning";}
		template<class U> static void AppendName(U& u) {u << "[warns]";}
		enum
		{
			level = 3000,
			shouldUsesStdCerr = 1,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::yellow;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Warning



	struct YUNI_DECL Checkpoint final
	{
		static const char* Name() {return "checkpoint";}
		template<class U> static void AppendName(U& u) {u << "[check]";}

		enum
		{
			level = 4000,
			shouldUsesStdCerr = 0,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::white;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::white;

	}; // class Checkpoint



	struct YUNI_DECL Notice final
	{
		static const char* Name() {return "notice";}
		template<class U> static void AppendName(U& u) {u << "[notic]";}

		enum
		{
			level = 5000,
			shouldUsesStdCerr = 0,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::green;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Notice


	struct YUNI_DECL Progress final
	{
		static const char* Name() {return "progress";}
		template<class U> static void AppendName(U& u) {u << "[progress]";}

		enum
		{
			level = 6000,
			shouldUsesStdCerr = 0,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::none;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Progress



	struct YUNI_DECL Info final
	{
		static const char* Name() {return "info";}
		template<class U> static void AppendName(U& u) {u << "[infos]";}

		enum
		{
			level = 7000,
			shouldUsesStdCerr = 0,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::none;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Info



	struct YUNI_DECL Compatibility final
	{
		static const char* Name() {return "Compatibility";}
		template<class U> static void AppendName(U& u) {u << "[compatibility notice]";}

		enum
		{
			level = 8000,
			shouldUsesStdCerr = 0,
			hasName = 1,
			enabled = 1,
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::yellow;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Info



	struct YUNI_DECL Debug final
	{
		static const char* Name() {return "debug";}
		template<class U> static void AppendName(U& u) {u << "[debug]";}

		enum
		{
			level = 10000,
			shouldUsesStdCerr = 0,
			hasName = 1,
			# ifdef NDEBUG // The debug messages must be disabled
			enabled = 0,
			# else
			enabled = 1,
			# endif
		};
		//! Text Color for displaying the verbosity
		static const System::Console::Color color = System::Console::none;
		//! Message Text Color
		static const System::Console::Color messageColor = System::Console::none;

	}; // class Debug





} // namespace Verbosity
} // namespace Logs
} // namespace Yuni

