
#include "read-index.h"
#include "compound.h"
#include "../logs.h"
// xml
#include "../tinyxml/tinyxml.h"


#define SEP IO::Separator




namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{


	namespace  // anonymous
	{

		class XMLIndexVisitor : public TiXmlVisitor
		{
		public:
			//! \name Constructor
			//@{
			/*!
			** \brief Constructor
			*/
			XMLIndexVisitor(TiXmlDocument& document);
			//! Destructor
			virtual ~XMLIndexVisitor();
			//@}

			virtual bool VisitEnter(const TiXmlDocument& /*doc*/ );

			virtual bool VisitExit(const TiXmlDocument& /*doc*/);

			virtual bool VisitEnter(const TiXmlElement& element, const TiXmlAttribute* attr);

			virtual bool VisitExit(const TiXmlElement& element);

			virtual bool Visit(const TiXmlDeclaration& /*declaration*/);
			virtual bool Visit(const TiXmlText& /*text*/);
			virtual bool Visit(const TiXmlComment& /*comment*/);
			virtual bool Visit(const TiXmlUnknown& /*unknown*/);


		private:
			//! XML document
			TiXmlDocument& pDocument;
			//!
			Compound::Deque  pStack;
			//! Stats
			unsigned int pStats[kdMax];
			//!
			bool pInName;
			//!
			bool pTagMemberAlreadyEncountered;

		}; // class XMLIndexVisitor





		XMLIndexVisitor::XMLIndexVisitor(TiXmlDocument& document) :
			pDocument(document),
			pInName(false),
			pTagMemberAlreadyEncountered(false)
		{
			assert(&pDocument != NULL);

			for (unsigned int i = 0; i != kdMax; ++i)
				pStats[i] = 0u;
		}


		XMLIndexVisitor::~XMLIndexVisitor()
		{
			CString<128,false>  result;
			CString<64,false>  name;

			logs.info() << "Compound statistics from index";
			for (unsigned int i = 1; i != kdMax; ++i)
			{
				name.clear();
				Compound::AppendKindToString(name, (CompoundType) i);

				result.clear();
				result.resize(10, ' ');
				result.overwriteRight(name);

				logs.info() << result << " : " << pStats[i];
			}
		}


		bool XMLIndexVisitor::VisitEnter(const TiXmlDocument& /*doc*/ )
		{
			return true;
		}


		bool XMLIndexVisitor::VisitExit(const TiXmlDocument& /*doc*/)
		{
			return true;
		}


		bool XMLIndexVisitor::VisitEnter(const TiXmlElement& element, const TiXmlAttribute* /*attr*/)
		{
			const TIXML_STRING& strname = element.ValueTStr();

			if (strname == "member" || strname == "compound")
			{
				pTagMemberAlreadyEncountered = false;

				const AnyString  kind = element.Attribute("kind");
				const AnyString  refid = element.Attribute("refid");

				Compound::Ptr compound = new Compound();
				compound->kind = Compound::StringToKind(kind);
				compound->refid = refid;

				++pStats[compound->kind];
				allSymbolsByRefID[compound->refid] = compound;
				allSymbols[compound->kind][compound->refid] = compound;

				if (!pStack.empty())
				{
					Compound::Ptr parent = pStack.front();
					if (!(!parent))
						parent->members[compound->refid] = compound;
				}

				pStack.push_front(compound);
				pInName = false;
				return true;
			}
			if (strname == "name")
			{
				if (!pTagMemberAlreadyEncountered && !pStack.empty())
					pInName = true;
				return true;
			}

			return true;
		}


		bool XMLIndexVisitor::Visit(const TiXmlText& text)
		{
			if (pInName && !pStack.empty())
			{
				Compound::Ptr compound = pStack.front();
				assert(!(!compound));

				const TIXML_STRING& name = text.ValueTStr();
				if (!name.empty())
					compound->name.append(name.c_str(), (unsigned int)name.size());
			}
			return true;
		}


		bool XMLIndexVisitor::Visit(const TiXmlComment&)
		{
			return true;
		}


		bool XMLIndexVisitor::Visit(const TiXmlDeclaration&)
		{
			return true;
		}


		bool XMLIndexVisitor::Visit(const TiXmlUnknown&)
		{
			return true;
		}



		bool XMLIndexVisitor::VisitExit(const TiXmlElement& element)
		{
			const TIXML_STRING& strname = element.ValueTStr();

			if (strname == "member" || strname == "compound")
			{
				// Reset in any case @pInName
				pInName = false;

				if (pStack.empty()) // strange - should never happen
					return true;
				Compound::Ptr compound = pStack.front();
				assert(!(!compound) && "invalid compound");
				pStack.pop_front();

				if (not compound->name.empty())
					allSymbolsByName[compound->kind][compound->name] = compound;
				return true;
			}
			if (strname == "name")
			{
				pInName = false;
				return true;
			}

			return true;
		}



		static void PrepareAllCompounds()
		{
			const Compound::Map::iterator end = allSymbolsByRefID.end();
			for (Compound::Map::iterator i = allSymbolsByRefID.begin(); i != end; ++i)
				(i->second)->prepare();
		}


	} // anonymous namespace








	bool ReadXMLCatalogIndex(const String& folder)
	{
		TiXmlDocument doc;

		// Parsing the XML
		{
			String filename;
			filename << folder << SEP << "index.xml";

			if (!doc.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8))
			{
				logs.error() << "impossible to read the catalog index: " << filename;
				return false;
			}
		}

		// Analyze the XML document
		{
			XMLIndexVisitor visitor(doc);
			if (!doc.Accept(&visitor))
				return false;
		}

		PrepareAllCompounds();
		return true;
	}




} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni


