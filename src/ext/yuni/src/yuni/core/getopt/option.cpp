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
#include "option.h"
#include <iostream>

#define YUNI_GETOPT_HELPUSAGE_30CHAR  "                             "



namespace Yuni
{
namespace Private
{
namespace GetOptImpl
{


	template<bool Decal, int LengthLimit>
	static void PrintLongDescription(std::ostream& out, const String& description)
	{
		uint start = 0;
		uint end = 0;
		uint offset = 0;

		do
		{
			// Jump to the next separator
			offset = description.find_first_of(" .\r\n\t", offset);

			// No separator, aborting
			if (String::npos == offset)
				break;

			if (offset - start < LengthLimit)
			{
				if ('\n' == description.at(offset))
				{
					out.write(description.c_str() + start, (std::streamsize)(offset - start));
					out << '\n';
					if (Decal)
						out.write(YUNI_GETOPT_HELPUSAGE_30CHAR, 30);

					start = offset + 1;
					end = offset + 1;
				}
				else
					end = offset;
			}
			else
			{
				if (0 == end)
					end = offset;

				out.write(description.c_str() + start, (std::streamsize)(end - start));
				out << '\n';

				if (Decal)
					out.write(YUNI_GETOPT_HELPUSAGE_30CHAR, 30);

				start = end + 1;
				end = offset + 1;
			}

			++offset;
		}
		while (true);

		// Display the remaining piece of string
		if (start < description.size())
			out << (description.c_str() + start);
	}







	void DisplayHelpForOption(std::ostream& out, const String::Char shortName, const String& longName,
		const String& description, bool requireParameter)
	{
		// Space
		if ('\0' != shortName && ' ' != shortName)
		{
			out.write("  -", 3);
			out << shortName;

			if (longName.empty())
			{
				if (requireParameter)
					out.write(" VALUE", 6);
			}
			else
				out.write(", ", 2);
		}
		else
			out.write("      ", 6);

		// Long name
		if (longName.empty())
		{
			if (requireParameter)
				out << "              ";
			else
				out << "                    ";
		}
		else
		{
			out.write("--", 2);
			out << longName;
			if (requireParameter)
				out.write("=VALUE", 6);

			if (30 <= longName.size() + 6 /*-o,*/ + 2 /*--*/ + 1 /*space*/ + (requireParameter ? 6 : 0))
				out << "\n                             ";
			else
			{
				for (uint i = 6 + 2 + 1 + static_cast<uint>(longName.size()) + (requireParameter ? 6 : 0); i < 30; ++i)
					out.put(' ');
			}
		}
		// Description
		if (description.size() <= 50 /* 80 - 30 */)
			out << description;
		else
			PrintLongDescription<true, 50>(out, description);

		out << '\n';
	}


	void DisplayTextParagraph(std::ostream& out, const String& text)
	{
		if (text.size() <= 80)
			out << text;
		else
			PrintLongDescription<false, 80>(out, text);

		out << '\n';
	}




} // namespace GetOptImpl
} // namespace Private
} // namespace Yuni

