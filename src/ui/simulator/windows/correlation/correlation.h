/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/
#ifndef __ANTARES_WINDOWS_CORRELATION_CORRELATION_H__
#define __ANTARES_WINDOWS_CORRELATION_CORRELATION_H__

#include <yuni/yuni.h>
#include <yuni/core/event.h>
#include <antares/study/study.h>

#include <wx/panel.h>
#include <wx/sizer.h>
#include "../../toolbox/components/notebook/notebook.h"

namespace Antares
{
namespace Window
{
class CorrelationPanelData;

class CorrelationPanel final : public wxPanel, public Yuni::IEventObserver<CorrelationPanel>
{
public:
    CorrelationPanel(wxWindow* parent, int timeseries);
    ~CorrelationPanel();

    void reload();

    void selectAllDefaultPages();

private:
    void onStudyLoaded();
    void onStudyClosed();

    void onStudyAreaDelete(Data::Area*);
    void onStudyAreaAdded(Data::Area*);

    void onStudyAreasChanged();

    void onSortAlpha(wxCommandEvent&);
    void onSortAlphaReverse(wxCommandEvent&);
    void onSortColor(wxCommandEvent&);

    void updateAllDatasources();

    void assignMatrices(Data::Correlation* corr);

private:
    CorrelationPanelData* pData;
    Component::Notebook::Page* pPageAnnual;

}; // class CorrelationPanel

} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_CORRELATION_CORRELATION_H__
