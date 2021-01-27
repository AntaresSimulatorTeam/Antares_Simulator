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

#include <antares/wx-wrapper.h>
#include "clipboard.h"
#include <yuni/core/math.h>
#include "../locales.h"

#include <wx/msgdlg.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>

#include "../../application/main.h"

using namespace Yuni;

namespace Antares
{
namespace Toolbox
{
Clipboard::Clipboard()
{
}

Clipboard::~Clipboard()
{
    pList.clear();
}

void Clipboard::clear()
{
    pList.clear();
}

void Clipboard::copy()
{
    logs.debug() << "[clipboard] copy to clipboard";
#ifdef ANT_WINDOWS
    // The Handle of the main Windows _is_ absolutely necessary !
    // Otherwise `SetClipboardData` may fail, and may sometimes produce
    // a SegV, which is hard to trace.
    const HWND wndHandle = (HWND)Antares::Forms::ApplWnd::Instance()->GetHandle();
    if (!wndHandle)
    {
        logs.error() << "[clipboard] impossible to retrieve the ID of the main window";
        return;
    }
    if (OpenClipboard(wndHandle))
    {
        EmptyClipboard();
        if (!pList.empty())
        {
            HGLOBAL hText;
            const List::iterator end = pList.end();
            for (List::iterator i = pList.begin(); i != end; ++i)
            {
                Item& item = *(*i);
                if (!item.data)
                    continue;

                switch (item.type)
                {
                case typeText:
                {
                    hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, item.data->size() + 2);
                    // Put your string in the global memory...
                    char* ptr = (char*)GlobalLock(hText);
                    if (ptr)
                    {
                        if (not item.data->empty())
                        {
                            ::memcpy(ptr, item.data->c_str(), item.data->size());
                            ptr[item.data->size()] = '\0';
                        }
                        else
                            ptr[0] = '\0';
                        GlobalUnlock(hText);

                        ::SetClipboardData(CF_TEXT, hText);
                    }
                    else
                        // Free memory...
                        GlobalFree(hText);
                    break;
                }
                case typeHTML:
                {
                    // CopyHTML(s.c_str());
                    static int cfid = 0;
                    if (!cfid)
                        cfid = RegisterClipboardFormat(wxT("HTML Format"));

                    if (!cfid)
                        break;
                    hText = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, item.data->size() + 2);

                    // Put your string in the global memory...
                    char* ptr = (char*)GlobalLock(hText);
                    if (ptr)
                    {
                        if (not item.data->empty())
                        {
                            ::memcpy(ptr, item.data->c_str(), item.data->size());
                            ptr[item.data->size()] = '\0';
                        }
                        else
                            ptr[0] = '\0';
                        GlobalUnlock(hText);

                        ::SetClipboardData(cfid, hText);
                    }
                    else
                        // Free memory...
                        GlobalFree(hText);
                    break;
                }
                }
            }
        }
        CloseClipboard();
    }
    else
        logs.error() << "[clipboard] impossible to open an access to the clipboard";

#else

    if (!pList.empty() && wxTheClipboard->Open())
    {
        wxTheClipboard->Clear();
        const List::iterator end = pList.end();
        for (List::iterator i = pList.begin(); i != end; ++i)
        {
            Item& item = *(*i);
            switch (item.type)
            {
            case typeText:
                wxTheClipboard->AddData(new wxTextDataObject(wxStringFromUTF8(item.data->c_str())));
                break;
            case typeHTML:
                wxCustomDataObject* data = new wxCustomDataObject;
                if (not item.data->empty())
                {
                    data->SetFormat(wxDataFormat(wxT("HTML Format")));
                    data->SetData(item.data->size() + 1, (void*)item.data->c_str());
                    wxTheClipboard->AddData(data);
                }
                break;
            }
        }
        wxTheClipboard->Flush();
        wxTheClipboard->Close();
    }
#endif
}

void Clipboard::add(const String& text)
{
    pList.push_back(new Item(typeText, new String(text)));
}

void Clipboard::add(const String::Ptr& text)
{
    pList.push_back(new Item(typeText, text));
}

void Clipboard::add(Antares::Component::Datagrid::VGridHelper* m,
                    uint offsetX,
                    uint offsetY,
                    uint offsetX2,
                    uint offsetY2,
                    bool withStyle,
                    bool withHeaders)
{
    if (!m)
    {
        logs.error() << "[clipboard] invalid grid helper";
        return;
    }

    if (m && m->GetNumberRows() && m->GetNumberCols() && m->renderer())
    {
        uint posX2 = Math::Min<uint>(offsetX2, (uint)m->GetNumberCols());
        uint posY2 = Math::Min<uint>(offsetY2, (uint)m->GetNumberRows());

        Antares::Component::Datagrid::VGridHelper::Indices& indicesCols = m->indicesCols;
        Antares::Component::Datagrid::VGridHelper::Indices& indicesRows = m->indicesRows;

        String::Ptr d = new String();
        if (!d)
            return;
        String tmp;
        d->reserve(400 + 33 * (posX2 + 1 - offsetX) * (posY2 + 1 - offsetY));

        // HTML Format
        *d << "Version:0.9\r\nStartHTML:00000000\r\nEndHTML:00000000\r\nStartFragment:00000000\r\n"
           << "EndFragment:00000000\r\n";

        *d << "<html><head>";
        *d << "<style type='text/css'>";
        m->appendCSSForAllCellStyles2(*d);
        *d << "</style>";
        *d << "</head><body>\r\n";
        *d << "<table>";
        if (withHeaders)
        {
            *d << "<tr><td></td>";
            for (uint x = offsetX; x < posX2; ++x)
            {
                *d << "<th>";
                wxStringToString(m->GetColLabelValue(indicesCols[x]), tmp);
                *d << tmp;
                *d << "</th>";
            }
            *d << "</tr>";
        }

        for (uint y = offsetY; y < posY2; ++y)
        {
            int cellY = indicesRows[y];
            assert(cellY >= 0 && cellY < m->renderer()->height()
                   && "Clipboard: Invalid indice pointer for rows (header)");

            *d << "<tr>";
            if (withHeaders)
            {
                *d << "<th>";
                wxStringToString(m->GetRowLabelValue(indicesRows[y]), tmp);
                *d << tmp;
                *d << "</th>";
            }
            for (uint x = offsetX; x < posX2; ++x)
            {
                int cellX = indicesCols[x];
                assert(cellX >= 0 && cellX < m->renderer()->width()
                       && "Clipboard: Invalid indice pointer for columns (header)");

                *d << "<td";
                if (withStyle)
                {
                    *d << " class='";
                    Component::Datagrid::Renderer::IRenderer::CellStyleToCSSClass(
                      m->renderer()->cellStyle(cellX, cellY), *d);
                    *d << '\'';
                }
                *d << '>';
                tmp.clear();
                m->renderer()->appendCellValue(cellX, cellY, tmp);
                // `.` replacement to get valid number in Excel
                if (Locale::DecimalPoint == ',')
                    tmp.replace('.', ',');
                else
                    tmp.replace(',', '.');

                *d << tmp << "</td>";
            }
            *d << "</tr>";
        }

        *d << "</table></body></html>";
        pList.push_back(new Item(typeHTML, d));

        // TXT Format
        d = new String();
        d->reserve(m->GetNumberRows() * m->GetNumberCols() * 8);
        for (uint y = offsetY; y < posY2; ++y)
        {
            int cellY = indicesRows[y];
            assert(cellY >= 0 && indicesRows[y] < m->renderer()->height()
                   && "Clipboard: Invalid indice pointer for rows (value)");

            bool firstColCell = true;
            for (uint x = offsetX; x < posX2; ++x)
            {
                assert(indicesCols[x] >= 0 && indicesCols[x] < m->renderer()->width()
                       && "Clipboard: Invalid indice pointer for columns (value)");

                if (!firstColCell)
                    *d << '\t';
                firstColCell = false;
                m->renderer()->appendCellValue(indicesCols[x], cellY, *d);
            }
#if defined(ANT_WINDOWS)
            *d << "\r\n";
#else
            *d << '\n';
#endif
        }
        pList.push_back(new Item(typeText, d));
    }
    else
        pList.push_back(new Item(typeText, nullptr));
}

void Clipboard::add(const Matrix<>& m)
{
    if (!m.empty())
    {
        String::Ptr d = new String();

        // TXT Format
        for (uint y = 0; y < m.height; ++y)
        {
            for (uint x = 0; x < m.width; ++x)
            {
                if (y)
                    *d << '\t';
                *d << m.entry[x][y];
            }
#if defined(ANT_WINDOWS)
            *d << "\r\n";
#else
            *d << '\n';
#endif
        }
        pList.push_back(new Item(typeText, d));
        // wxTheApp->Yield();
    }
    else
        pList.push_back(new Item(typeText, nullptr));
}

void Clipboard::GetFromClipboard(String& out)
{
    if (wxTheClipboard && wxTheClipboard->Open())
    {
        if (wxTheClipboard->IsSupported(wxDF_TEXT))
        {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            wxString t = data.GetText();
            if (!t.empty())
                wxStringToString(t, out);
            else
                out.clear();
        }
        else
            out.clear();
        wxTheClipboard->Close();
    }
}

} // namespace Toolbox
} // namespace Antares
