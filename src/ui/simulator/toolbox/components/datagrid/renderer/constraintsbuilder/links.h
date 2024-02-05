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
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_H__
#define __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_H__

#include "../../renderer.h"
#include <antares/study/study.h>
#include "../solver/constraints-builder/cbuilder.h"
#include <map>
#include <yuni/core/bind.h>
#include <yuni/core/event.h>

namespace Antares
{
namespace Component
{
namespace Datagrid
{
namespace Renderer
{
namespace ConstraintsBuilder
{
class Links final : public virtual Renderer::IRenderer
{
public:
    //! Map of string
    using Map = std::map<Yuni::String, Yuni::String>;
    class Record final
    {
    public:
        enum Type
        {
            tyAC,
            tyDC,
            tyACPST,
        };
        class Vector final
        {
        public:
            using iterator = std::vector<Record>::iterator;
            using const_iterator = std::vector<Record>::const_iterator;

        public:
            size_t size() const
            {
                return array.size();
            }

            bool empty() const
            {
                return array.empty();
            }

        public:
            mutable Yuni::Mutex mutex;
            std::vector<Record> array;
            bool canRunAnalyzer;
        };

    public:
        //! Get if the item is enabled
        bool enabled;
        //! Index of the area in the study
        Data::AreaName areaFromId;
        //! Index of the area in the study
        Data::AreaName areaWithId;
        //! Name of the link (wx)
        wxString wxLinkName;
        //! Type of the link
        Antares::Data::AssetType type;

        //! Width and height of the matrix
        uint mWidth, mHeight;
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Constructor
    */
    Links(CBuilder* cBuilder) : pMapFile(nullptr), pCBuilder(cBuilder){};
    //! Destructor
    virtual ~Links();
    //@}

    void mapFile(Map* newMap);
    Map* mapFile();

    Record::Vector& record()
    {
        return pRecord;
    }

    void onBeforeUpdate(const Yuni::Bind<void(int, int)>& b)
    {
        pOnBeforeUpdate = b;
    }
    void onAfterUpdate(const Yuni::Bind<void(int, int)>& b)
    {
        pOnAfterUpdate = b;
    }

    virtual int width() const
    {
        return 2;
    }

    virtual int height() const
    {
        return pCBuilder->linkCount();
    }

    virtual wxString columnCaption(int colIndx) const;

    virtual wxString rowCaption(int rowIndx) const;

    virtual wxString cellValue(int, int) const;

    virtual double cellNumericValue(int, int) const;

    virtual bool cellValue(int x, int y, const Yuni::String& value);

    virtual void resetColors(int, int, wxColour&, wxColour&) const
    { /*Do nothing*/
    }

    virtual IRenderer::CellStyle cellStyle(int col, int row) const;
    virtual wxColour cellBackgroundColor(int, int) const;
    virtual wxColour cellTextColor(int, int) const;

    virtual bool valid() const;

    /*!
    ** \brief Reset internal variables according the current study
    */
    void initializeFromStudy();

protected:
    virtual void onUpdate()
    {
    }

private:
    Map* pMapFile;
    Record::Vector pRecord;
    CBuilder* pCBuilder;
    Yuni::Bind<void(int, int)> pOnBeforeUpdate;
    Yuni::Bind<void(int, int)> pOnAfterUpdate;

}; // class Areas

} // namespace ConstraintsBuilder
} // namespace Renderer
} // namespace Datagrid
} // namespace Component
} // namespace Antares

#endif // __ANTARES_TOOLBOX_COMPONENT_DATAGRID_RENDERER_LINKS_H__
