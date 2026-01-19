// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "scenario-builder-renderer-base.h"
#include "../../../input/connection.h"

namespace Antares::Component::Datagrid::Renderer
{
class ntcScBuilderRenderer: public ScBuilderRendererBase
{
public:
    ntcScBuilderRenderer();

    ntcScBuilderRenderer& operator=(const ntcScBuilderRenderer&) = delete;
    ntcScBuilderRenderer(const ntcScBuilderRenderer&) = delete;

    bool valid() const override;
    int height() const override;
    wxString rowCaption(int rowIndx) const override;

    bool cellValue(int x, int y, const Yuni::String& value) override;
    double cellNumericValue(int x, int y) const override;

private:
    void onStudyChanged(Data::Study& study);
    Data::AreaLink::Vector* pListOfLinks = nullptr;
}; // class ntcScBuilderRenderer

} // namespace Antares::Component::Datagrid::Renderer
