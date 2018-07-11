#ifndef __ANTARES_CONFIG_H__
# define __ANTARES_CONFIG_H__


//! The Hi version
# define ANTARES_VERSION_HI           @ANTARES_VERSION_HI@
//! The Lo version
# define ANTARES_VERSION_LO           @ANTARES_VERSION_LO@
//! Build (Revision) of Antares
# define ANTARES_VERSION_BUILD        @ANTARES_VERSION_REVISION@
//! Canonical version
# define ANTARES_VERSION              "@ANTARES_VERSION_HI@.@ANTARES_VERSION_LO@"
//! Date of publication
# define ANTARES_VERSION_YEAR         @ANTARES_VERSION_YEAR@

//! Version in CString format
# define ANTARES_VERSION_STR          "@ANTARES_VERSION_HI@.@ANTARES_VERSION_LO@.@ANTARES_VERSION_REVISION@"

//! Version + Publisher
# define ANTARES_VERSION_PUB_STR      "@ANTARES_VERSION_HI@.@ANTARES_VERSION_LO@.@ANTARES_VERSION_REVISION@ (@ANTARES_PUBLISHER@)"


# define ANTARES_VERSION_BUILD_DATE   __DATE__

//! The Publisher
# define ANTARES_PUBLISHER            "@ANTARES_PUBLISHER@"
//! The Website for RTE
# define ANTARES_WEBSITE              "@ANTARES_WEBSITE@"

# define								ANTARES_PROXY_PARAMETERS_FILENAME	"ProxyConnexionParameters.ini"

# define								ANTARES_SERVER_SCRIPT_ROOT				"/check/"

# define								ANTARES_CA_BUNDLE					"ca-bundle.crt"
// ----------------------------------------------------------------------------

//! Beta version
#cmakedefine ANTARES_BETA     @ANTARES_BETA@

//! RC version
#cmakedefine ANTARES_RC     @ANTARES_RC@

#endif // __ANTARES_CONFIG_H__

