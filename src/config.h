#ifndef __ANTARES_CONFIG_H__
# define __ANTARES_CONFIG_H__


//! The Hi version
# define ANTARES_VERSION_HI           8
//! The Lo version
# define ANTARES_VERSION_LO           8
//! Build (Revision) of Antares
# define ANTARES_VERSION_BUILD        3
//! Canonical version
# define ANTARES_VERSION              "8.8"
//! Date of publication
# define ANTARES_VERSION_YEAR         2024

//! Version in CString format
# define ANTARES_VERSION_STR          "8.8.3"

//! Version + Publisher
# define ANTARES_VERSION_PUB_STR      "8.8.3 (RTE France)"


# define ANTARES_VERSION_BUILD_DATE   __DATE__

//! The Publisher
# define ANTARES_PUBLISHER            "RTE France"
//! The Website for RTE
# define ANTARES_WEBSITE              "https://antares-simulator.org/"
//! URL for the online documentation
#define ANTARES_ONLINE_DOC            "https://antares-simulator.readthedocs.io/"

// ----------------------------------------------------------------------------

//! Beta version
/* #undef ANTARES_BETA */

//! RC version
/* #undef ANTARES_RC */

//! git revision (SHA-1)
#define GIT_SHA1_SHORT_STRING "7be66dc-dirty"

#endif // __ANTARES_CONFIG_H__

