/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */
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
