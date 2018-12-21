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

#include "sets.h"
#include <yuni/io/file.h>
#include <yuni/io/directory/system.h>
#include <yuni/uuid/uuid.h>
#include "../toolbox/resources.h"
#include "../toolbox/components/button.h"
#include "../toolbox/create.h"
#include <wx/richtext/richtextctrl.h>
#include <wx/richtext/richtextstyles.h>
#include <wx/richtext/richtextxml.h>
#include <wx/richtext/richtextxml.h>
#include <wx/richtext/richtextformatdlg.h>
#include <wx/richtext/richtextsymboldlg.h>
#include <wx/richtext/richtextstyledlg.h>
#include <wx/richtext/richtextprint.h>
#include <wx/statline.h>
#include <antares/study.h>
#include "../application/study.h"
#include "inspector.h"


using namespace Yuni;



namespace Antares
{
namespace Window
{

	Yuni::Event<void (Sets*)> Sets::OnChanged;

	static uint pGlobalRevision = 0;
	static bool pLocked = false;



	BEGIN_EVENT_TABLE(Sets, Antares::Component::Panel)
		EVT_RICHTEXT_CHARACTER(wxID_ANY, Sets::onUserNotesCharacter)
		EVT_RICHTEXT_STYLE_CHANGED(wxID_ANY, Sets::onUserNotesStyleChanged)
		EVT_RICHTEXT_CONTENT_INSERTED(wxID_ANY, Sets::onUserNotesStyleChanged)
		EVT_RICHTEXT_CONTENT_DELETED(wxID_ANY, Sets::onUserNotesStyleChanged)
		EVT_RICHTEXT_BUFFER_RESET(wxID_ANY, Sets::onUserNotesStyleChanged)
	END_EVENT_TABLE()



	Sets::Sets(wxWindow* parent, uint margin) :
		Antares::Component::Panel(parent),
		pRichEdit(nullptr),
		pStyleSheet(nullptr),
		pLocalRevision(0),
		pUpdatesToSkip(0)
	{
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		wxBoxSizer* ssz = new wxBoxSizer(wxHORIZONTAL);
		ssz->AddSpacer(margin);

		Component::Button* btn;

		btn = new Component::Button(this, wxT("[Set]"), nullptr, this, &Sets::onNewSet);
		ssz->Add(btn);

		Antares::Component::AddVerticalSeparator(this, ssz);

		btn = new Component::Button(this, wxT("Caption"), nullptr, this, &Sets::onCaption);
		ssz->Add(btn);
		btn = new Component::Button(this, wxT("Filter"), nullptr, this, &Sets::onFilter);
		ssz->Add(btn);
		btn = new Component::Button(this, wxT("Output"), nullptr, this, &Sets::onOutput);
		ssz->Add(btn);
		btn = new Component::Button(this, wxT("Comments"), nullptr, this, &Sets::onComments);
		ssz->Add(btn);

		Antares::Component::AddVerticalSeparator(this, ssz);

		btn = new Component::Button(this, wxT("+="), nullptr, this, &Sets::onAdd);
		ssz->Add(btn);
		btn = new Component::Button(this, wxT("-="), nullptr, this, &Sets::onRemove);
		ssz->Add(btn);

		Antares::Component::AddVerticalSeparator(this, ssz);


		btn = new Component::Button(this, wxEmptyString, "images/16x16/check.png", this, &Sets::onCheck);
		ssz->Add(btn);


		sizer->Add(ssz, 0, wxALL|wxEXPAND, 2);

		
		// Rich Edit
		wxBoxSizer* richSizer = new wxBoxSizer(wxHORIZONTAL);
		pRichEdit = new wxRichTextCtrl(this, wxID_ANY,
			wxEmptyString, wxDefaultPosition, wxSize(200, 200),
			wxVSCROLL|wxHSCROLL|wxNO_BORDER|wxWANTS_CHARS);
		wxPanel* p = new Antares::Component::Panel(this);
		p->SetSize(margin, 10);
		p->SetBackgroundColour(pRichEdit->GetBackgroundColour());
		richSizer->Add(p, 0, wxALL|wxEXPAND);
		richSizer->Add(pRichEdit, 1, wxALL|wxEXPAND);

		wxBoxSizer* help = new wxBoxSizer(wxVERTICAL);
		wxTextCtrl* hlp = new wxTextCtrl(this, -1, "[Set] : Creates new district from current selection \n\nCaption : Additionnal information about the current set \n\nFilter : add-all or remove-all \n\nOutput : true or false \n\nComments : Text information that is only visible in this file \n\n+= / -= : Add or remove content from district based on the current selection\n\nCheck : Checks syntax (required before saving modifications)", wxDefaultPosition, wxSize(350,1000), wxTE_READONLY | wxTE_MULTILINE | wxBORDER_NONE);
		hlp->SetBackgroundColour(wxNullColour);
		help->Add(hlp, 0, wxALL | wxEXPAND, 2);
		richSizer->Add(help, 0, wxALL | wxEXPAND, 2);

		sizer->Add(richSizer, 1, wxALL|wxEXPAND);


		
		// Sizer
		SetSizer(sizer);

		OnChanged.connect(this, &Sets::onSetsModified);
		OnStudyClosed.connect(this, &Sets::onStudyClosed);
	}


	Sets::~Sets()
	{
		// Remove the temporary file
		if (not pTempFile.empty() and not IO::File::Delete(pTempFile))
		{
			if (IO::File::Exists(pTempFile))
				logs.warning() << "I/O error: impossible to delete " << pTempFile;
			pTempFile.clear();
		}

		destroyBoundEvents();
		delete pStyleSheet;
	}


	void Sets::onStudyClosed()
	{
		// Resetting the internal pseudo revision numbers
		pGlobalRevision = 0;
		pLocalRevision  = 0;
	}


	void Sets::onUserNotesCharacter(wxRichTextEvent&)
	{
		//do nothing
	}


	void Sets::onUserNotesStyleChanged(wxRichTextEvent&)
	{
		//do nothing
	}


	void Sets::notifyChanges()
	{
		if (Data::Study::Current::Valid() and not pLocked)
		{
			saveToStudy();
			MarkTheStudyAsModified();
			++pUpdatesToSkip;
			OnChanged(this);
		}
	}


	void Sets::onSetsModified(Sets* sender)
	{
		assert(pRichEdit);

		if (sender != this and pGlobalRevision != pLocalRevision and not pLocked)
		{
			if (pUpdatesToSkip)
			{
				if (0 != (--pUpdatesToSkip))
					return;
			}
			// Reloading the user set
			loadFromStudy();
			// Updating our local revision number
			pLocalRevision = pGlobalRevision;
		}
	}

	void Sets::onNewSet(void*)
	{
		assert(pRichEdit);
		if (not Data::Study::Current::Valid())
			return;
		auto& study = *Data::Study::Current::Get();
		int numberOfSets = study.setsOfAreas.size() +1;
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		if (x == 0)//Insertion point is at the beginning of a line
		{
			pRichEdit->WriteText(wxString::Format(wxT("[set %i]"), numberOfSets));
		}
		else
		{
			pRichEdit->WriteText(wxString::Format(wxT("\n[set %i]"), numberOfSets));
		}
		pRichEdit->WriteText(wxString::Format(wxT("\ncaption = set %i"), numberOfSets));
		pRichEdit->WriteText("\noutput = true");
		pRichEdit->WriteText("\ncomments =");
		if (Antares::Window::Inspector::SelectionAreaCount())
		{
			auto end = study.areas.end();
			for (auto i = study.areas.begin(); i != end; ++i)
			{
				if (Antares::Window::Inspector::isAreaSelected(i->second->name))
				{
					pRichEdit->WriteText("\n+ = ");
					pRichEdit->WriteText(i->second->name.c_str());
				}
			}
		}
		pRichEdit->WriteText("\n");
	}

	void Sets::onCaption(void*)
	{
		assert(pRichEdit);
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		if (x == 0)//Insertion point is at the beginning of a line
		{
			pRichEdit->WriteText("caption =");
		}
		else
		{
			pRichEdit->WriteText("\ncaption =");
		}
	}

	void Sets::onFilter(void*)
	{
		assert(pRichEdit);
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		if (x == 0)//Insertion point is at the beginning of a line
		{
			pRichEdit->WriteText("apply-filter =");
		}
		else
		{
			pRichEdit->WriteText("\napply-filter =");
		}
	}

	void Sets::onOutput(void*)
	{
		assert(pRichEdit);
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		if (x == 0)//Insertion point is at the beginning of a line
		{
			pRichEdit->WriteText("output =");
		}
		else
		{
			pRichEdit->WriteText("\noutput =");
		}
	}

	void Sets::onComments(void*)
	{
		assert(pRichEdit);
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		if(x == 0)//Insertion point is at the beginning of a line
		{
			pRichEdit->WriteText("comments =");
		}
		else
		{
			pRichEdit->WriteText("\ncomments =");
		}
	}

	void Sets::onAdd(void*)
	{
		assert(pRichEdit);
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		bool first = true;
		auto study = Data::Study::Current::Get();
		if (Antares::Window::Inspector::SelectionAreaCount())
		{
			auto end = study->areas.end();
			for (auto i = study->areas.begin(); i != end; ++i)
			{
				if (Antares::Window::Inspector::isAreaSelected(i->second->name))
				{
					if (first && x == 0)//first += and we are at the beginning of the line
					{
						pRichEdit->WriteText("+ = ");
						first = false;
					}
					else
					{
						pRichEdit->WriteText("\n+ = ");
						first = false;
					}
					pRichEdit->WriteText(i->second->name.c_str());
				}
			}
		}
		else if(x==0)
		{
			pRichEdit->WriteText("+ = ");
		}
		else
		{
			pRichEdit->WriteText("\n+ = ");
		}
	}

	void Sets::onRemove(void*)
	{
		assert(pRichEdit);
		long x;
		long y;
		pRichEdit->PositionToXY(pRichEdit->GetInsertionPoint(), &x, &y);
		bool first = true;
		auto study = Data::Study::Current::Get();
		if (Antares::Window::Inspector::SelectionAreaCount())
		{
			auto end = study->areas.end();
			for (auto i = study->areas.begin(); i != end; ++i)
			{
				if (Antares::Window::Inspector::isAreaSelected(i->second->name))
				{
					if (first && x == 0)
					{
						pRichEdit->WriteText("- = ");
						first = false;
					}
					else
					{
						pRichEdit->WriteText("\n- = ");
						first = false;
					}
					pRichEdit->WriteText(i->second->name.c_str());
				}
			}
		}
		else if (x == 0)
		{
			pRichEdit->WriteText("- = ");
		}
		else
		{
			pRichEdit->WriteText("\n- = ");
		}
	}

	void Sets::onCheck(void*)
	{
		assert(pRichEdit);
		
		wxTextAttr blackTxt;
		blackTxt.SetTextColour(*wxBLACK);
		pRichEdit->SetStyle(pRichEdit->XYToPosition(0, 0), pRichEdit->XYToPosition(pRichEdit->GetLineLength(pRichEdit->GetNumberOfLines() - 1), pRichEdit->GetNumberOfLines() - 1), blackTxt);
		auto study = Data::Study::Current::Get();
		int max = pRichEdit->GetNumberOfLines();
		auto end = study->areas.end();
		for (int i = 0; i < max; i++)
		{
			wxString line = pRichEdit->GetLineText(i);
			wxString lineNopace = pRichEdit->GetLineText(i);
			bool error = false;
			lineNopace.Replace(" ", "");
			int pos = line.Find("=");
			if (pos == wxNOT_FOUND)
			{
				pos = 0;
				
				if (!lineNopace.IsEmpty() && lineNopace.GetChar(0) == '[')
				{
					if (lineNopace.GetChar(lineNopace.Length() - 1) != ']')
					{
						error = true;
					}
					else
					{
						wxString lineStripped = line.SubString(line.Find('[') + 1, line.Find(']') - 1).Trim().Trim(false);
						for (auto area = study->areas.begin(); area != end; ++area)
						{
							if (lineStripped.IsSameAs(area->second->name.c_str(), false))//the area and the district have the same name
							{
								wxTextAttr orangeText;
								orangeText.SetTextColour(wxColor(255, 140, 0));
								pRichEdit->SetStyle(pRichEdit->XYToPosition(line.Find('[') + 1, i), pRichEdit->XYToPosition(line.Find(']'), i), orangeText);							pRichEdit->EndAllStyles();
								break;
							}
						}
					}
				}
				else if (!lineNopace.IsEmpty())
				{
					error = true;
				}
			}
			else if (lineNopace.StartsWith("+=")|| lineNopace.StartsWith("-="))
			{
				error = true;
				line = line.SubString(pos+1, line.length()).Trim().Trim(false); //from after the "=" to the end of the line, trimming spaces from left and right side
				for (auto area = study->areas.begin(); area != end; ++area)
				{
					if (line.IsSameAs(area->second->name.c_str(),false))
					{
						error = false;
						break;
					}
				}
				if (error)
				{
					uint setMax = study->setsOfAreas.size();
					for (uint setIt=0; setIt<setMax; setIt++)
					{
						int comp = study->setsOfAreas.nameByIndex(setIt).compareInsensitive(line.ToStdString());
						if (comp==0)
						{
							error = false;
							break;
						}
					}
				}
			}
			else if (lineNopace.StartsWith("output"))
			{
				line = line.SubString(pos + 1, line.length()).Trim().Trim(false);
				if (!(line.IsSameAs("true") || line.IsSameAs("false")))
				{
					error = true;
				}
			}
			else if (lineNopace.StartsWith("apply-filter"))
			{
				line = line.SubString(pos + 1, line.length()).Trim().Trim(false);
				if (!(line.IsSameAs("add-all") || line.IsSameAs("remove-all")))
				{
					error = true;
				}
			}
			else if (lineNopace.StartsWith("comments"))
			{
				error = false;
			}
			else if (lineNopace.StartsWith("caption"))
			{
				error = false;
			}
			else if (!lineNopace.IsEmpty())
			{
				error = true;
			}
			if (error)
			{
				wxTextAttr redText;
				redText.SetTextColour(*wxRED);
				if (pos != 0)
				{
					pos++;
				}
				pRichEdit->SetStyle(pRichEdit->XYToPosition(pos,i), pRichEdit->XYToPosition(pRichEdit->GetLineLength(i), i), redText);
				pRichEdit->EndAllStyles();
			}
		}
		notifyChanges();
	}

	void Sets::connectToNotification()
	{
		OnChanged.remove(this);
		OnChanged.connect(this, &Sets::onSetsModified);
	}


	void Sets::disconnectFromNotification()
	{
		OnChanged.remove(this);
	}
	
	void Sets::loadFromStudy()
	{
		if (!pRichEdit)
			return;
		if (not Data::Study::Current::Valid())
			return;
		auto& study = *Data::Study::Current::Get();

		if (not pTempFile)
		{
			if (not initializeTemporaryFile())
			{
				logs.error() << "impossible to write temporary file. The area sets won't be loaded";
				return;
			}
		}

		if (not pLocked)
		{
			pLocked = true;
			if (study.setsOfAreas.toString().empty())
			{
				pRichEdit->Clear();
			}
			else
			{
				// Txt version
				pRichEdit->SetValue(wxStringFromUTF8(study.setsOfAreas.toString()));
				wxTextAttr blackTxt;
				blackTxt.SetTextColour(*wxBLACK);
				pRichEdit->SetStyle(pRichEdit->XYToPosition(0, 0), pRichEdit->XYToPosition(pRichEdit->GetLineLength(pRichEdit->GetNumberOfLines() - 1), pRichEdit->GetNumberOfLines()-1), blackTxt);
			}
			pLocked = false;
		}
	}
	
	
	void Sets::saveToStudy()
	{
		auto study = Data::Study::Current::Get();
		if (!pRichEdit || !study)
			return;
		if (!pTempFile)
		{
			if (not initializeTemporaryFile())
			{
				logs.error() << "impossible to write temporary file. The area sets won't be saved.";
				return;
			}
		}

		// Incrementing the pseudo revision
		pLocalRevision = ++pGlobalRevision;
		// Saving the content into an XML file...
		pRichEdit->SaveFile(wxStringFromUTF8(pTempFile), wxRICHTEXT_TYPE_TEXT);
		// ... and reloading it
		//IO::File::LoadFromFile(study->simulation.comments, pTempFile);
		study->setsOfAreas.loadFromFile(pTempFile);
	}



	bool Sets::initializeTemporaryFile()
	{
		IO::Directory::System::Temporary(pTempFile);
		pTempFile << IO::Separator << "antares-sets-";
		UUID uuid;
		uuid.generate();
		pTempFile << uuid << ".txt";

		if (!IO::File::CreateEmptyFile(pTempFile))
		{
			pTempFile.clear();
			return false;
		}
		return true;
	}






} // namespace Window
} // namespace Antares
