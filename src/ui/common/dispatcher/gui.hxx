/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
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
*/
#ifndef __ANTARES_TOOLBOX_ACTION_GUI_HXX__
#define __ANTARES_TOOLBOX_ACTION_GUI_HXX__

class wxDialog;

namespace Antares
{
namespace Private
{
namespace Dispatcher
{
void FormShowModal(wxDialog* window);

template<class FormT, class ParentT>
class JobShowForm : public Yuni::Job::IJob
{
public:
    explicit JobShowForm(ParentT* parent) : pParent(parent)
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

} // namespace Dispatcher
} // namespace Private
} // namespace Antares

namespace Antares
{
namespace Dispatcher
{
namespace GUI
{
template<class FormT, class ParentT>
void CreateAndShowModal(ParentT* parent)
{
    ::Antares::Dispatcher::GUI::Post(
      (const Yuni::Job::IJob::Ptr&)new ::Antares::Private::Dispatcher::JobShowForm<FormT, ParentT>(
        parent),
      70);
}

} // namespace GUI
} // namespace Dispatcher
} // namespace Antares

#endif // __ANTARES_TOOLBOX_ACTION_GUI_HXX__
