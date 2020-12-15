/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#include <ctype.h>
#include "uri.h"
#include "../../io/file.h"
#include "../string.h"
#include "stdio.h"
// begin added by Papa NDIAYE on 07/03/2016
extern "C"
{
    FILE __iob_func[3] = {*stdin, *stdout, *stderr};
}
// end
namespace Yuni
{
namespace // anonymous
{
class BuildSession final
{
public:
    BuildSession(Private::Uri::Informations& inf, String& string) :
     location(partScheme),
     infos(inf),
     tag(0),
     str(string),
     lastPosition(0u),
     indx(0u),
     pMustRemoveDotSegments(false)
    {
    }

    /*!
    ** \brief Build informations about the URI
    */
    void run();

private:
    bool postFlight();

    /*!
    ** \brief Extract the scheme part
    **
    ** The scheme may not exist and it is possible to be directly redirect
    ** to the method of the path extraction
    */
    bool extractScheme(const String::value_type c);

    /*!
    ** \brief May Extract the server informations
    **
    ** At this point of the global extraction, we are not sure to deal with
    ** some server informations. It could possibly be a path
    */
    bool extractAuthorty(const String::value_type c);

    /*!
    ** \brief Extract the server informations
    **
    ** The method `extractAuthorty()` has detected some real server informations.
    ** Actually the following string has been detected :
    ** \code
    ** scheme://
    ** \endcode
    ** But it can be a path, like this :
    ** \code
    ** file:///path/to/somewhere
    ** \endcode
    */
    bool extractServerInfos(const String::value_type c);

    /*!
    ** \brief Extract the port of the server
    **
    ** The method `extractServerInfos()` has detected that a port value
    ** was given.
    ** Actually it might not be a port value, but a password. For example :
    ** \code
    ** myserver:8080
    ** \endcode
    ** and
    ** \code
    ** myuser:mypass
    ** \endcode
    ** The difference between the two items is ambigous until we found a `@`.
    */
    bool extractPort(const String::value_type c);

    /*!
    ** \brief Extract the path
    **
    ** The path may be prefixed by a query and/or a fragment
    */
    bool extractPath(const String::value_type c);

    /*!
    ** \brief Extract the query
    */
    bool extractQuery(const String::value_type c);

    /*!
    ** \brief Extract the fragment
    */
    bool extractFragment();

private:
    /*!
    ** \brief All parts in an URI
    */
    enum Parts
    {
        partScheme,
        partServer,
        partServerForSure,
        partUser,
        partPassword,
        partPort,
        partPath,
        partQuery,
        partFragment
    };

private:
    //! Where are we in the string ?
    Parts location;
    //! Reference to the real container of all parts of the URI
    Private::Uri::Informations& infos;
    //! A state variable used for several purposes
    int tag;
    //! The raw string (trimmed)
    String& str;
    //! The last outstanding position in the string
    uint lastPosition;
    //! The current index in the string
    uint indx;

    //! Get if there are some dot segments to remove
    bool pMustRemoveDotSegments;

}; // class BuildSession

bool BuildSession::postFlight()
{
    if (lastPosition < (uint)str.size())
    {
        // The last outstanding position was inside the string. Thus we
        // still have some data that remain to be copied.

        // size of the remaining string part
        uint remainingSize = str.size() - lastPosition;

        switch (location)
        {
        case partScheme:
        {
            // Suffix reference
            infos.server = str;
            break;
        }
        case partServerForSure:
        {
            infos.server.append(str, remainingSize, lastPosition);
            break;
        }
        case partPort:
        {
            ShortString32 intstr(str, lastPosition, remainingSize);
            infos.port = intstr.to<sint32>();
            break;
        }
        case partPath:
        {
            infos.path.append(str, remainingSize, lastPosition);
            break;
        }

        case partQuery:
        {
            infos.query.append(str, remainingSize, lastPosition);
            break;
        }
        case partFragment:
        {
            infos.fragment.append(str, remainingSize, lastPosition);
            break;
        }
        default:
            return false;
        }
    }
    // The scheme must be in lowercase
    infos.scheme.toLower();

    // Shall we remove dot segments ?
    if (not infos.path.empty())
    {
        if (not pMustRemoveDotSegments)
        {
            // This solution to detect final dot (eg. `/path/to/file/.`) is not
            // really satisfying and can lead to unnecessary tests to remove
            // the dot segments (e.g. `file://filewith_missing_extension.`).
            pMustRemoveDotSegments = ('.' == infos.path.last());
        }
        if (pMustRemoveDotSegments)
        {
            String tmp;
            tmp.reserve(infos.path.size());
            IO::Normalize(tmp, infos.path);
        }
    }
    return true;
}

bool BuildSession::extractScheme(const String::value_type c)
{
    switch (c)
    {
    case '[': // Suffix reference ?
    {
        if (0 == this->indx) // Indeed, Here is the begining of an IPv6 address
        {
            // `.` or `/` : Actually it is a path (absolute or relative)
            // This part may not fully compliant with the RFC but it allows
            // relative filename
            location = partServerForSure;
            this->indx = 0;
            // The next loop will have `indx` == 1, so we must keep the information
            // about the encountered bracket
            this->tag = 1;
            //
            return true;
        }
        // A bracket should not be alone in the middle of the nowhere
        return false;
    }
    case '.':
    case '/':
    {
        if (0 == this->indx) // Begining of an absolute path
        {
            // `.` or `/` : Actually it is a path (absolute or relative)
            // This part may not fully compliant with the RFC but it allows
            // relative filename
            location = partPath;
            return true;
        }
        // If it is not a path, it can not be a scheme. It seems to be
        // a suffix reference (with a missing scheme) like this :
        // www.libyuni.org which should be http://www.libyuni.org for example
        location = partServerForSure;
        --indx;
        return true;
    }
    case '#': // Begining of a fragment in a scheme ?
    {
        if (0 == this->indx)
        {
            // Actually the URI is only a fragment
            lastPosition = indx + 1;
            location = partFragment;
            return true;
        }
        // Suffix reference : We was dealing with a host name in fact
        location = partServerForSure;
        --indx;
        break;
    }
    case '?': // Begining of a query in a scheme ?
    {
        if (0 == this->indx)
        {
            // Actually the URI is only a query
            lastPosition = indx + 1;
            location = partQuery;
            return true;
        }
        // Suffix reference : We were dealing with a host name in fact
        location = partServerForSure;
        --indx;
        break;
    }
    case ':':
    {
        // End of the scheme
        infos.scheme.append(str, 0, indx);
        infos.scheme.toLower();
        lastPosition = indx + 1;
        location = partServer;
        return true;
    }
    }
    return true;
}

bool BuildSession::extractAuthorty(const String::value_type c)
{
    switch (c)
    {
    case '.':
    {
        if (tag <= 2) // With slashes or not at the begining, it is a relative filename
        {
            location = partPath;
            lastPosition = indx;
        }
        break;
    }
    case '/': // Strange, keeping a count of slashes for further investigation
    {
        ++tag;
        if (3 == tag) // Too many slashes, definitely a path
        {
            location = partPath;
            lastPosition = indx;
        }
        break;
    }
    default:
    {
        switch (tag)
        {
        case 0: // Not a host name but a path
        {
            location = partPath;
            tag = 0;
            break;
        }
        case 1: // Not a host name but a path
        {
            location = partPath;
            lastPosition = indx - 1;
            tag = 0;
            break;
        }
        case 2: // Oh it really is a host name
        {
            location = partServerForSure;
            lastPosition = indx;
            --indx;
            tag = 0;
            break;
        }
        }
    }
    }
    return true;
}

bool BuildSession::extractServerInfos(const String::value_type c)
{
    switch (c)
    {
    case '@': // We were actually deal with a user name
    {
        infos.user.append(str, indx - lastPosition, lastPosition);
        if (infos.user.empty())
            return false;
        lastPosition = indx + 1;
        tag = 0;
        break;
    }
    case '[': // Begining of an IPv6 address
    {
        if (tag) // only one bracket is allowed
            return false;
        tag = 1; // A tag has been found
        break;
    }
    case ']': // Ending of an IPv6 address
    {
        if (1 != tag) // must begin by a bracket
            return false;
        // New state : a tag has been found and has been closed
        tag = 2;
        break;
    }
    case '/': // Begining of a path
    {
        if (tag != 1)
        {
            // We are not inside of an IPv6 address
            infos.server.append(str, indx - lastPosition, lastPosition);
            if (infos.server.empty())
                return false;
            lastPosition = indx;
            location = partPath;
        }
        else
            // Forbidden [::/] is not a valid IPv6 address for sure
            return false;
        break;
    }
    case ':': // Port value or IPv6 address ?
    {
        if (tag != 1)
        {
            // We are not inside of an IPv6 address
            // Thus it is the begining of a port value
            infos.server.append(str, indx - lastPosition, lastPosition);
            if (infos.server.empty())
                return false;
            lastPosition = indx + 1;
            location = partPort;
        }
        break;
    }
    case '?': // Query ?
    {
        if (tag != 1)
        {
            // We are not inside of an IPv6 address
            // It is the begining of a query
            infos.server.append(str, indx - lastPosition, lastPosition);
            if (infos.server.empty(), lastPosition)
                return false;
            lastPosition = indx + 1;
            location = partQuery;
        }
        else
            // forbidden  [::?] is not a valid IPv6 address for sure
            return false;
        break;
    }
    case '#': // Fragment ?
    {
        if (tag != 1)
        {
            // We are not inside of an IPv6 address
            // It is the begining of a fragment
            infos.server.append(str, indx - lastPosition, lastPosition);
            if (infos.server.empty())
                return false;
            lastPosition = indx + 1;
            location = partFragment;
        }
        else
            // forbidden  [::#] is not a valid IPv6 address for sure
            return false;
        break;
    }
    }
    return true;
}

bool BuildSession::extractPort(const String::value_type c)
{
    switch (c)
    {
    case '@': // We may deal with a password and the server was actually a user name
    {
        if (infos.user.empty() and infos.password.empty())
        {
            // Oups... Actually it was not a port but a password...
            infos.user = infos.server;
            infos.password.append(str, indx - lastPosition, lastPosition);
            infos.server.clear();
            tag = 0;
            location = partServerForSure;
            lastPosition = indx + 1;
            return true;
        }
        return false;
        break;
    }
    case '/': // Begining of a path
    {
        ShortString16 intstr(str, lastPosition, indx - lastPosition);
        if (not intstr.to(infos.port))
        {
            // May be a invalid sequence
            if (indx - lastPosition != 1 or '\0' != str[lastPosition])
                return false;
        }
        lastPosition = indx;
        location = partPath;
        break;
    }
    case '#': // Begining of a fragment
    {
        ShortString16 intstr(str, lastPosition, indx - lastPosition);
        if (not intstr.to(infos.port))
        {
            // May be a invalid sequence
            if (indx - lastPosition != 1 or '\0' != str[lastPosition])
                return false;
        }
        lastPosition = indx + 1;
        location = partFragment;
        break;
    }
    case '?': // Begining of a query
    {
        ShortString16 intstr(str, lastPosition, indx - lastPosition);
        if (not intstr.to(infos.port))
        {
            // May be a invalid sequence
            if (indx - lastPosition != 1 or '\0' != str[lastPosition])
                return false;
        }
        lastPosition = indx + 1;
        location = partQuery;
        break;
    }
    }
    return true;
}

bool BuildSession::extractPath(const String::value_type c)
{
    // All chars are allowed, except for `?` and `#` which are reserved chars
    switch (c)
    {
    case '?': // Begining of a query
    {
        infos.path.append(str, indx - lastPosition, lastPosition);
        location = partQuery;
        lastPosition = indx + 1;
        break;
    }
    case '#': // Begining of a fragment
    {
        infos.path.append(str, indx - lastPosition, lastPosition);
        location = partFragment;
        lastPosition = indx + 1;
        break;
    }
    case '.':
    {
        pMustRemoveDotSegments = true;
        break;
    }
    }
    return true;
}

inline bool BuildSession::extractQuery(const String::value_type c)
{
    // `?` and `/` are valid in a query
    // Example :
    //  http://www.example.org/redirect.php?url=http://www.foo.org/index.php?anotherredirect=1
    switch (c)
    {
    case '#': // Begining of a fragment
    {
        infos.query.append(str, indx - lastPosition, lastPosition);
        location = partFragment;
        lastPosition = indx + 1;
        break;
    }
    }
    return true;
}

inline bool BuildSession::extractFragment()
{
    // A fragment is composed by the end of the URI
    // Directly go to the end of the string, the routine postflight() will
    // finish the job for us
    indx = (uint)str.size();
    return true;
}

void BuildSession::run()
{
    // Foreach char in the string...
    for (; indx < str.size(); ++indx)
    {
        const String::value_type c = str[indx];
        switch (location)
        {
        case partPath:
        {
            if (not this->extractPath(c))
                return;
            break;
        }
        case partQuery:
        {
            if (not this->extractQuery(c))
                return;
            break;
        }
        case partScheme:
        {
            if (not this->extractScheme(c))
                return;
            break;
        }
        case partServer:
        {
            if (not this->extractAuthorty(c))
                return;
            break;
        }
        case partServerForSure:
        {
            if (not this->extractServerInfos(c))
                return;
            break;
        }
        case partPort:
        {
            if (not this->extractPort(c))
                return;
            break;
        }
        case partFragment:
        {
            if (not this->extractFragment())
                return;
            break;
        }
        default:
            return; // We should not be in the middle of nowhere

        } // switch location
    }     // for each char

    // Postflight : Some content have not been copied yet
    infos.isValid = postFlight();
}

} // anonymous namespace

Uri::Uri()
{
}

Uri::Uri(const Uri& rhs) : pInfos(rhs.pInfos)
{
}

void Uri::extractURIFromString(const AnyString& raw)
{
    // Cleanup before anything
    clear();

    // trim the string for unwanted and useless char
    // making a copy for the helper class
    String copy = raw;
    copy.trim();

    // Go ahead !
    if (not copy.empty())
        BuildSession(pInfos, copy).run();
}

void Uri::defaultPathIfEmpty()
{
    if (pInfos.path.empty())
        pInfos.path = '/';
}

void Uri::port(int p)
{
    if (p > 0 or p == INT_MIN)
    {
        pInfos.port = p;
        pInfos.isValid = true;
    }
}

void Uri::password(const AnyString& newpassword)
{
    pInfos.password = newpassword;
    pInfos.isValid = true;
}

void Uri::server(const AnyString& newserver)
{
    pInfos.server = newserver;
    pInfos.isValid = true;
}

void Uri::scheme(const AnyString& newscheme)
{
    pInfos.scheme = newscheme;
    pInfos.isValid = true;
}

void Uri::user(const AnyString& newuser)
{
    pInfos.user = newuser;
    pInfos.isValid = true;
}

void Uri::path(const AnyString& newpath)
{
    pInfos.path = newpath;
    pInfos.isValid = true;
}

void Uri::query(const AnyString& newquery)
{
    pInfos.query = newquery;
    pInfos.isValid = true;
}

void Uri::fragment(const AnyString& newfragment)
{
    pInfos.fragment = newfragment;
    pInfos.isValid = true;
}

std::ostream& Uri::print(std::ostream& out) const
{
    pInfos.print(out);
    return out;
}

} // namespace Yuni
