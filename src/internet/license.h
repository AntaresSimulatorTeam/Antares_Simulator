/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
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
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_LICENSE_H__
#define __ANTARES_LIBS_LICENSE_H__

#include <antares/antares.h>
#include <yuni/core/string.h>
#include <map>
#include <antares/study/version.h>
#include <yuni/core/event.h>



namespace Antares
{
namespace License
{

enum Error
{
    //! No error, the license is valid
    errNone = 0,
    //! Generic error
    errFailed,
    //! The maximum number of licenses has been reached from the Internet Server (LS)
    errLSTooManyLicense,
    //! The Internet server is down \o/
    errLSHostDown,
    //! The proxy is not correct
    errLSPoxy,
    //! The License is not valid on the internet server
    errLSOnline,
    //! The license root CAcertificat error
    errCAFileRoot,
    //! The hostid is not valid
    errHostid,
    //! The product is expired
    errProductExpired,
    //! The license database connexion error
    errDBConnexion,
    //! The licence server rejected the machine start date
    errStart,
    //! The licence server rejected the session id
    errSession,
    //! The licence server rejected the mac address
    errMac,
    //! The licence server rejected the token request (NaN or <0)
    errToken,
};

enum Status
{
    //! The license has not been verfied on the internet server
    stInitialize = 0,
    //! The verification on the server is not requested
    stNotRequested,
    //! The verification is running
    stWaiting,
    //! The license is validated on the Internet Server
    stValidOnline,
    //! The license is not validated in the Internet Server
    stInvalidOnline,
};

// ---------------------------------------------------------------------------
// low-level routines
// ---------------------------------------------------------------------------

enum
{
    encryptionKeyLength = 16,
    encryptionIVLength = 8,
    encryptionLength = encryptionKeyLength + encryptionIVLength
};

//! Key used for encryption
typedef struct
{
    Yuni::CString<encryptionKeyLength, false> key;
    Yuni::CString<encryptionIVLength, false> iv;
} EncryptionKey;

//! Dictionary for properties
typedef std::map<YString, YString> Properties;

/*!
** \brief Retrieve the mac address of the host
**
** \param out the string containing the mac address -> string
*/
void firstMacAddress(YString& out);

} // namespace License
} // namespace Antares

#include "licensecacert.h"

#endif // __ANTARES_LIBS_LICENSE_H__
