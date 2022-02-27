#pragma once

#include <wx/frame.h>
#include <wx/sizer.h>
#include "yuni/core/event/interfaces.h"
#include "../toolbox/components/button.h"
#include "libs/antares/study/area/links.h"

namespace Antares
{
namespace Window
{

class NTCusage : public wxFrame, public Yuni::IEventObserver<NTCusage>
{
public:
    NTCusage(wxWindow* parent,
             Yuni::Bind<void(Antares::Component::Button&, wxMenu&, void*)>& onPopup,
             wxFlexGridSizer* sizer_flex_grid);

    ~NTCusage() = default;

    void update(Data::AreaLink* link);
    bool isEmpty();

private:
    void onPopupMenu(Component::Button&, wxMenu& menu, void*);

    void onSelectUseNTC(wxCommandEvent&);
    void onSelectSetToNull(wxCommandEvent&);
    void onSelectSetToInfinite(wxCommandEvent&);

    void broadCastChange();

public:
    static Yuni::Event<void(Antares::Data::AreaLink*)> onTransmissionCapacitiesUsageChanges;
private:
    Component::Button* button_ = nullptr;
    Data::AreaLink* currentLink_ = nullptr;
};

}
}