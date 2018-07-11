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
#include "versions.h"
#include <yuni/io/directory.h>
#include <yuni/io/file.h>
#include <iostream>


#define SEP IO::Separator


namespace Yuni
{
namespace LibConfig
{
namespace VersionInfo
{


	template<class StringT>
	static const String& QuotePath(const StringT& value)
	{
		static String s;
		# ifndef YUNI_OS_WINDOWS
		s = value;
		s.replace(" ", "\\ ");
		# else
		s.clear();
		s << '"' << value << '"';
		# endif
		return s;
	}





	void List::checkRootFolder(const String& root)
	{
		String yuniMarker;
		yuniMarker << root << SEP << "mark-for-yuni-sources";

		if (IO::File::Exists(yuniMarker))
		{
			if (pOptDebug)
				std::cout << "[yuni-config][debug] found special yuni marker `" << yuniMarker << "`" << std::endl;

			# ifdef YUNI_OS_WINDOWS
			loadFromPath(root + "\\..\\..\\..");
			# else
			loadFromPath(root + "/../../..");
			# endif
			return;
		}

		# ifdef YUNI_OS_MSVC //Visual Studio
		// For dealing with the paths like '{Debug,Release}/yuni-config.exe'
		if (IO::File::Exists(String() << root << "\\..\\mark-for-yuni-sources"))
			loadFromPath(root + "\\..\\..\\..\\..");
		# endif
	}


	void List::findFromPrefixes(const String::List& prefix)
	{
		if (not prefix.empty())
		{
			const String::List::const_iterator end = prefix.end();
			for (String::List::const_iterator i = prefix.begin(); i != end; ++i)
				loadFromPath(*i);
		}
	}


	void List::loadFromPath(const String& folder)
	{
		String path;
		IO::Canonicalize(path, folder);
		if (pOptDebug)
			std::cout << "[yuni-config][debug] :: reading `" << path << "`" << std::endl;

		VersionInfo::Settings info;
		info.mapping = mappingStandard;

		String s;
		s << path << SEP << "yuni.version";
		if (not IO::File::Exists(s))
		{
			s.clear() << path << SEP << "include" << SEP << "yuni" << SEP << "yuni.version";
			if (not IO::File::Exists(s))
			{
				info.mapping = mappingSVNSources;
				s.clear() << path << SEP << "src" << SEP << "yuni" << SEP << "yuni.version";
				if (not IO::File::Exists(s))
                {
					if (pOptDebug)
						std::cout << "[yuni-config][debug] :: " << s << " not found" << std::endl;
					return;
                }
			}
		}

		IO::File::Stream file;
		if (file.open(s))
		{
			String key;
			String value;

			Version version;

			// A buffer. The given capacity will be the maximum length for a single line
			Clob buffer;
			buffer.reserve(8000);
			while (file.readline(buffer))
			{
				buffer.extractKeyValue(key, value);

				if (key.empty() || key == "[")
					continue;
				if (key == "version.hi")
					version.hi = value.to<unsigned int>();
				if (key == "version.lo")
					version.lo = value.to<unsigned int>();
				if (key == "version.rev")
					version.revision = value.to<unsigned int>();
				if (key == "version.target")
					info.compilationMode = value;
				if (key == "modules.available")
					value.split(info.modules, ";\"', \t", false);
				if (key == "support.opengl")
					info.supportOpenGL = value.to<bool>();
				if (key == "support.directx")
					info.supportDirectX = value.to<bool>();
				if (key == "redirect")
					loadFromPath(value);
				if (key == "path.include")
				{
					if (not value.empty())
						info.includePath.push_back(value);
				}
				if (key == "path.lib")
				{
					if (not value.empty())
						info.libPath.push_back(value);
				}
			}

			if (not version.null() and not info.modules.empty())
			{
				info.path = path;
				info.compiler = pCompiler;
				pList[version] = info;

				if (pOptDebug)
				{
					std::cout << "[yuni-config][debug]  - found installation `" << path
						<< "` (" << version << ")" << std::endl;
				}
			}
			else
			{
				std::cerr << "error: " << s << ": invalid file";
				if (version.null())
					std::cerr << " (invalid version)" << std::endl;
				else if (info.modules.empty())
					std::cerr << " (no module)" << std::endl;
			}
		}
	}


	void List::compiler(const String& c)
	{
		pCompiler = c;
	}


	void List::print()
	{
		if (!pList.empty())
		{
			const const_iterator end = pList.rend();
			for (const_iterator i = pList.rbegin(); i != end; ++i)
			{
				std::cout << i->first;
				if (i->second.mapping == mappingSVNSources)
					std::cout << " (svn-sources)";
				std::cout << "\n";
			}
		}
	}


	void List::printWithInfos()
	{
		if (not pList.empty())
		{
			const const_iterator end = pList.rend();
			for (const_iterator i = pList.rbegin(); i != end; ++i)
			{
				std::cout << i->first << " " << i->second.compilationMode;
				if (i->second.mapping == mappingSVNSources)
					std::cout << " (svn-sources)";
				std::cout << "\n";
				String path(i->second.path);
				# ifdef YUNI_OS_WINDOWS
				path.convertBackslashesIntoSlashes();
				# endif
				std::cout << "    path    : " << path << "\n";

				std::cout << "    modules : [core]";
				const String::List::const_iterator mend = i->second.modules.end();
				for (String::List::const_iterator j = i->second.modules.begin(); j != mend; ++j)
					std::cout << " " << *j;
				std::cout << "\n";
				std::cout << "    OpenGL  : " << (i->second.supportOpenGL ? "Yes" : "No") << "\n";
				# ifdef YUNI_OS_WINDOWS
				std::cout << "    DirectX : " << (i->second.supportDirectX ? "Yes" : "No") << "\n";
				# endif
			}
		}
	}



	VersionInfo::Settings* List::selected()
	{
		if (pList.empty())
			return nullptr;
		InternalList::iterator i = pList.begin();
		return &i->second;
	}



	bool Settings::configFile(String::List& options, bool displayError) const
	{
		if (compiler.empty())
		{
			if (displayError)
				std::cout << "Error: unknown compiler\n";
			return false;
		}
		String out;
		out << this->path << SEP;
		switch (mapping)
		{
			case mappingSVNSources:
				out << "src" << SEP << "yuni" << SEP;
				break;
			case mappingStandard:
				// Nothing to do
				break;
		}
		out << "yuni.config." << this->compiler;

		if (not IO::File::Exists(out))
		{
			if (displayError)
				std::cout << "Error: impossible to open the config file '" << out << "'\n";
			return false;
		}

		options.clear();
		IO::File::Stream file;
		if (file.open(out))
		{
			CString<8192> buffer;
			while (file.readline(buffer))
				options.push_back(buffer);
		}
		else
		{
			if (displayError)
				std::cout << "Error: Impossible to read '" << out << "'\n";
			return false;
		}
		return true;
	}



	bool Settings::parserModulesOptions(String::List& options, bool displayError)
	{
		// Cleanup if needed
		moduleSettings.clear();
		// End of the list
		const String::List::const_iterator end = options.end();
		// Key
		String key;
		// Value
		String value;
		// Module name
		String modName;
		// Group
		String group;
		// normalized path
		String norm;

		// The default compiler is gcc
		CompilerCompliant compliant = gcc;
		// Checking for Visual Studio
		if (not compiler.empty() && compiler.at(0) == 'v' && compiler.at(1) == 's')
			compliant = visualstudio;

		// For each entry in the ini file
		for (String::List::const_iterator i = options.begin(); i != end; ++i)
		{
			i->extractKeyValue(key, value);
			if (key.empty() || key.first() == '[')
				continue;
			value.trim();
			if (!value)
				continue;

			// Reset
			modName.clear();
			group.clear();

			// Splitting
			const String::Size p = key.find(':');
			if (p == String::npos)
				continue;
			group.assign(key, p);
			modName.assign(key, key.size() - p - 1, p + 1);
			if (not group or not modName)
				continue;

			SettingsPerModule& s = moduleSettings[modName];

			if (group == "path.include")
			{
				IO::Normalize(norm, value);
				switch (compliant)
				{
					case gcc          :
						s.includes[String() << "-I" << QuotePath(norm)] = true;
						break;

					case visualstudio :
						s.includes[String() << "/I" << QuotePath(norm)] = true;
						break;
				}
				continue;
			}

			if (group == "lib,rawcommand")
			{
				s.libs[value] = true;
				continue;
			}

			if (group == "path.lib")
			{
				IO::Normalize(norm, value);
				switch (compliant)
				{
					case gcc          :
						s.libIncludes[String() << "-L" << QuotePath(norm)] = true;
						break;
					case visualstudio :
						s.libIncludes[String() << "/LIBPATH:" << QuotePath(norm)] = true;
						break;
				}
				continue;
			}

			if (group == "lib")
			{
				IO::Normalize(norm, value);
				# ifdef YUNI_OS_MAC
				// it may happen that cmake provides a framework for linking, which is wrong obvioulsy
				CString<32,false> ext;
				IO::ExtractExtension(ext, norm);
				ext.toLower();
				if (ext != ".framework")
					s.libs[String() << "-l" << QuotePath(norm)] = true;
				else
				{
					// adding the parent directory
					String frameworkpath;
					norm += "/../";
					IO::Normalize(frameworkpath, norm);
					s.libIncludes[String() << "-F" << QuotePath(frameworkpath)] = true;
				}
				# else
				switch (compliant)
				{
					case gcc          : s.libs[String() << "-l" << QuotePath(norm)] = true; break;
					case visualstudio : s.libs[String() << QuotePath(norm)] = true; break;
				}
				# endif
				continue;
			}

			if (group == "cxxflag")
			{
				s.cxxFlags[value] = true;
				continue;
			}

			if (group == "define")
			{
				switch (compliant)
				{
					case gcc          : s.defines[String() << "-D" << value] = true; break;
					case visualstudio : s.defines[String() << "/D" << value] = true; break;
				}
				continue;
			}
			if (group == "dependency")
			{
				s.dependencies.push_back(value);
				continue;
			}

			if (group == "framework")
			{
				s.frameworks[String() << "-framework " << QuotePath(value)] = true;
				continue;
			}

			if (displayError)
				std::cout << "Error: unknown key in the config file: '" << key << "'\n";
			return false;
		}
		return true;
	}



	void Settings::SettingsPerModule::merge(OptionMap& out, const OptionMap& with) const
	{
		const OptionMap::const_iterator end = with.end();
		for (OptionMap::const_iterator i = with.begin(); i != end; ++i)
			out[i->first] = true;
	}





} // namespace VersionInfo
} // namespace LibConfig
} // namespace Yuni

