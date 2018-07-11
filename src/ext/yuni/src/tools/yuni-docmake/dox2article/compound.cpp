
#include "compound.h"



namespace Yuni
{
namespace Edalene
{
namespace Dox2Article
{

	Compound::Map  allSymbolsByRefID;
	
	Compound::MapPerKind  allSymbols = nullptr;

	Compound::MapPerKind  allSymbolsByName = nullptr;






	Compound::Compound() :
		parent(nullptr)
	{}


	Compound::~Compound()
	{}


	void Compound::prepare()
	{
		name.trim("\t\n ");
		htdocs.clear();
		brief.clear();
		description.clear();
		switch (kind)
		{
			case kdFunction:
				break;
			case kdTypedef:
				break;
			case kdClass:
				htdocs << "class/" << name;
				break;
			case kdNamespace:
				htdocs << "namespace/" << name;
				break;
			case kdVariable:
				break;
			case kdEnum:
				 break;
			case kdFile:
				 break;
			case kdFolder:
				 break;
			case kdGroup:
				 break;
			case kdFriend:
				 break;
			case kdUnknown:
				 break;
			case kdMax:
				 break;
		}

		if (!htdocs && parent)
			htdocs = parent->htdocs;

		htdocs.replace("::", "/");
		htdocs.toLower();
	}





} // namespace Dox2Article
} // namespace Edalene
} // namespace Yuni

