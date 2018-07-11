
#include "job-compound-writer.h"
#include <yuni/core/system/suspend.h>
#include <yuni/datetime/timestamp.h>
#include "../logs.h"
#include "options.h"
#include "toolbox.h"


#define SEP IO::Separator



namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{
namespace Job
{

	namespace // anonymous
	{

		static Atomic::Int<> GenerationNumericID;

	} // anonymous namespace



	void CompoundWriter::Dispatch()
	{
		// Write articles
		{
			uint count = (uint) allSymbolsByRefID.size();
			switch (count)
			{
				case 0:  logs.info() << "No article";break;
				case 1:  logs.info() << "writing 1 article";break;
				default: logs.info() << "writing " << count << " articles";break;
			}
		}
		{
			Compound::Map::iterator end = allSymbolsByRefID.end();
			for (Compound::Map::iterator i = allSymbolsByRefID.begin(); i != end; ++i)
			{
				Compound::Ptr& compound = i->second;
				Edalene::Dox2Article::Job::queueService += new CompoundWriter(compound);
			}
		}

		queueService.wait();
	}





	CompoundWriter::CompoundWriter(Compound::Ptr& compound) :
		pCompound(compound)
	{
	}


	CompoundWriter::~CompoundWriter()
	{
	}



	void CompoundWriter::onExecute()
	{
		if (!pCompound->name || pCompound->name.contains('@'))
			return;

		switch (pCompound->kind)
		{
			case kdFunction: break;
			case kdTypedef: break;
			case kdClass: buildClass();break;
			case kdNamespace: buildNamespace();break;
			case kdVariable: break;
			case kdEnum: break;
			case kdFile: break;
			case kdFolder: break;
			case kdGroup: break;
			case kdFriend: break;
			case kdUnknown: break;
			case kdMax: break;
		}
	}


	void CompoundWriter::buildClass()
	{
		if (pCompound->name.find_first_of("<@") != String::npos)
			return;

		String filename;
		filename << Options::target << SEP << pCompound->htdocs;
		if (!IO::Directory::Create(filename))
			return;
		filename << SEP << "article.xml";


		IO::File::Stream file;
		if (file.openRW(filename))
		{
			// Getting the name
			const String& name = pCompound->name;

			fileOut.clear();

			String pageTitle;
			{
				String tmp;
				String::Size offset = name.find_last_of(":\\/");
				if (offset < name.size() && offset + 1 < name.size())
					pageTitle.append(name.c_str() + offset + 1, name.size() - (offset + 1));
				else
					pageTitle << name;
				PrepareTitle(tmp, pageTitle);
				fileOut << "<title>" << tmp << "</title>\n";
				fileOut << "<pragma:weight value=\"0.5\" />\n";
				fileOut << "<pragma:toc visible=\"false\" />\n";
				fileOut << "<tag name=\"doxygen\" />\n";
				fileOut << "<tag name=\"dox:class\" />\n";
				fileOut << "\n\n\n";

				fileOut << "<h2>";
				if (not pCompound->brief.empty())
					PrepareTitle(tmp, pCompound->brief);
				else
					PrepareTitle(tmp, pageTitle);
				// in some cases, a final point is in the string, and it is not especially beautiful
				fileOut << tmp;
				fileOut << "</h2>\n";
			}

			bool isAbstract = (pageTitle.first() == 'I');

			OrderedSection sectionmap;
			buildSectionMapping(sectionmap, isAbstract);

			if (!sectionmap.empty())
			{
				// resetting temporary stream outputs
				out.clear();
				for (uint i = 0; i != 2; ++i)
				{
					for (uint j = 0; j != (uint) kdMax; ++j)
						outC[i][j].clear();
				}

				// Starting the table
				out << "<table class=\"doxygen_table\">\n";

				// iterating through all sections found
				OrderedSection::const_iterator end = sectionmap.end();
				for (OrderedSection::const_iterator i = sectionmap.begin(); i != end; ++i)
				{
					// Append all sections found
					Section::Vector::const_iterator send = i->second.end();
					for (Section::Vector::const_iterator j = i->second.begin(); j != send; ++j)
						appendClassSection(/*section*/ *(*j), isAbstract);
				}

				// End of table
				out << "</table>\n\n\n";
			}

			// Writing the begining of the article (title...)
			file << fileOut;

			// Preparing indexes from temporary buffers
			fileOut.clear();
			for (uint i = 1; i < 2; --i)
			{
				for (uint j = 0; j != (uint) kdMax; ++j)
				{
					if (not outC[i][j].empty())
						appendClassIndex(fileOut, (i != 0) /*isPublic*/, (CompoundType) j, outC[i][j]);
				}
			}

			if (not fileOut.empty())
				fileOut << "<h2>Detailed Description</h2>";

			if (not pCompound->description.empty())
			{
				fileOut << "<div>" << pCompound->description
					<< "</div><div style=\"margin-top:3em;border:1px solid #aaa;border-bottom-style:none;border-left-style:none;border-right-style:none\"></div>\n";
			}

			// Writing indexes
			file << fileOut;
			// Writing detailed description
			file << out;
		}
	}



	void CompoundWriter::buildNamespace()
	{
		if (pCompound->name.find_first_of("<@") != String::npos)
			return;

		String filename;
		filename << Options::target << SEP << pCompound->htdocs;
		if (!IO::Directory::Create(filename))
			return;
		filename << SEP << "article.xml";

		IO::File::Stream file;
		if (file.openRW(filename))
		{
			// Getting the name
			const String& name = pCompound->name;

			file << "<title>";
			String::Size offset = name.find_last_of(":\\/");
			if (offset < name.size() && offset + 1 < name.size())
				file.write(name.c_str() + offset + 1, name.size() - (offset + 1));
			else
				file << name;
			file << "</title>\n";
			file << "<pragma:weight value=\"0.40\" />\n";
			file << "<tag name=\"doxygen\" />\n";
			file << "<tag name=\"dox:namespace\" />\n";
			file << "\n\n";
		}
	}



	void CompoundWriter::buildSectionMapping(OrderedSection& map, bool isAbstract)
	{
		// just in case
		map.clear();
		// Section ID
		CString<48,false> id;

		uint count = (uint) pCompound->sections.size();
		for (uint i = 0; i != count; ++i)
		{
			const Section::Ptr& sectionptr = pCompound->sections[i];
			if (!sectionptr) // just in case
				continue;
			const Section& section = *sectionptr;

			if (section.kind.startsWith("public-"))
				id = "0-public";
			else if (section.kind.startsWith("protected-"))
			{
				// Protected and private data should not be displayed when the class is not inherited
				if (!isAbstract)
					continue;
				id = "1-protected";
			}
			else if (section.kind.startsWith("private-"))
			{
				// Skipping all private members
				continue;
			}
			else
				id = "0-public";

			// We will accept the fact that the first item is enough
			// to determine the section order
			//
			// In the same time, this loop will filter empty sections
			// (push_back will never be called) which may occur in some
			// rare cases
			uint memcount = (uint) section.members.size();
			for (uint j = 0; j != memcount; ++j)
			{
				const Member::Ptr& memberptr = section.members[j];
				const Member& member = *memberptr;

				// useless stuff - junk
				if (member.name == "YUNI_STATIC_ASSERT")
					continue;
				if (member.kind == kdFriend)
					continue;

				if (member.kind == kdFunction)
					id += "-1-method";
				else if (member.kind == kdVariable)
					id += "-2-vars";
				else
					id += "-0-typedef-enum";

				map[id].push_back(sectionptr);
				break;
			}
		}
	}


	void CompoundWriter::prepareClassSubtitle(const Section& section)
	{
		subtitle = "<tr><td class=\"doxnone\"></td><td class=\"doxnone\">";

		if (not section.caption.empty())
		{
			HtmlEntities(sectionName, section.caption);
			subtitle << "<h3 class=\"doxygen_section\">" << sectionName << " <code class=\"doxygen_visibility\">" << visibility << "</code></h3>\n";
		}
		else
		{
			sectionName.clear();
			subtitle << "<h3 class=\"doxygen_section\">" << visibility << " <code class=\"doxygen_visibility\">" << visibility << "</code></h3>\n";
		}
		subtitle << "</td></tr>\n";
	}



	void CompoundWriter::appendClassSection(const Section& section, bool isAbstract)
	{
		umlSymbol = '+';
		visibility.clear();
		bool isPublic = true;

		if (section.kind.startsWith("public-"))
			visibility = "Public";
		else if (section.kind.startsWith("protected-"))
		{
			// Protected and private data should not be displayed when the class is not inherited
			if (!isAbstract)
				return;
			visibility = "Protected";
			umlSymbol = '#';
			isPublic = false;
		}
		else if (section.kind.startsWith("private-"))
		{
			// Skipping all private members
			return;
			//visibility = "Private";
			//umlSymbol = '-';
		}
		else
			visibility = "Public";

		// class subtitle
		prepareClassSubtitle(section);

		bool subtitleAlreadyWritten = false;
		bool firstIndexMember = true;
		uint memcount = (uint) section.members.size();
		for (uint j = 0; j != memcount; ++j)
		{
			const Member::Ptr& memberptr = section.members[j];
			const Member& member = *memberptr;
			if (member.name == "YUNI_STATIC_ASSERT")
				continue;
			if (member.kind == kdFriend)
				continue;

			if (!subtitleAlreadyWritten)
			{
				out << subtitle;
				subtitleAlreadyWritten = true;
			}

			if (firstIndexMember && (member.kind == kdFunction || member.kind == kdTypedef))
			{
				firstIndexMember = false;
				Clob& outIx = outC[isPublic][member.kind];
				if (outIx.empty())
				{
					outIx<< "<table class=\"nostyle\">";
					outIx << "<tr><td></td><td><h4>";
				}
				else
					outIx << "<tr><td></td><td><br /><h4>";
				outIx << sectionName << "</h4></td></tr>\n";
			}

			out << "<tr>";

			id.clear() << member.name << '_' << (++GenerationNumericID) << DateTime::Now();
			id.replace('-', '_'); // prevent against int overflow
			toggle.clear() << "toggleVisibility('" << id << "')";

			HtmlEntities(name, member.name);
			HtmlEntities(type, member.type);
			ArrangeTypename(type);

			switch (member.kind)
			{
				case kdFunction: out << "<td class=\"doxygen_fun\">";break;
				case kdTypedef:  out << "<td class=\"doxygen_typedef\">";break;
				case kdVariable: out << "<td class=\"doxygen_var\">";break;
				case kdEnum:     out << "<td class=\"doxygen_enum\">";break;
				default: out << "<td>";break;
			}
			out << "</td><td class=\"doxnone\"><div class=\"doxygen_brief\">";

			if (not member.brief.empty())
				out << "<b>" << member.brief << "</b><div class=\"doxygen_name_spacer\"></div>\n";
			out << "<code>";

			switch (member.kind)
			{
				case kdFunction:
					appendClassFunction(member, isPublic);
					break;
				case kdTypedef:
					appendClassTypedef(member, isPublic);
					break;
				case kdVariable:
					appendClassVariable();
					break;
				default:
					out << "<i>(unmanaged tag: " << (uint) member.kind << ")</i>";
					break;
			}

			out << "</code>\n";

			out << "</div></td>";

			out << "</tr>";
			out << "<tr><td class=\"doxnone doxreturn\"></td><td class=\"doxnone\">\n";
			out << "<div class=\"doxygen_name_spacer\"></div>\n<div class=\"doxygen_desc\">";

			if (not member.detailedDescription.empty())
				out << member.detailedDescription;

			out << "\n</div>\n";
			out << "</td>";
			out << "</tr>\n";

		} // each member
	}



	void CompoundWriter::appendClassFunction(const Member& member, bool isPublic)
	{
		Clob& outIx = outC[isPublic][kdFunction];
		if (outIx.empty())
			outIx << "<table class=\"nostyle\">";
		outIx << "<tr><td class=\"doxygen_index\"><code>";
		if (member.isStatic)
			outIx << "<span class=\"keyword\">static</span> ";
		outIx << type << ' ';
		outIx << "</code></td><td class=\"doxygen_index_def\"><code>";

		if (!member.templates.empty())
		{
			out << "<div class=\"doxygen_tmpllist\">";
			out << "<span class=\"keyword\">template</span>&lt;";
			for (uint p = 0; p != member.templates.size(); ++p)
			{
				if (p)
					out << ", ";
				const Parameter::Ptr& paramstr = member.templates[p];
				const Parameter& param = *paramstr;
				HtmlEntities(paramType, param.type);
				HtmlEntities(paramName, param.name);
				ArrangeTypename(paramType);
				out << paramType << ' ' << paramName;
			}
			out << "&gt;</div>\n";
		}

		if (name.first() == '~')
		{
			String t = name;
			t.replace("~", "<b> ~ </b>");
			out << " <span class=\"method\"><a name=\"" << member.htmlID << "\" href=\"#\">" << umlSymbol << ' ' << t << "</a></span>";
			outIx << " <span class=\"method\"><a href=\"#" << member.htmlID << "\">" << t << "</a></span>";
		}
		else
		{
			out << " <span class=\"method\"><a name=\"" << member.htmlID << "\" href=\"#\">" << umlSymbol << ' ' << name << "</a></span>";
			outIx << " <span class=\"method\"><a href=\"#" << member.htmlID << "\">" << name << "</a></span>";
		}

		out << ": ";

		if (member.isStatic)
			out   << "<span class=\"keyword\">static</span> ";

		out   << type << " (";
		outIx << '(';

		for (uint p = 0; p != member.parameters.size(); ++p)
		{
			if (p)
			{
				out << ", ";
				outIx << ", ";
			}
			const Parameter::Ptr& paramstr = member.parameters[p];
			const Parameter& param = *paramstr;
			HtmlEntities(paramType, param.type);
			HtmlEntities(paramName, param.name);
			ArrangeTypename(paramType);
			out << paramType << ' ' << paramName;
			outIx << paramType << ' ' << paramName;
		}

		out << ')';
		outIx << ')';

		if (member.isConst)
		{
			out  << " <span class=\"keyword\">const</span>";
			outIx << " <span class=\"keyword\">const</span>";
		}
		out << ";\n";
		outIx << "</code></td></tr>\n";
	}



	void CompoundWriter::appendClassTypedef(const Member& member, bool isPublic)
	{
		Clob& outIx = outC[isPublic][kdTypedef];
		if (outIx.empty())
			outIx << "<table class=\"nostyle\">";
		out
			<< "<span class=\"method\"><a name=\"" << member.htmlID << "\" href=\"#\">" << umlSymbol << ' ' << name << "</a></span>"
			<< ": <span class=\"keyword\">typedef</span> "
			<< type
			<< ";\n";
		outIx
			<< "<tr><td class=\"doxygen_index\"><code><span class=\"keyword\">typedef</span></code></td>"
			<< "<td class=\"doxygen_index_def\"><code><span class=\"method\"><a name=\"#" << member.htmlID << "\" href=\"#\">" << name << "</a></span> : "
			<< type
			<< "</code></td></tr>\n";
	}


	void CompoundWriter::appendClassVariable()
	{
		out << "<span class=\"method\"><a href=\"#\">" << umlSymbol << ' ' << name << "</a></span>";
		out << ": " << type;
		out << ';';
	}


	void CompoundWriter::appendClassIndex(Clob& output, bool isPublic, CompoundType compoundType, const Clob& data)
	{
		output << "<h3>";
		if (isPublic)
			output << "Public ";
		else
			output << "Protected ";

		Compound::AppendKindToString(output, compoundType);
		output << "</h3>\n";

		output << "<div class=\"doxygen_brief\">";
		output << data;
		output << "</table></div>\n\n";
	}






} // namespace Job
} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

