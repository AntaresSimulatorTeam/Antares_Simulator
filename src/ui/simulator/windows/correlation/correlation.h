// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_WINDOWS_CORRELATION_CORRELATION_H__
#define __ANTARES_WINDOWS_CORRELATION_CORRELATION_H__

#include <yuni/yuni.h>
#include <yuni/core/event.h>
#include <antares/study/study.h>

#include <wx/panel.h>
#include <wx/sizer.h>
#include "../../toolbox/components/notebook/notebook.h"

namespace Antares::Window
{
class CorrelationPanelData;

class CorrelationPanel final: public wxPanel, public Yuni::IEventObserver<CorrelationPanel>
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

} // namespace Antares::Window

#endif // __ANTARES_WINDOWS_CORRELATION_CORRELATION_H__
