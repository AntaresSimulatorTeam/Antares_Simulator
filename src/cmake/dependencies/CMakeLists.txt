##############
##  OpenSSL  ##
##############
if(BUILD_OPENSSL)

#TODO : get architecture for windows (can propably be done in a better way)
if(WIN32 OR WIN64)
	if(ANTARES_x86_64)
		set(OPENSSL_ARCH VC-WIN64A)
	else()			
		set(OPENSSL_ARCH VC-WIN32)
	endif()
	
	set(CONFIGURE_CALL "perl <SOURCE_DIR>/Configure")

else()
	set(CONFIGURE_CALL "config")	
endif()

build_git_dependency(
    NAME
      OpenSSL
    REPOSITORY
      "https://github.com/openssl/openssl.git"
    TAG
      "OpenSSL_1_1_1g "
	CONFIGURE_COMMAND
	   "${CONFIGURE_CALL} ${OPENSSL_ARCH} no-asm no-shared --prefix=${CMAKE_CURRENT_BINARY_DIR}/install/ --openssldir=${CMAKE_CURRENT_BINARY_DIR}/install/"
	BUILD_COMMAND
	   "nmake"
	INSTALL_COMMAND
	   "nmake install_sw"	   
  )
endif()

##############
##  curl  ##
##############
if(BUILD_curl)


if(WIN32 OR WIN64)
	if(ANTARES_x86_64)
		set(OPENSSL_ARCH VC-WIN64A)
	else()			
		set(OPENSSL_ARCH VC-WIN32)
	endif()
	
	set(CONFIGURE_CALL "call <SOURCE_DIR>/buildconf.bat")
	
	#Need to create curl include install dir for correct installation
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/install/include/curl")
	
	#TODO : define VC version for now hard coded with VC=14
	#TODO : see for better WITH_PREFIX use

	#Remarks : WITH_PREFIX compile option must use \ (backslash). Need to add multiple backslash because two backslashes (\\1) are required in CMake code to get a backslash through argument parsing
	#We need 4 backslash because the BUILD_COMMAND option is passed to build_git_dependency then to another function inside

	set(BUILD_CALL "cd <SOURCE_DIR>/winbuild COMMAND nmake Makefile.vc mode=static VC=14 WITH_SSL=static WITH_DEVEL=${CMAKE_CURRENT_BINARY_DIR}/install/ WITH_PREFIX=..\\\\..\\\\..\\\\install ENABLE_SSPI=no ENABLE_IDN=no ENABLE_WINSSL=no GEN_PDB=no DEBUG=no MACHINE=x64")
	
	#No install command, done with WITH_PREFIX compile option
	set(INSTALL_CALL "echo libcurl installation done with WITH_PREFIX compile option")
	
else()
	set(CONFIGURE_CALL 	"configure")	
	set(BUILD_CALL 		"make")	
	set(INSTALL_CALL 	"make_install")
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
  )
  
  #Need to rename libcurl_a.lib to libcurl.lib for find_package(CURL)
  file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/install/lib/libcurl_a.lib ${CMAKE_CURRENT_BINARY_DIR}/install/lib/libcurl.lib)
  
endif()

##############
##  wxWidgets  ##
##############
if(BUILD_wxWidgets)
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


