
###############################################################################
# Profile
#  Everything related to this particular profile
###############################################################################

# Profile name
# FIXME: Document usage.
set(YUNI_PROFILE_NAME "Default")




###############################################################################
# Environment
#  Compilers, program search paths, everything about your system.
###############################################################################

# YUNI_MACPORTS_PREFIX
# Mac-specific: MacPorts path. When you specify "macports" in a package search,
# this prefix is used.
#
# Default: Prefix of the first «port» program found in PATH.
# Should be /opt/local in the most common case.
#
#set(YUNI_MACPORTS_PREFIX "/my/twisted/macports/prefix")
set(YUNI_MACPORTS_PREFIX "/opt/local/")



###############################################################################
# Target
#  What is produced, and everything related to it.
###############################################################################

# YUNI_TARGET
# Specifies the compilation profile. Accepted values are:
# - debug: No optimization, debug info, very slow.
# - release: O3 optimization level, no debug, production code.
#
# Default: debug
#
#set(YUNI_TARGET "debug")


# MODULES
# Specifies which Yuni components should be build.
# Uncomment the following line to override the module list
# Example : `messaging,audio,-uuid`
# Note : `cmake -DMODULES=help` to have a list of all available modules
#set(MODULES "messaging,audio")



###############################################################################
# External packages
#
# A module often requires one or more external packages (`lua` for example).
# Most of the time the system has its own package management utility, which
# will provide all needed and up-to-date packages (`lua`, `libxml`...).
# It is not always the case (Windows for example), so some pre-built packages
# (DevPacks) are available on http://devpacks.libyuni.org and can be
# automatically downloaded.
#
# Several modes can be given in the preferred order to find and use the
# appropriate package. If nothing suits your needs, it is possible to use the
# `custom` mode and to set the prefix path where the package can be found.
# This is useful if you have special needs or wish to integrate Yuni in an
# software that already uses the package.
#
# Generic modes :
#
#    auto    : Automatically choose a mode depending on platform
#
#    disabled: Do as if the particular package could not be found.
#              Implemented only for certain packages providing optional support.
#
#    system  : Try to use the standard way to find the package provided by the system
#              Standard paths, System Frameworks (OS X)
#
#    custom  : Try to find the package from a custom prefix path
#              The variable `YUNI_DvP_<pkg>_PREFIX` must be set.
#              Other variables may need to be set, on a per-package
#              basis
#
#    macport : Try to find the package from a macport installation (Mac OS X)
#              (http://www.macports.org)
#
#    devpack : Download and use the pre-build package from `devpacks.libyuni.org`
#              This is the recommended way on Windows, and when you want to have
#              a known-working build.
#
# NOTE: Each package is not required to support every mode. Available modes are speci
#
# Example :
# Use `lua` compiled by hand, installed in `/opt/lua` (/opt/lua/include and `/opt/lua/lib`) :
#   Set(YUNI_DvP_LUA_MODE custom)
#   Set(YUNI_DvP_LUA_PREFIX "/opt/lua")
#
###############################################################################


## Audio

# ZLIB [FIXME: NOT IMPLEMENTED]
# Provides: GZip compression support
# Modes: system, custom
# Required for Audio module.

# OpenAL
# Provides: Audio abstraction layer.
# Modes: system, devpack, custom
# Required for Audio module.
set(YUNI_DvP_OPENAL_MODE      auto)


## Scripts

# Lua (+script,+lua)
# Provides: Lua scripting support
# Modes: devpack, system, macports, custom
# Optional.
set(YUNI_DvP_LUA_MODE         auto)



###############################################################################
# Platform-specific options
###############################################################################

# For building universal binaries on OS X
# Value by default : i686;x86_64
#set(YUNI_MACOX_UNIVERSAL_BINARIES  "ppc;ppc64;i386;x86_64")


###############################################################################
# Miscellaneous build options
#  Options that concern small features, tweaks or optimisations
###############################################################################

# Auto-Compile yuni-config from CMake
# Enable this option to automatically compile yuni-config from CMake
# FIXME: Describe when this could be useful.
#set(YUNI_AUTO_COMPILE_YUNI_CONFIG  false)


# Special instructions sets
# The following options enable or disable certain CPU optimisations.
# If you have to run on platforms that do not support certain options,
# we recommend to disable them.
# Options:
#  auto  : Auto detect if these sets can be used (based on build host)
#  no    : Completely disable it
#  yes   : Try to use it anyways (it may not even compile, though)
#
set(YUNI_PROFILE_MMX       auto)    # MMX
set(YUNI_PROFILE_3DNOW     auto)    # 3DNow!
set(YUNI_PROFILE_SSE       auto)    # SSE
set(YUNI_PROFILE_SSE2      auto)    # SSE2 (depends on SSE)
set(YUNI_PROFILE_SSE3      auto)    # SSE3 (depends on SSE2)
set(YUNI_PROFILE_SSE4      auto)    # SSE4 (depends on SSE3)
set(YUNI_PROFILE_SSE4a     auto)    # SSE4a (depends on SSE3)
set(YUNI_PROFILE_SSE4_1    auto)    # SSE 4.1 (depends on SSE3)
set(YUNI_PROFILE_SSE4_2    auto)    # SSE 4.2 (depends on SSE3)


# yuni-config
# Enable this option to skip the build of yuni-config
set(YUNI_SKIP_YUNI_CONFIG  false)

# Custom C/C++ Flags
#
# Advanced users only : it may not a good idea to use your own CFlags
# for compiling the Yuni library. Do not file any bug reports before re-testing
# with standard build-flags.

# Uncomment the following line to ADD some C++ compiler flags
#set(YUNI_CXX_FLAGS_OVERRIDE_ADD_DEBUG          "-Wextra")
#set(YUNI_CXX_FLAGS_OVERRIDE_ADD_RELEASE        "-Wextra")
#set(YUNI_CXX_FLAGS_OVERRIDE_ADD_RELWITHDEBINFO "-Wextra")

# Uncomment the following line to OVERRIDE the C++ compiler flags
# This is not recommended.
#set(YUNI_CXX_FLAGS_OVERRIDE_DEBUG          "-g -ggdb -Wall -Wextra")
#set(YUNI_CXX_FLAGS_OVERRIDE_RELEASE        "-g -ggdb -Wall -Wextra")
#set(YUNI_CXX_FLAGS_OVERRIDE_RELWITHDEBINFO "-g -ggdb -Wall -Wextra")


# That's all, folks ! #########################################################
