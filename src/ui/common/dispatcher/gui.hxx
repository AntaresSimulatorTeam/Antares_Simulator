// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_TOOLBOX_ACTION_GUI_HXX__
#define __ANTARES_TOOLBOX_ACTION_GUI_HXX__

class wxDialog;

namespace Antares::Private::Dispatcher
{
void FormShowModal(wxDialog* window);

template<class FormT, class ParentT>
class JobShowForm: public Yuni::Job::IJob
{
public:
    explicit JobShowForm(ParentT* parent):
        pParent(parent)
    {
    }

    virtual ~JobShowForm()
    {
    }

protected:
    virtual void onExecute()
    {
        // An event will be triggered to avoid flickering
        Antares::Dispatcher::GUI::ShowModal(new FormT(pParent));
    }

private:
    ParentT* pParent;
};

} // namespace Antares::Private::Dispatcher

namespace Antares::Dispatcher::GUI
{
template<class FormT, class ParentT>
void CreateAndShowModal(ParentT* parent)
{
    ::Antares::Dispatcher::GUI::Post(
      (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobShowForm<FormT, ParentT>(
        parent),
      70);
}

} // namespace Antares::Dispatcher::GUI

#endif // __ANTARES_TOOLBOX_ACTION_GUI_HXX__
