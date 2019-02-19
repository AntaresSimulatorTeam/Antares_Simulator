
#include "variable-collector.h"


namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
	
	VariableCollector::VariableCollector(	Spotlight::IItem::Vector& out,
											const Spotlight::SearchToken::Vector& tokens,
											wxBitmap* bmp,
											wxBitmap* thermalBmp,
											const String& selection
										) :
			pOut(out),
			pTokens(tokens),
			pBmp(bmp),
			pThermalBmp(thermalBmp),
			pCurrentSelection(selection)
	{
	}


	void VariableCollector::add(const AnyString& name, const AnyString& /*unit*/, const AnyString& /*comments*/)
	{
		if (!pTokens.empty())
		{
			Spotlight::SearchToken::Vector::const_iterator end = pTokens.end();
			Spotlight::SearchToken::Vector::const_iterator i = pTokens.begin();
			bool gotcha = false;
			for (; i != end; ++i)
			{
				const String& text = (*i)->text;
				if (name.icontains(text))
				{
					gotcha = true;
					break;
				}
			}
			if (!gotcha)
				return;
		}

		Spotlight::IItem* item = new Spotlight::IItem();
		if (item)
		{
			item->caption(name);
			item->group("Variables");
			if (pBmp)
				item->image(*pBmp);
			if (pCurrentSelection == name)
				item->select();
			pOut.push_back(item);
		}
	}

	
	void VariableCollector::addCluster(const String& name)
	{
		if (!pTokens.empty())
		{
			Spotlight::SearchToken::Vector::const_iterator end = pTokens.end();
			Spotlight::SearchToken::Vector::const_iterator i = pTokens.begin();
			bool gotcha = false;
			for (; i != end; ++i)
			{
				const String& text = (*i)->text;
				if (name.icontains(text))
				{
					gotcha = true;
					break;
				}
			}
			if (!gotcha)
				return;
		}

		Spotlight::IItem* item = new Spotlight::IItem();
		if (item)
		{
			item->caption(name);
			item->group("Clusters");
			if (pThermalBmp)
				item->image(*pThermalBmp);
			if (pCurrentSelection == name)
				item->select();
			pOut.push_back(item);
		}
	}
	
}
}
}
}