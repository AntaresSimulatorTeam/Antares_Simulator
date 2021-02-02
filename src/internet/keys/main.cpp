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
#include <yuni/yuni.h>
#include <yuni/core/system/process.h>
#include <yuni/core/getopt.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>
#include <yuni/core/math/random/default.h>
#include <vector>
#include <iostream>

#include "../../config.h"

using namespace Yuni;

int main(int argc, char** argv)
{
    // The parser
    GetOpt::Parser getopt;

    getopt.addParagraph(String() << "Antares read-key v" << ANTARES_VERSION_PUB_STR << "\n");

    String optInputFile;
    String optOutputFile;
    String name;

    // --input
    getopt.add(optInputFile, 'i', "input", "Specify an input file");
    // --output
    getopt.add(
      optOutputFile, 'o', "output", "Specify an output file where the content will be APPENDED");
    // --name
    getopt.add(name, 'n', "name", "Specify the function name");

    // --pid
    String optPID;
    getopt.add(optPID, 'p', "pid", "Specify the file where to write the process ID");

    getopt.addParagraph("\nHelp");
    // --version
    bool optVersion = false;
    getopt.addFlag(optVersion, 'v', "version", "Print the version of the solver and exit");

    // Ask to parse the command line
    if (!getopt(argc, argv))
        return (getopt.errors() ? EXIT_FAILURE : 0);

    // Version
    if (optVersion)
    {
        std::cout << ANTARES_VERSION_STR << std::endl;
        return 0;
    }

    if (!optPID.empty())
    {
        IO::File::Stream pidfile;
        if (pidfile.openRW(optPID))
            pidfile << ProcessID();
        else
            std::cerr << "impossible to write pid file " << optPID << '\n';
    }

    if (name.empty())
    {
        std::cerr << "Invalid empty function name\n";
        return EXIT_FAILURE;
    }
    if (optInputFile.empty())
    {
        std::cerr << "Invalid empty filename\n";
        return EXIT_FAILURE;
    }
    if (optOutputFile.empty())
    {
        std::cerr << "Invalid empty filename\n";
        return EXIT_FAILURE;
    }

    // normalizing the  filename
    {
        String tmp;
        // input
        IO::MakeAbsolute(tmp, optInputFile);
        IO::Normalize(optInputFile, tmp);
        // output
        IO::MakeAbsolute(tmp, optOutputFile);
        IO::Normalize(optOutputFile, tmp);
    }

    Clob content;
    IO::File::LoadFromFile(content, optInputFile);
    // removing any final line feeds
    content.trim();

    if (content.empty())
    {
        std::cerr << "empty content\n";
        return EXIT_FAILURE;
    }

    std::vector<uint> charToCopy;
    charToCopy.resize(content.size());
    for (uint i = 0; i != content.size(); ++i)
        charToCopy[i] = i;

    Clob out;
    out << "\n\n";
    out << "/*!\n";
    out << "** \\brief License Key\n";
    out << "**\n";
    out << "** \\code\n** ";

    for (uint i = 0; i != content.size(); ++i)
    {
        char c = content[i];
        if (c == '\n')
        {
            out << "\n** ";
        }
        else
            out += c;
    }
    out << "\n** \\endcode\n";
    out << "*/\n";
    out << "# define " << name << "(S) \\\n";
    out << "\tdo \\\n";
    out << "\t{ \\\n";
    out << "\t\tS.resize(" << content.size() << "); \\\n";

    Math::Random::Default rnd;
    uint perLine = 0;
    CString<32, false> t;
    while (not charToCopy.empty())
    {
        uint index = rnd() % charToCopy.size();
        auto i = charToCopy.begin() + index;
        assert(*i < content.size());

        if (!perLine)
            out += "\t\t";

        char c = content[*i];
        t.clear();
        switch (c)
        {
        case '\n':
            t << "S[" << *i << "] = '\\n';";
            break;
        case '\r':
            t << "S[" << *i << "] = '\\r';";
            break;
        case '\t':
            t << "S[" << *i << "] = '\\t';";
            break;
        default:
            t << "S[" << *i << "] = '" << c << "';";
            break;
        };
        out += t;
        charToCopy.erase(i);

        for (unsigned int j = t.size(); j < 15; ++j)
            out += ' ';

        if (perLine == 5)
        {
            perLine = 0;
            out += "\\\n";
        }
        else
            ++perLine;
    }

    if (!perLine)
        out << "} \\\n";
    else
        out << "\\\n\t} \\\n";

    out << "\twhile (0)";
    out << "\n\n\n";

    IO::File::AppendContent(optOutputFile, out);

    return 0;
}
