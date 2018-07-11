
#include "job-compound-explorer.h"
#include "../logs.h"
#include <yuni/core/system/suspend.h>
#include "../tinyxml/tinyxml.h"
#include "options.h"
#include "toolbox.h"

#define SEP Yuni::IO::Separator




namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{
namespace Job
{


	namespace  // anonymous
	{


		class XMLCompoundVisitor : public TiXmlVisitor
		{
		public:
			//! \name Constructor
			//@{
			/*!
			** \brief Constructor
			*/
			XMLCompoundVisitor(Compound& compound, TiXmlDocument& document);
			//! Destructor
			virtual ~XMLCompoundVisitor();
			//@}

			virtual bool VisitEnter(const TiXmlDocument& /*doc*/ );

			virtual bool VisitExit(const TiXmlDocument& /*doc*/);

			virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* attr);

			virtual bool VisitExit(const TiXmlElement& element);

			virtual bool Visit(const TiXmlDeclaration& /*declaration*/);
			virtual bool Visit(const TiXmlText& /*text*/);
			virtual bool Visit(const TiXmlComment& /*comment*/);
			virtual bool Visit(const TiXmlUnknown& /*unknown*/);

			Member::Ptr currentMember();

			void startNewParagraph(String* text);

		private:
			Compound& pCompound;
			//!
			bool pInCompoundDef;
			bool pInSectionHeader;
			bool pInMemberDef;
			bool pInMemberDefName;
			bool pInMemberDefinition;
			bool pInMemberParam;
			bool pInMemberBrief;
			bool pInMemberDetailedDescription;
			bool pInMemberTemplates;
			//! XML document
			TiXmlDocument& pDocument;
			//!
			Section::Deque pSections;

			//! \name Paragraph
			//@{
			//!
			String* pCurrentParagraph;
			//! Temporary string
			String pS;
			//! Codeline
			unsigned int pParagraphCodelineCount;
			//@}

		}; // class XMLCompoundVisitor





		XMLCompoundVisitor::XMLCompoundVisitor(Compound& compound, TiXmlDocument& document) :
			pCompound(compound),
			pInCompoundDef(false),
			pInSectionHeader(false),
			pInMemberDef(false),
			pInMemberDefName(false),
			pInMemberParam(false),
			pInMemberBrief(false),
			pInMemberDetailedDescription(false),
			pInMemberTemplates(false),
			pDocument(document),
			pCurrentParagraph(nullptr)
		{
		}


		XMLCompoundVisitor::~XMLCompoundVisitor()
		{
		}


		Member::Ptr XMLCompoundVisitor::currentMember()
		{
			if (!pSections.empty())
			{
				Section::Ptr section = pSections.front();
				if (!(!section) && !section->members.empty())
					return section->members[section->members.size() - 1];
			}
			return nullptr;
		}


		bool XMLCompoundVisitor::VisitEnter(const TiXmlDocument& /*doc*/ )
		{
			return true;
		}


		bool XMLCompoundVisitor::VisitExit(const TiXmlDocument& /*doc*/)
		{
			return true;
		}


		bool XMLCompoundVisitor::VisitEnter(const TiXmlElement& element, const TiXmlAttribute* /*attr*/)
		{
			const TIXML_STRING& strname = element.ValueTStr();

			if (strname == "memberdef")
			{
				if (pSections.empty())
					return true;

				Section::Ptr section = pSections.front();
				Member* member = new Member();
				member->id = element.Attribute("id");
				IO::ExtractFileName(member->htmlID, member->id);
				member->kind = Compound::StringToKind(AnyString(element.Attribute("kind")));
				if (member->kind == kdUnknown)
					logs.warning() << "unknown type: " << AnyString(element.Attribute("kind"));
				member->visibility = element.Attribute("prot");
				member->isStatic = AnyString(element.Attribute("static")).to<bool>();
				member->isConst = AnyString(element.Attribute("const")).to<bool>();
				member->isExplicit = AnyString(element.Attribute("explicit")).to<bool>();
				member->isInline = AnyString(element.Attribute("inline")).to<bool>();

				section->members.push_back(member);
				pInMemberDef = true;
				pInMemberDefName = false;
				pInMemberDefinition = false;
				pInMemberParam = false;
				pInMemberBrief = false;
				pInMemberDetailedDescription = false;
				pCurrentParagraph = nullptr;
				return true;
			}
			if (strname == "name")
			{
				if (pInMemberDef)
					pInMemberDefName = true;
				return true;
			}
			if (strname == "declname")
			{
				if (pInMemberDef && pInMemberParam)
					pInMemberDefName = true;
				return true;
			}
			if (strname == "templateparamlist")
			{
				if (pInMemberDef)
					pInMemberTemplates = true;
			}
			if (strname == "type")
			{
				if (pInMemberDef)
					pInMemberDefinition = true;
				return true;
			}
			if (strname == "briefdescription")
			{
				if (pInMemberDef)
				{
					Member::Ptr member = currentMember();
					startNewParagraph((!member) ? nullptr : &(member->brief));
					pInMemberBrief = true;
				}
				else
					startNewParagraph(&(pCompound.brief));
				return true;
			}
			if (strname == "detaileddescription")
			{
				if (pInMemberDef)
				{
					Member::Ptr member = currentMember();
					startNewParagraph((!member) ? nullptr : &(member->detailedDescription));
					pInMemberDetailedDescription = true;
				}
				else
					startNewParagraph(&(pCompound.description));
				return true;
			}
			if (strname == "parameterlist")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "<div class=\"doxygen_params\">Parameters :</div><ul>";
				return true;
			}
			if (strname == "parameteritem")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "<li>";
			}
			if (strname == "parameternamelist")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "<code>";
			}
			if (strname == "programlisting")
			{
				if (pCurrentParagraph)
				{
					if (!pParagraphCodelineCount)
						(*pCurrentParagraph) += "<source type=\"cpp\">";
				}
				return true;
			}
			if (strname == "computeroutput")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "<code>";
				return true;
			}
			if (strname == "itemizedlist")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "<ul>";
				return true;
			}
			if (strname == "listitem")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "<li>";
				return true;
			}
			if (strname == "simplesect")
			{
				if (pCurrentParagraph)
				{
					AnyString kind = element.Attribute("kind");
					if (kind == "return")
						(*pCurrentParagraph) += "<div><code>return</code> ";
					else if (kind == "warning")
						(*pCurrentParagraph) += "<div><b>warning</b> : ";
					else if (kind == "see")
						(*pCurrentParagraph) += "<div><code>see</code> : ";
					else if (kind == "note")
						(*pCurrentParagraph) += "<div><b>note</b> : ";
					else if (kind == "remark")
						(*pCurrentParagraph) += "<div><b>remark</b> : ";
					else if (kind == "attention")
						(*pCurrentParagraph) += "<div><b>attention</b> : ";
					else
					{
						logs.warning() << "unmanaged simplesect '" << kind;
						(*pCurrentParagraph) += "<div>";
					}
				}
				return true;
			}

			if (strname == "param")
			{
				if (pInMemberDef && !pCurrentParagraph)
				{
					Member::Ptr member = currentMember();
					if (!(!member))
					{
						if (pInMemberTemplates)
							member->templates.push_back(new Parameter());
						else
							member->parameters.push_back(new Parameter());
						pInMemberParam = true;
					}
				}
				return true;
			}
			if (strname == "sectiondef")
			{
				const AnyString  stat = element.Attribute("static");
				Section* section = new Section();
				const AnyString kind = element.Attribute("kind");
				section->kind = kind;
				section->visibility = element.Attribute("prot");
				section->isStatic = AnyString(element.Attribute("static")).to<bool>();
				pSections.push_front(section);
				pInSectionHeader = false;
				pInMemberDef = false;
				pInMemberDefName = false;
				pInMemberDefinition = false;
				pInMemberBrief = false;
				pInMemberDetailedDescription = false;
				pCurrentParagraph = nullptr;
				return true;
			}
			if (strname == "header")
			{
				if (pSections.empty())
					return false;
				pInSectionHeader = true;
				return true;
			}
			if (strname == "compounddef")
			{
				const AnyString id = element.Attribute("id");
				if (pCompound.refid == id)
				{
					pInCompoundDef = true;
					pInMemberDef = false;
					pInMemberDefName = false;
					pInMemberDefinition = false;
					pInMemberBrief = false;
					pInMemberDetailedDescription = false;
					pCurrentParagraph = nullptr;
				}
				return true;
			}
			return true;
		}


		bool XMLCompoundVisitor::Visit(const TiXmlText& text)
		{
			if (pInMemberDef && pInMemberDefName)
			{
				if (!pSections.empty())
				{
					Section::Ptr section = pSections.front();
					if (!(!section) && !section->members.empty())
					{
						Member::Ptr member = section->members[section->members.size() - 1];
						const TIXML_STRING& name = text.ValueTStr();
						if (pInMemberParam)
						{
							if (pInMemberTemplates)
							{
								if (!member->templates.empty())
								{
									Parameter::Ptr& param = member->templates[member->templates.size() - 1];
									if (not param->name.empty())
										param->name += ' ';
									param->name.append(name.c_str(), (unsigned int)name.size());
								}
							}
							else
							{
								if (!member->parameters.empty())
								{
									Parameter::Ptr& param = member->parameters[member->parameters.size() - 1];
									if (not param->name.empty())
										param->name += ' ';
									param->name.append(name.c_str(), (unsigned int)name.size());
								}
							}
						}
						else
							member->name.append(name.c_str(), (unsigned int)name.size());
					}
				}
				return true;
			}
			if (pInMemberDef && pInMemberDefinition)
			{
				if (!pSections.empty())
				{
					Section::Ptr section = pSections.front();
					if (!(!section) && !section->members.empty())
					{
						Member::Ptr member = section->members[section->members.size() - 1];
						const TIXML_STRING& name = text.ValueTStr();

						if (pInMemberParam)
						{
							if (pInMemberTemplates)
							{
								if (!member->templates.empty())
								{
									Parameter::Ptr& param = member->templates[member->templates.size() - 1];
									if (not param->type.empty())
										param->type += ' ';
									param->type.append(name.c_str(), (unsigned int)name.size());
								}
							}
							else
							{
								if (!member->parameters.empty())
								{
									Parameter::Ptr& param = member->parameters[member->parameters.size() - 1];
									if (not param->type.empty())
										param->type += ' ';
									param->type.append(name.c_str(), (unsigned int)name.size());
								}
							}
						}
						else
						{
							if (not member->type.empty())
								member->type += ' ';
							member->type.append(name.c_str(), (unsigned int)name.size());
						}
					}
				}
				return true;
			}
			if (pInSectionHeader)
			{
				Section::Ptr section = pSections.front();
				if (!(!section))
				{
					const TIXML_STRING& name = text.ValueTStr();
					section->caption.append(name.c_str(), (unsigned int)name.size());
				}
				return true;
			}

			if (pCurrentParagraph)
			{
				const TIXML_STRING& name = text.ValueTStr();
				if (not (*pCurrentParagraph).empty())
					(*pCurrentParagraph) += ' ';
				HtmlEntities(pS, AnyString(name.c_str(), (unsigned int)name.size()));
				(*pCurrentParagraph) += pS;
				return true;
			}

			return true;
		}


		bool XMLCompoundVisitor::Visit(const TiXmlComment&)
		{
			return true;
		}


		bool XMLCompoundVisitor::Visit(const TiXmlDeclaration&)
		{
			return true;
		}


		bool XMLCompoundVisitor::Visit(const TiXmlUnknown&)
		{
			return true;
		}



		bool XMLCompoundVisitor::VisitExit(const TiXmlElement& element)
		{
			const TIXML_STRING& strname = element.ValueTStr();

			if (strname == "memberdef")
			{
				pInMemberDef = false;
				return true;
			}
			if (strname == "name")
			{
				if (pInMemberDef)
					pInMemberDefName = false;
			}
			if (strname == "declname")
			{
				if (pInMemberDef && pInMemberParam)
					pInMemberDefName = false;

			}
			if (strname == "type")
			{
				if (pInMemberDef)
					pInMemberDefinition = false;
			}
			if (strname == "briefdescription")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph).trim();
				pInMemberBrief = false;
				pCurrentParagraph = nullptr;
			}
			if (strname == "detaileddescription")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph).trim();
				pInMemberDetailedDescription = false;
				pCurrentParagraph = nullptr;
			}
			if (strname == "parameterlist")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</ul>";
			}
			if (strname == "parameteritem")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</li>";
			}
			if (strname == "parameternamelist")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</code>";
			}
			if (strname == "programlisting")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</source>\n";
				return true;
			}
			if (strname == "computeroutput")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</code>";
				return true;
			}
			if (strname == "codeline")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += '\n';
				return true;
			}
			if (strname == "itemizedlist")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</ul>";
				return true;
			}
			if (strname == "para")
			{
				if (pCurrentParagraph && !(*pCurrentParagraph).endsWith("<br />"))
					(*pCurrentParagraph) += "<br />";
				return true;
			}

			if (strname == "listitem")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</li>";
				return true;
			}

			if (strname == "sp")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += ' ';
				return true;
			}
			if (strname == "simplesect")
			{
				if (pCurrentParagraph)
					(*pCurrentParagraph) += "</div>";
				return true;
			}

			if (strname == "templateparamlist")
			{
				if (pInMemberDef)
					pInMemberTemplates = false;
				return true;
			}
			if (strname == "param")
			{
				if (pInMemberDef)
					pInMemberParam = false;
				return true;
			}
			if (strname == "sectiondef")
			{
				pCompound.sections.push_back(pSections.front());
				pSections.pop_front();
				return true;
			}
			if (strname == "header")
			{
				pInSectionHeader = false;
				return true;
			}
			if (strname == "compounddef")
			{
				pInCompoundDef = false;
				return true;
			}
			return true;
		}


	} // anonymous namespace





	void CompoundExplorer::Dispatch()
	{
		typedef Compound::Map::iterator iterator;

		logs.info() << "reading class/namespace members";

		// Namespaces
		{
			const iterator end = allSymbols[kdNamespace].end();
			for (iterator i = allSymbols[kdNamespace].begin(); i != end; ++i)
			{
				Compound::Ptr& compound = i->second;
				queueService += new CompoundExplorer(compound);
			}
		}
		// Classes
		{
			const iterator end = allSymbols[kdClass].end();
			for (iterator i = allSymbols[kdClass].begin(); i != end; ++i)
			{
				Compound::Ptr& compound = i->second;
				queueService += new CompoundExplorer(compound);
			}
		}

		queueService.wait();
	}




	CompoundExplorer::CompoundExplorer(const Compound::Ptr& compound) :
		pCompound(compound)
	{
	}


	CompoundExplorer::~CompoundExplorer()
	{
	}


	void CompoundExplorer::onExecute()
	{
		if (!pCompound)
			return;

		Compound& compound = *pCompound;
		if (!compound.refid)
			return;

		TiXmlDocument doc;

		// Parsing the XML
		{
			String filename;
			filename << Options::doxygenXMLIndex << SEP << compound.refid << ".xml";

			if (!doc.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
			{
				logs.error() << "impossible to read the compound index: " << filename;
				return;
			}
		}

		// Analyze the XML document
		{
			XMLCompoundVisitor visitor(compound, doc);
			if (!doc.Accept(&visitor))
				return;
		}
	}


	void XMLCompoundVisitor::startNewParagraph(String* text)
	{
		if (text)
		{
			(*text).clear();
			pCurrentParagraph = text;
			pParagraphCodelineCount = 0;
		}
		else
			pCurrentParagraph = nullptr;
	}




} // namespace Job
} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

