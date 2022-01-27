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

#include "mainpanel.h"
#include <wx/dcbuffer.h>
#include <wx/sizer.h>
#include "../../application/study.h"
#include "../../application/main/main.h"
#include "../../config.h"
#include "../../toolbox/resources.h"
#include "../../../common/lock.h"

using namespace Yuni;

namespace Antares
{
namespace Component
{
BEGIN_EVENT_TABLE(MainPanel, Panel)
EVT_PAINT(MainPanel::onDraw)
EVT_ERASE_BACKGROUND(MainPanel::onEraseBackground)
END_EVENT_TABLE()

MainPanel::MainPanel(wxWindow* parent) :
 Panel(parent), pCachedSizeY(50), pBeta(nullptr), pRC(nullptr)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Required by both GTK and Windows
    SetSize(100, 50);

    OnStudyChanged.connect(this, &MainPanel::onStudyChanged);
    OnStudySaved.connect(this, &MainPanel::onStudyChanged);
    OnStudySavedAs.connect(this, &MainPanel::onStudyChanged);
#ifdef ANTARES_BETA
    pBeta = Resources::BitmapLoadFromFile("images/misc/beta.gif");
#endif
#if ANTARES_RC != 0
    pRC = Resources::BitmapLoadFromFile("images/misc/rc.gif");
#endif
}

MainPanel::~MainPanel()
{
    destroyBoundEvents();
    delete pBeta;
    delete pRC;
}

void MainPanel::studyCaption(const wxString& s)
{
    pStudyCaption = s;
    Dispatcher::GUI::Refresh(this);
}

void MainPanel::author(const wxString& s)
{
    pAuthor = s;
    Dispatcher::GUI::Refresh(this);
}

void MainPanel::addProperty(wxDC& dc,
                            const wxString& caption,
                            const wxString& text,
                            const wxSize& size,
                            int& posY) const
{
    // Grabbing the bounding box of the text caption, to align it to the right
    const wxSize s = dc.GetTextExtent(caption);
    const wxSize st = dc.GetMultiLineTextExtent(text);

    // Drawing the caption
    dc.SetTextForeground(wxColour(200, 200, 230));
    dc.DrawText(caption, 50 + size.GetWidth() - s.GetWidth(), posY);
    // Drawing the text
    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.DrawLabel(text, wxRect(60 + size.GetWidth(), posY, 10000, st.GetHeight()));

    posY += st.GetHeight() + 1;
}

static wxString thermalTSBehaviorTag(Data::Study::Ptr study)
{
    // Some thermal clusters force gen, some force no TS generation
    wxString s;
    const bool forceGen = StudyHasThermalForceGen(study);
    const bool forceNoGen = StudyHasThermalForceNoGen(study);
    if (forceGen && forceNoGen)
        s << wxT("[G|NG]");
    else
    {
        // Some thermal clusters force TS generation
        if (forceGen)
            s << wxT("[G]");
        // Some thermal clusters force no TS generation
        if (forceNoGen)
            s << wxT("[NG]");
    }
    return s;
}

void MainPanel::onDraw(wxPaintEvent&)
{
    // The font re-used for each drawing
    enum
    {
        fontSize = 8,
    };
    static const wxFont font(wxFontInfo(fontSize).Family(wxFONTFAMILY_SWISS).FaceName("Tahoma"));

    // The DC
    wxAutoBufferedPaintDC dc(this);
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(dc);

    // Cute font
    dc.SetFont(font);

    // Drawing the background
    const wxRect rect = this->GetRect();
    dc.GradientFillLinear(rect, wxColour(30, 30, 30), wxColour(100, 100, 100), wxSOUTH);
    // Drawing the border
    dc.SetPen(wxPen(wxColour(20, 20, 20), 1));
    dc.DrawLine(0, rect.GetHeight() - 1, rect.GetWidth(), rect.GetHeight() - 1);

#ifdef ANTARES_BETA
    if (pBeta)
        dc.DrawBitmap(*pBeta, 0, 0, true);
#endif
#if ANTARES_RC != 0
    if (pRC)
        dc.DrawBitmap(*pRC, 0, 0, true);
#endif

    // Getting the size of the longest property name
    // We assume here that the first property (name of the study) is the longest one
    static const wxString name = wxT("Study");
    wxSize size = dc.GetTextExtent(name);
    int posY = 10;

    // the current opened study
    auto study = Data::Study::Current::Get();
    if (!(!study))
    {
        // Property: Name of the study
        {
            wxFont f = dc.GetFont();
            f.SetWeight(wxFONTWEIGHT_BOLD);
            dc.SetFont(f);
            wxString caption;

            if (study->header.version != (uint)Data::versionLatest)
            {
                if (StudyHasBeenModified())
                    caption << wxT("(*) ");
                caption << pStudyCaption << wxT(" (v")
                        << Data::VersionToWStr((Data::Version(study->header.version))) << wxT(")");
            }
            else
            {
                if (not StudyHasBeenModified())
                    caption << pStudyCaption;
                else
                    caption << wxT("(*) ") << pStudyCaption;
            }
            caption << " " << thermalTSBehaviorTag(study);
            addProperty(dc, name, caption, size, posY);

            f.SetWeight(wxFONTWEIGHT_LIGHT);
            dc.SetFont(f);
        }

        // Property: The folder
        if (study->folder.empty())
            addProperty(dc, wxT("from "), wxT("<memory>"), size, posY);
        else
            addProperty(dc, wxT("from "), wxStringFromUTF8(study->folder), size, posY);
    }

    // Checking if the actual height of the control is the good one
    posY += 10;
    if (posY != pCachedSizeY)
    {
        // The actual size of the panel is not the best size, which has just been computed
        // during the drawing.
        // We have to resize the panel.
        pCachedSizeY = posY;
        assert(GetParent());
        wxSizer* sizer = GetParent()->GetSizer();
        sizer->SetItemMinSize(this, 100, posY);
        sizer->Layout();
        // Then to force a refresh again
        Dispatcher::GUI::Refresh(GetParent());
    }
    else
    {
        if (!(!study) and study->readonly())
        {
            // We are in readonly mode

            // We would like a smaller font
            wxFont newF(*wxSWISS_FONT);
            newF.SetPointSize(newF.GetPointSize() - 1);
            dc.SetFont(newF);

            const wxString readOnly(wxT(" Read only "));
            wxSize size = dc.GetTextExtent(readOnly);

            wxRect newRect(20, pCachedSizeY >> 1, size.GetWidth() + 12, size.GetHeight() + 6);
            newRect.x = rect.x + rect.width - 20 - newRect.width;
            newRect.y -= newRect.GetHeight() >> 1;

            dc.SetPen(wxPen(wxColour(70, 70, 70)));
            dc.DrawRectangle(
              newRect.x - 1, newRect.y - 1, newRect.GetWidth() + 3, newRect.GetHeight() + 3);

            dc.SetPen(wxPen(wxColour(0, 0, 0)));
            dc.DrawRectangle(
              newRect.x - 1, newRect.y - 1, newRect.GetWidth() + 2, newRect.GetHeight() + 2);

            dc.GradientFillLinear(newRect, wxColour(255, 100, 0), wxColour(183, 0, 0), wxSOUTH);

            // Drawing the caption
            dc.SetTextForeground(wxColour(0, 0, 0));
            dc.DrawText(readOnly, newRect.x + 7, newRect.y + 4);
            dc.SetTextForeground(wxColour(255, 255, 255));
            dc.DrawText(readOnly, newRect.x + 6, newRect.y + 3);
        }
    }
}

void MainPanel::onStudyChanged(Data::Study&)
{
    refreshFromStudy();
}

void MainPanel::onStudyChanged()
{
    refreshFromStudy();
}

void MainPanel::refreshFromStudy()
{
    Forms::ApplWnd* mainFrm = Forms::ApplWnd::Instance();
    if (!mainFrm or IsGUIAboutToQuit())
    {
        mainFrm->title();
        return;
    }

    if (not Data::Study::Current::Valid())
    {
        pStudyCaption.clear();
        pAuthor.clear();
        mainFrm->title();
    }
    else
    {
        auto& study = *Data::Study::Current::Get();

        // Caption
        if (study.header.caption.empty())
        {
            pStudyCaption.clear();
            mainFrm->title();
        }
        else
        {
            pStudyCaption = wxStringFromUTF8(study.header.caption);
            mainFrm->title(pStudyCaption);
        }

        // Author
        if (study.header.author.empty())
            pAuthor.clear();
        else
            pAuthor = wxStringFromUTF8(study.header.author);
    }

    // Refresh
    Dispatcher::GUI::Refresh(this);
}

} // namespace Component
} // namespace Antares
