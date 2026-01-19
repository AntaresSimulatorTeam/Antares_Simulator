// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_FWD_H__
#define __ANTARES_FWD_H__

// wxWigets
class wxGrid;
class wxWindow;
class wxTopLevelWindow;

// Antares study library
#include <antares/study/fwd.h>

namespace Antares
{
namespace Map
{
class Component;

} // namespace Map

namespace Component
{
class Notebook;

} // namespace Component

namespace Window
{
class StudyLogs;
class BindingConstraint;
class Notes;
class Sets;

} // namespace Window

namespace Forms
{
class ApplWnd;
template<class WindowT>
class Disabler;

} // namespace Forms

} // namespace Antares

#endif // __ANTARES_FWD_H__
