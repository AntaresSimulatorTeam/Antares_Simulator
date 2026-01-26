// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_APPLICATION_H__
#define __ANTARES_APPLICATION_H__

#include "../toolbox/wx-wrapper.h"

namespace Antares
{
class Application final : public wxApp
{
public:
    //! \name Constructor & Destructor
    //@{

    /*!
    ** \brief Default constructor
    */
    Application();

    /*!
    ** \brief Destructor
    */
    virtual ~Application();

    //@}

    /*!
    ** \brief Initialize the application
    */
    bool OnInit();

}; // class Application

} // namespace Antares

#endif // __ANTARES_APPLICATION_H__
