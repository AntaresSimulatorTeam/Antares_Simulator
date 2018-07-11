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

#include "policy.h"
#include <map>
#include <yuni/io/file.h>
#include "appdata.h"
#include "../logs.h"
#include "../inifile/inifile.h"
#include "hostname.hxx"
#include "../config.h"
#include <yuni/core/system/environment.h>
#include <yuni/core/system/process.h>
#include <yuni/core/system/username.h>
#include <yuni/core/system/cpu.h>
#include <yuni/core/system/memory.h>



using namespace Yuni;
using namespace Antares;

#define SEP IO::Separator


namespace LocalPolicy
{


	namespace // anonymous
	{

		static bool localPoliciesOpened = false;

		typedef std::map<PolicyKey, String> PolicyMap;

		//! All entries
		static PolicyMap*  entries = nullptr;

		//! Mutex for policy entries
		static Mutex gsMutex;




		template<class StringT>
		static void OpenFromINIFileWL(const String& filename, const StringT& hostname)
		{
			IniFile ini;
			if (not ini.open(filename))
				return;

			PolicyKey key;
			ShortString128 hostnameVersion;
			ShortString128 hostnameAll;
			hostnameVersion << hostname << ':' << ANTARES_VERSION;
			hostnameAll     << hostname << ":*";

			ini.each([&] (const IniFile::Section& section)
			{
				// This section is dedicated to another host
				if (section.name == "*:*" or section.name == "*:" ANTARES_VERSION
					or section.name == hostnameAll
					or section.name == hostnameVersion)
				{
					section.each([&] (const IniFile::Property& property)
					{
						key = property.key;
						key.trim(" \t");
						(*entries)[key] = property.value;
					});
				}
			});
		}


		class PredicateEnv final
		{
		public:
			template<class StringT1, class StringT2>
			void operator () (StringT1& out, const StringT2& value) const
			{
				PolicyMap::const_iterator i = entries->find(value);
				if (i != entries->end())
					out += i->second;
				else
				{
					const ShortString128 zeroT = value;
					System::Environment::Read(zeroT, out, false);
				}
			}
		};


		template<class PredicateT, class StringT1, class StringT2>
		static inline uint ExpandWL(StringT1& out, const StringT2& string, PredicateT& predicate)
		{
			// checks
			assert(&out != &string and "undefined behavior");

			// Try to find the first occurence of an expression
			String::Size p = string.find("${", 0);
			if (p == String::npos)
				return 0;

			// Ok we have found something, let's go deeper...
			uint count = 0;
			String::Size offset = 0;
			out.clear();
			AnyString expr;
			do
			{
				// Looking for the end of the expression
				String::Size end = string.find('}', p + 2);
				if (end == String::npos)
				{
					// misformed
					out.append(string.c_str() + offset, string.size() - offset);
					return count;
				}

				// Ok, we have a complete expression
				++count;
				// appending before the expression
				out.append(string.c_str() + offset, p - offset);
				// The expression
				expr.adapt(string.c_str() + p + 2, end - p - 2);
				if (not expr.empty())
					predicate(out, expr);

				// Try to find the next one
				offset = end + 1;
				p = string.find("${", offset);
			}
			while (p != String::npos);

			if (offset < string.size())
				out.append(string.c_str() + offset, string.size() - offset);
			return count;
		}


		static inline void ExpansionWL()
		{
			PredicateEnv predicate;
			String out;
			// The end of the container
			auto end = entries->end();
			for (auto i = entries->begin(); i != end; ++i)
			{
				String& value = i->second;
				if (ExpandWL(out, value, predicate))
					value = out;
			}
		}

	} // anonymous namespace







	bool Open(bool expandEntries)
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (localPoliciesOpened)
			return true;

		String path;
		path.reserve(512);

		// Good ! This section is what we need !
		delete entries;
		entries = new PolicyMap();

		ShortString256 hostname;
		InternalAppendHostname(hostname);

		String username;
		System::Username(username);

		String localsettings;
		PolicyKey key;

		String localAppData;
		if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
			localAppData.clear();
		String localAppDataAllUsers;
		if (not OperatingSystem::FindAntaresLocalAppData(localAppDataAllUsers, true))
			localAppDataAllUsers.clear();


		String pathLocalPolicy;
		pathLocalPolicy << localAppData << SEP << "localpolicy.ini";
		String pathLocalPolicyAllUsers;
		pathLocalPolicyAllUsers << localAppDataAllUsers << SEP << "localpolicy.ini";

		if (IO::File::Exists(pathLocalPolicy))
			OpenFromINIFileWL(pathLocalPolicy, hostname);
		if (IO::File::Exists(pathLocalPolicyAllUsers))
			OpenFromINIFileWL(pathLocalPolicyAllUsers, hostname);


		// informations about antares
		// version
		(*entries)[(key = "antares.product.version")] = ANTARES_VERSION_STR;
		(*entries)[(key = "antares.product.majorversion")] = ANTARES_VERSION;

		// Keeping the hostname
		(*entries)[(key = "hostname")] = hostname;
		// user
		(*entries)[(key = "user")] = username;
		(*entries)[(key = "userdata")] = localAppData;
		// Process ID
		(*entries)[(key = "pid")] = Yuni::ProcessID();

		// type of platform : Windows, Linux...
		(*entries)[(key = "sys.os")] = YUNI_OS_NAME;
		// how many cpus ?
		(*entries)[(key = "sys.cpu")] = System::CPU::Count();
		// how much memory ?
		(*entries)[(key = "sys.memory")].clear()
			<< (uint) Math::Round(((double) (System::Memory::Total() + 1) / 1024 / 1024 / 1024)) << " Go";

		// paths
		(*entries)[(key = "localpolicy.user.path")] = pathLocalPolicy;
		(*entries)[(key = "localpolicy.allusers.path")] = pathLocalPolicyAllUsers;

		// license
		(*entries)[(key = "license.user.path")].clear()
			<< localAppData << SEP << "antares-" << ANTARES_VERSION << ".hwb";
		(*entries)[(key = "license.allusers.path")].clear()
			<< localAppDataAllUsers << SEP << "antares-" << ANTARES_VERSION << ".hwb";

		if ((*entries).count((key = "license.servers")) == 0)
			(*entries)[key].clear();

		if (expandEntries)
			ExpansionWL();
		return true;
	}



	void Close()
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (localPoliciesOpened)
		{
			localPoliciesOpened = false;
			if (entries)
			{
				delete entries;
				entries = nullptr;
			}
		}
	}


	bool HasKey(const PolicyKey& key)
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		return (not key.empty() and entries
			and entries->find(key) != entries->end());
	}


	bool Read(String& out, const PolicyKey& key)
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (not key.empty() and entries)
		{
			PolicyMap::const_iterator i = entries->find(key);
			if (i != entries->end())
			{
				out = i->second;
				return true;
			}
		}
		out.clear();
		return false;
	}


	bool ReadAsBool(const PolicyKey& key, bool defval)
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (not key.empty() and entries)
		{
			const PolicyMap::const_iterator i = entries->find(key);
			if (i != entries->end())
			{
				bool v;
				if (i->second.to(v))
					return v;
			}
		}
		return defval;
	}



	void DumpToString(Clob& out)
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (!entries or entries->empty())
			return;

		// The end of the container
		const PolicyMap::const_iterator end = entries->end();

		// Find the longest key length
		uint maxlen = 0;
		{
			for (PolicyMap::const_iterator i = entries->begin(); i != end; ++i)
			{
				uint len = i->first.size();
				if (len > maxlen)
					maxlen = len;
			}
		}

		maxlen += 3; // spaces after the key
		PolicyKey row;

		for (PolicyMap::const_iterator i = entries->begin(); i != end; ++i)
		{
			row = i->first;
			row.resize(maxlen, " ");
			out.append("  ", 2);
			out << row << ": " << i->second << '\n';
		}
	}


	void DumpToStdOut()
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (!entries or entries->empty())
			return;

		// The end of the container
		const PolicyMap::const_iterator end = entries->end();

		// Find the longest key length
		uint maxlen = 0;
		{
			for (PolicyMap::const_iterator i = entries->begin(); i != end; ++i)
			{
				uint len = i->first.size();
				if (len > maxlen)
					maxlen = len;
			}
		}

		maxlen += 3; // spaces after the key
		PolicyKey row;

		for (PolicyMap::const_iterator i = entries->begin(); i != end; ++i)
		{
			row = i->first;
			row.resize(maxlen, " ");
			std::cout << "  " << row << ": " << i->second << '\n';
		}
	}


	void DumpToLogs()
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (not entries or entries->empty())
		{
			logs.info() << "using the default local policy";
			return;
		}

		// The end of the container
		auto end = entries->end();
		for (auto i = entries->begin(); i != end; ++i)
		{
			//if (!i->first.startsWith("product_"))
			logs.info() << "  policy: " << i->first << ":  " << i->second;
		}
	}


	void CheckRootPrefix(const char* argv0)
	{
		// avoid concurrent changes
		MutexLocker locker(gsMutex);

		if (!entries)
			return;
		auto i = entries->find("force_root_prefix");
		if (i != entries->end() and not i->second.empty())
		{
			AnyString adapter = argv0;
			if (IO::IsAbsolute(adapter))
			{
				if (0 != adapter.ifind(i->second))
					exit(EXIT_FAILURE);
			}
			else
			{
				String absfilename;
				IO::MakeAbsolute(absfilename, adapter);
				if (0 != absfilename.ifind(i->second))
					exit(EXIT_FAILURE);
			}
		}
	}




} // namespace LocalPolicy
