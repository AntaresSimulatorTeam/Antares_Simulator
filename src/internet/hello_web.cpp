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
#include <yuni/thread/timer.h>
#include <yuni/thread/thread.h>

#include <regex>

#include <antares/antares.h>
#include <yuni/yuni.h>
#include <yuni/uuid/uuid.h>
#include <yuni/io/file.h>
#include <yuni/datetime/timestamp.h>
#include <antares/sys/appdata.h>
#include <antares/logs.h>
#include <yuni/core/system/environment.h>
#include <yuni/core/system/suspend.h>
#include <antares/proxy/proxy.h>
#include "../internet/limits.h"

#include "license.h"
#include "licensetimer.h"
#include "base64/cencode.h"
#include "decrypt.hxx"
#include <curl/curl.h>
#include <fstream>

using namespace Yuni;
using namespace std;

#define SEP Yuni::IO::Separator

namespace Antares
{
namespace License
{
//! Global mutex for licensing
static Yuni::Mutex licenseMutex;

/*!
** \brief Global flag to determine whether the OpenSSL engine is initialized or not
**
** The access to this variable must be protected by \p licenseMutex
*/
static bool encryptionEngineInitialized = false;

/*extern*/ bool hasLicenseServers = false;

/*extern*/ enum Error lastError = errNone;

/*extern*/ Yuni::Event<void()> onDisconnect;

/*extern*/ ProxySettings proxy;

/*extern*/ enum Status statusOnline = stInitialize;

/*extern*/ bool hasSimulatorAuthorization = false;

/*extern*/ bool timerlaunched = false;

vector<Yuni::String> used_Machines;

static void launchLicenseTimer(uint tick)
{
    if (tick > 0)
    {
        timerlaunched = true;
        static LicenseTimer* licenseTimer = new LicenseTimer(10000);
        licenseTimer->cycleCount(-1);
        licenseTimer->interval(tick * 1000); // in milliseconds
        licenseTimer->start();
    }
}

enum
{
    minimalLicenseActivationKeyLength = 10,
};

static void ExitWithoutAsking()
{
    exit(42);
}

static inline void CABundleFile(String& file)
{
    String localAppData;
    if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
        localAppData.clear();

    // build file directory
    file << localAppData;
    file << SEP << ANTARES_CA_BUNDLE;
}

static inline bool Fcreaterootca()
{
    String filename;
    CABundleFile(filename);

    std::remove(filename.c_str());
    ofstream rootca(filename.c_str(), ios::out | ios::trunc);

    // if the file can be opened
    if (rootca)
    {
        rootca << ANTARES_ROOT_CERT;
        rootca.close();
        return true;
    }

    return false;
}

static inline void Fremoverootca()
{
    String filename;
    CABundleFile(filename);

    LICENSE_DEBUG("remove rootca");
    std::remove(filename.c_str());
}

#ifndef NDEBUG
// Debug mode for the license manager
inline CURLcode curlDebugPerform(CURL* x)
{
    auto tempname = tmpnam(NULL);
    auto pFile = fopen(tempname, "w");
    curl_easy_setopt(x, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(x, CURLOPT_STDERR, pFile);
    auto code = curl_easy_perform(x);
    // TODO : removed because cause crashes on windows (maybe related to curl version tested
    // (7.53.1) : does curl close file handle ? fclose(pFile);
    std::ifstream filestream(tempname);
    string ligne;
    if (filestream) // si l'ouverture a fonctionné
    {
        while (getline(filestream, ligne))
        {
            logs.debug() << ligne;
        }
    }
    filestream.close();
    remove(tempname);
    return code;
}

#define CURL_PERFORM(x) curlDebugPerform(x)
#else
#define CURL_PERFORM(x) curl_easy_perform(x)
#endif

template<class PredicateT>
static void IterateAllLicenseActivationKeys(uint version, const PredicateT& callback)
{
    bool stop = false;
    String filename;

    String localAppData;
    if (not OperatingSystem::FindAntaresLocalAppData(localAppData, false))
        localAppData.clear();

    String localAppDataAllUsers;
    if (not OperatingSystem::FindAntaresLocalAppData(localAppDataAllUsers, true))
        localAppDataAllUsers.clear();

#ifndef YUNI_OS_WINDOWS
    // The license has not been found, continuing
    // TODO  This method should be improved and should not rely on the variable env
    String hostname;
    if (System::Environment::Read("HOSTNAME", hostname))
    {
        for (float v = version / 100.f; v > 3.9f; v -= 0.1f)
        {
            CString<64, false> vstr;
            vstr << SEP << "antares-" << v;
            vstr.trimRight('0');
            if (vstr.last() == '.')
                vstr += '0';
            vstr << '-' << hostname << ".hwb";

            if (not localAppData.empty())
            {
                filename = localAppData;
                filename += vstr;
                callback(stop, filename, false);
                if (stop)
                    return;
            }
            if (not localAppDataAllUsers.empty())
            {
                filename = localAppDataAllUsers;
                filename += vstr;
                callback(stop, filename, true);
                if (stop)
                    return;
            }
        }
    }
#endif

    for (float v = version / 100.f; v > 3.9f; v -= 0.1f)
    {
        CString<64, false> vstr;
        vstr << SEP << "antares-" << v;
        vstr.trimRight('0');
        if (vstr.last() == '.')
            vstr += '0';
        vstr += ".hwb";

        if (not localAppData.empty())
        {
            filename = localAppData;
            filename += vstr;
            callback(stop, filename, false);
            if (stop)
                return;
        }
        if (not localAppDataAllUsers.empty())
        {
            filename = localAppDataAllUsers;
            filename += vstr;
            callback(stop, filename, true);
            if (stop)
                return;
        }
    }
}

void WriteLastError(String& message, String& errType)
{
    String licenseErr = "Antares Internet server";
    String connectionErr = "Connection to the Internet";

    switch (lastError)
    {
    case errNone:
    {
        message << "Antares token is valid";
        return;
    }

    // License errors
    case errLSTooManyLicense:
    {
        message << "The maximum number of tokens has been reached from the Antares server.\n";
        if (used_Machines.size() > 0)
        {
            message << "These users are currently using antares :\n";
            for (size_t i_used = 0; i_used < used_Machines.size(); i_used++)
            {
                message << used_Machines[i_used] << "\n";
            }
        }
        errType << licenseErr;
        return;
    }
    case errLSOnline:
    {
        message << "Unable to connect (check your proxy settings or try later)";
        errType << licenseErr;
        return;
    }
    case errCAFileRoot:
    {
        message << "Curl root CAcertificat error";
        errType << licenseErr;
        return;
    }
    case errHostid:
    {
        message << "Invalid key";
        errType << licenseErr;
        return;
    }
    case errStart:
    {
        message << "Invalid start date";
        errType << licenseErr;
        return;
    }
    case errSession:
    {
        message << "Invalid session id";
        errType << licenseErr;
        return;
    }
    case errMac:
    {
        message << "Invalid mac address";
        errType << licenseErr;
        return;
    }
    case errToken:
    {
        message << "Invalid token number";
        errType << licenseErr;
        return;
    }
    case errProductExpired:
    {
        message << "License has expired";
        errType << licenseErr;
        return;
    }

    // Connection or server side errors
    case errLSHostDown:
    {
        message << "Impossible to connect to Antares server";
        errType << connectionErr;
        return;
    }
    case errLSPoxy:
    {
        message << "Unable to connect (check your proxy settings)";
        errType << connectionErr;
        return;
    }
    case errDBConnexion:
    {
        message << "Unable to connect (Please try later)";
        errType << connectionErr;
        return;
    }

    // Unexpected error
    default:
    {
        message << "An unexpected error has occured.\nPlease, contact the Antares Support Team";
        return;
    }
    }
}

void InitializeEncryptionEngine()
{
    Yuni::MutexLocker locker(licenseMutex);

    if (not encryptionEngineInitialized)
    {
        // mark the license engine as initialized
        encryptionEngineInitialized = true;
    }

    if (onDisconnect.empty())
        onDisconnect.connect(&ExitWithoutAsking);
}

void ReleaseEncryptionEngine()
{
    Yuni::MutexLocker locker(licenseMutex);

    if (encryptionEngineInitialized)
    {
        encryptionEngineInitialized = false;
    }
}

bool RetrieveActivationKey(uint version, String& activationKey, String* error)
{
    // we will try to retrieve the activation key from a mere file
    // we will iterate through all available files (from the user's home and
    // from /etc) and through all Antares versions
    IterateAllLicenseActivationKeys(
      version, [&](bool& stop, const String& filename, bool /*allusers*/) {
          if (IO::errNone == IO::File::LoadFromFile(activationKey, filename))
          {
              // We have found something !
              // (one file matching our criteria)
              LICENSE_DEBUG("reading " << filename);

              // trivial check
              // (to not take care of pseudo-empty files)
              stop = (activationKey.size() > minimalLicenseActivationKeyLength);
          }
          else
          {
              stop = false;
              LICENSE_DEBUG("could not load " << filename);
          }
      });

    // second pseudo-check, used in the same time to detect error
    // (license file not found)
    if (activationKey.size() < minimalLicenseActivationKeyLength)
    {
        LICENSE_DEBUG(
          "impossible to find a antares.hwb file (not enough permissions or the file is missing)");
        if (error)
            *error = "impossible to find a antares.hwb file (not enough permissions or the file is "
                     "missing)";
        return false;
    }
    return true;
}

bool RevokeAllUserActivationKeys(uint version)
{
    // iterating through all activation keys we can find
    // and removing them (if not dedicated to all users, where we should
    // not have enough permissions anyway)
    IterateAllLicenseActivationKeys(
      version, [&](bool& /*stop*/, const String& filename, bool allusers) {
          if (not allusers) // do not warm global activation keys
          {
              if (IO::File::Exists(filename))
              {
                  logs.info() << "  license: trying to delete " << filename;
                  IO::File::Delete(filename);
              }
          }
      });
    return true;
}

bool CheckLicenseValidity(uint version, String* error)
{
    return false;
}

static size_t write_data(void* ptr, size_t, size_t nmemb, void*)
{
    std::string test = (char*)ptr;
    if (strstr((char*)ptr, "available"))
    {
        statusOnline = stValidOnline;
        lastError = errNone;
    }
    else
    {
        if (strstr((char*)ptr, "connexion database error"))
        {
            statusOnline = stInvalidOnline;
            lastError = errDBConnexion;
        }
        if (strstr((char*)ptr, "license expired"))
        {
            statusOnline = stInvalidOnline;
            lastError = errProductExpired;
        }
        else if (strstr((char*)ptr, "invalid host"))
        {
            statusOnline = stInvalidOnline;
            lastError = errHostid;
        }
        else if (strstr((char*)ptr, "invalid session"))
        {
            statusOnline = stInvalidOnline;
            lastError = errSession;
        }
        else if (strstr((char*)ptr, "invalid token"))
        {
            statusOnline = stInvalidOnline;
            lastError = errToken;
        }
        else if (strstr((char*)ptr, "invalid mac"))
        {
            statusOnline = stInvalidOnline;
            lastError = errMac;
        }
        else if (strstr((char*)ptr, "invalid start"))
        {
            statusOnline = stInvalidOnline;
            lastError = errStart;
        }
        else if (strstr((char*)ptr, "Too many tokens"))
        {
            statusOnline = stInvalidOnline;
            lastError = errLSTooManyLicense;
            used_Machines.clear();
            string tag_name = "<!-- Name -->";
            string::size_type tag_name_size = tag_name.size();
            string end_tag_name = "<!-- /Name -->";
            string::size_type end_tag_name_size = end_tag_name.size();
            // regex expression("(?<=(<!-- Name -->))([^<]{1,20})(?<=(<!-- /Name -->))");
            regex expression("(" + tag_name + ")[^<]{1,20}(" + end_tag_name + ")");
            string str((char*)ptr);
            smatch regexRes;
            string result;
            string::size_type size;
            string::const_iterator searchStart(str.cbegin());
            while (regex_search(searchStart, str.cend(), regexRes, expression))
            {
                result = regexRes[0];
                size = result.size() - (tag_name_size + end_tag_name_size);
                result = result.substr(tag_name_size, size);
                searchStart += regexRes.position() + regexRes.length();
                used_Machines.push_back(result);
            }
        }
        else
        {
            statusOnline = stInvalidOnline;
            lastError = errLSOnline;
        }
    }
    return nmemb;
}

bool CheckOnlineLicenseValidity(uint version, bool recheck)
{
    return false;
}

} // namespace License
} // namespace Antares
