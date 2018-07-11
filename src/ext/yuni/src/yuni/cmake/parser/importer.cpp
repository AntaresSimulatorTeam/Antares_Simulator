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
#include <yuni/yuni.h>
#include <yuni/string.h>
#include <yuni/io/file.h>
#include <yuni/io/directory.h>
#include <iostream>

using namespace Yuni;



class Reader final
{
public:
	void operator () (const String& line) const
	{
		s = line;
		size_t length = line.size();
		s.replace("\\", "\\\\");
		s.replace("\"", "\\\"");
		s.replace("\r", "\\r");
		s.replace("\n", "\\n");
		s.replace("\t", "\\t");

		content << "\t\tout.append(\"" << s << "\\n\", " << (1 + length) << ");\n";
	}

public:
	mutable Clob content;
	mutable Clob s;
};



int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "USAGE: <template-file> <destination-file>\n";
		return 1;
	}

	AnyString tmplfile(argv[1]);
	AnyString targetfile{argv[2]};

	if (not IO::IsAbsolute(tmplfile))
	{
		std::cerr << "the template filename '" << tmplfile << "' is not absolute\n";
		return 1;
	}
	if (not IO::IsAbsolute(targetfile))
	{
		std::cerr << "the target filename '" << targetfile << "' is not absolute\n";
		return 1;
	}

	String targetFolder;
	IO::ExtractFilePath(targetFolder, targetfile);

	String tmplcontent;

	Reader reader;
	reader.content  = "\n\n// GENERATED\n";
	reader.content += "\n\ntemplate<class StreamT>\nstatic inline void PrepareCPPInclude(StreamT& out)\n{\n";

	if (not IO::File::ReadLineByLine(tmplfile, reader))
	{
		std::cerr << "failed to read '" << tmplfile << "'\n";
		return 1;
	}

	reader.content << "}\n";

	if (not IO::Directory::Create(targetFolder))
	{
		std::cerr << "failed to create the directory '" << targetFolder << "'\n";
		return 1;
	}

	return (IO::File::SetContent(argv[2], reader.content)) ? 0 : 1;
}
