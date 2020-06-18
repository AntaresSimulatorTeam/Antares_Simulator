

##############
##  OpenSSL  ##
##############
if(BUILD_OPENSSL)

#TODO : get architecture for windows (can propably be done in a proper way)
if(WIN32 OR WIN64)
	if(ANTARES_x86_64)
		set(OPENSSL_ARCH VC-WIN64A)
	else()			
		set(OPENSSL_ARCH VC-WIN32)
	endif()
endif()

build_git_dependency(
    NAME
      OpenSSL
    REPOSITORY
      "https://github.com/openssl/openssl.git"
    TAG
      "OpenSSL_1_1_1g "
	CONFIGURE_COMMAND
	   "perl <SOURCE_DIR>/Configure ${OPENSSL_ARCH} no-asm no-shared --prefix=${CMAKE_CURRENT_BINARY_DIR}/install/ --openssldir=${CMAKE_CURRENT_BINARY_DIR}/install/"
	BUILD_COMMAND
	   "nmake"
	INSTALL_COMMAND
	   "nmake install_sw"	   
  )
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
  
  # Define wxWidgets_ROOT_DIR
  set(wxWidgets_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/dependencies/install)
  
endif()


