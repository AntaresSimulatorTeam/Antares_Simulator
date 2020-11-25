##############
##  OpenSSL  ##
##############
if(BUILD_OPENSSL)

#Check if OpenSSL is not already available
find_package(OpenSSL)

if (NOT OPENSSL_FOUND)

#TODO : get architecture for windows (can propably be done in a better way)
if(WIN32 OR WIN64)
	if(ANTARES_x86_64)
		set(OPENSSL_ARCH VC-WIN64A)
	else()			
		set(OPENSSL_ARCH VC-WIN32)
	endif()
	
	set(CONFIGURE_CALL "perl <SOURCE_DIR>/Configure")
    set(MAKE_COMMAND "nmake")

else()
	set(CONFIGURE_CALL "sh <SOURCE_DIR>/config")
    set(MAKE_COMMAND "make")
endif()

build_git_dependency(
    NAME
      OpenSSL
    REPOSITORY
      "https://github.com/openssl/openssl.git"
    TAG
      "OpenSSL_1_1_1g"
	CONFIGURE_COMMAND
	   "${CONFIGURE_CALL} ${OPENSSL_ARCH} no-asm no-shared --prefix=${DEPS_INSTALL_DIR} --openssldir=${DEPS_INSTALL_DIR}"
	BUILD_COMMAND
	   "${MAKE_COMMAND}"
	INSTALL_COMMAND
	   "${MAKE_COMMAND}  install_sw"	   
  )
  
endif()

endif()

##############
##  curl  ##
##############
if(BUILD_curl)

#Check if CURL is not already available
find_package(CURL)

if (NOT CURL_FOUND)

if(WIN32 OR WIN64)
	if(ANTARES_x86_64)
		set(OPENSSL_ARCH VC-WIN64A)
	else()			
		set(OPENSSL_ARCH VC-WIN32)
	endif()
	
	set(CONFIGURE_CALL "call <SOURCE_DIR>/buildconf.bat")
	
	#Need to create curl include install dir for correct installation
	file(MAKE_DIRECTORY "${DEPS_INSTALL_DIR}/include/curl")
	
	#TODO : define VC version, at the moment it is hard coded with VC=14
	#TODO : see for better WITH_PREFIX use

	#Remarks : WITH_PREFIX compile option must use \ (backslash). Need to add multiple backslash because two backslashes (\\1) are required in CMake code to get a backslash through argument parsing
	#We need 4 backslash because the BUILD_COMMAND option is passed to build_git_dependency then to another function inside:
    string(REPLACE "/" "\\\\" DEPS_INSTALL_DIR_BACKSLASH ${DEPS_INSTALL_DIR})
	set(BUILD_CALL "cd <SOURCE_DIR>/winbuild COMMAND nmake Makefile.vc mode=static VC=14 WITH_SSL=static WITH_DEVEL=${DEPS_INSTALL_DIR} WITH_PREFIX=${DEPS_INSTALL_DIR_BACKSLASH} ENABLE_SSPI=no ENABLE_IDN=no ENABLE_WINSSL=no GEN_PDB=no DEBUG=no MACHINE=x64")

	#No install command; the install is done using WITH_PREFIX compile option
	set(INSTALL_CALL "echo libcurl installation is done with WITH_PREFIX compile option")
	
else()
	set(CONFIGURE_CALL 	"sh <SOURCE_DIR>/buildconf COMMAND <SOURCE_DIR>/configure --disable-shared --enable-static --without-libidn --prefix=${DEPS_INSTALL_DIR}")	
	set(BUILD_CALL 		"${CMAKE_MAKE_PROGRAM}")	
	set(INSTALL_CALL 	"${CMAKE_MAKE_PROGRAM} install")
endif()


build_git_dependency(
    NAME
      curl
    REPOSITORY
      "https://github.com/curl/curl.git"
    TAG
      "curl-7_68_0"
	CONFIGURE_COMMAND
	   "${CONFIGURE_CALL}"
	BUILD_COMMAND
	   "${BUILD_CALL}"	   
	INSTALL_COMMAND
	   "${INSTALL_CALL}"
    INSOURCE_BUILD ON
  )
  
if(WIN32 OR WIN64)
    #We need to rename libcurl_a.lib to libcurl.lib for find_package(CURL)
    file(RENAME ${DEPS_INSTALL_DIR}/lib/libcurl_a.lib ${DEPS_INSTALL_DIR}/lib/libcurl.lib)
endif()

  
endif()
  
endif()

##############
##  wxWidgets  ##
##############
if(BUILD_wxWidgets)

#Check if wxWidgets is not already available
find_package(wxWidgets)

if (NOT wxWidgets_FOUND)

  build_git_dependency(
    NAME
      wxWidgets
    REPOSITORY
      "https://github.com/wxWidgets/wxWidgets.git"
    TAG
      "v3.1.3"
    CMAKE_ARGS
      -DwxBUILD_SHARED=OFF
  ) 
  
endif()

endif()

#####################
##  Sirius solver  ##
#####################
if(BUILD_sirius)

#Check if sirius is already available
find_package(sirius_solver)

if (NOT sirius_solver_FOUND)
build_git_dependency(
    NAME
      sirius
    REPOSITORY
      "https://github.com/AntaresSimulatorTeam/sirius-solver.git"
    TAG
      "Antares_VCPKG"
    SOURCE_SUBDIR
      "src"
  )
endif()
  
endif()

#####################
##  ORTOOLS  ##
#####################
if(BUILD_ortools)

#Check if ortools is already available
find_package(ortools)

if(NOT ortools_FOUND)

set(REPOSITORY "https://github.com/AntaresSimulatorTeam/or-tools.git")
set(TAG "rte_dev_sirius")
set(CMAKE_ARGS "-DUSE_SIRIUS=ON")

if (USE_ORTOOLS_STABLE)
	set(REPOSITORY "https://github.com/google/or-tools.git")
	set(TAG "v7.8")
	set(CMAKE_ARGS "")
endif()

build_git_dependency(
    NAME
      ortools
    REPOSITORY
      ${REPOSITORY}
    TAG
      ${TAG}
    CMAKE_ARGS
      "${CMAKE_ARGS} -DUSE_COINOR=ON -DBUILD_PYTHON=OFF -DBUILD_TESTING=OFF -DBUILD_DEPS=ON"
  )
endif()

endif()



