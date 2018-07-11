


#
# wxWidgets
#
if(WIN32)
	#
	# Specific setup
	#

	if("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
		set(WX_BUILD_STRING "release")
	else()
		set(WX_BUILD_STRING "debug")
		set(WX_BUILD_TYPE "d")
	endif()
	if(NOT "${ANTARES_x86_64}" STREQUAL "")
		set(WX_ARCH_TYPE "_x64")
		#set(WX_ARCH_TYPE "_x64")
	else()
		set(WX_ARCH_TYPE "")
	endif()

	#set(WX_ROOT_PATH  "C:/projects/wxwidgets/3.1/${COMPILER_LIB_INCLUDE}-${WX_BUILD_TYPE}-${WX_ARCH_TYPE}")
	set(WX_ROOT_PATH "${ANTARES_WX_ROOT_PATH}")
	set(WX_VERSION ${ANTARES_WX_VERSION})
	
	#
	# Generic setup
	#
	
	set(wxWidgets_BASEDIR           "${WX_ROOT_PATH}")
	set(wxWidgets_LIBRARIES_DIRS	"${wxWidgets_BASEDIR}/lib/${COMPILER_LIB_INCLUDE}${WX_ARCH_TYPE}_lib")

	set(wxWidgets_INCLUDE_DIRS 	"${wxWidgets_BASEDIR}/include"
								"${wxWidgets_LIBRARIES_DIRS}/mswu${WX_BUILD_TYPE}")
							
	OMESSAGE("${wxWidgets_BASEDIR}")							
	OMESSAGE("${wxWidgets_LIBRARIES_DIRS}")
	OMESSAGE("${wxWidgets_INCLUDE_DIRS}")
	
	set(wxWidgets_LIBRARIES
				"wxbase${WX_VERSION}u${WX_BUILD_TYPE}"
				"wxmsw${WX_VERSION}u${WX_BUILD_TYPE}_core"
				"wxmsw${WX_VERSION}u${WX_BUILD_TYPE}_aui"
				"wxmsw${WX_VERSION}u${WX_BUILD_TYPE}_adv"
				"wxmsw${WX_VERSION}u${WX_BUILD_TYPE}_richtext"
				"wxmsw${WX_VERSION}u${WX_BUILD_TYPE}_html"
				"wxbase${WX_VERSION}u${WX_BUILD_TYPE}_xml"
				"wxmsw${WX_VERSION}u${WX_BUILD_TYPE}_propgrid"
				wxjpeg${WX_BUILD_TYPE}
				wxpng${WX_BUILD_TYPE}
				wxregexu${WX_BUILD_TYPE}
				wxtiff${WX_BUILD_TYPE}
				wxzlib${WX_BUILD_TYPE}
				wxexpat${WX_BUILD_TYPE}
				# Windows related libraries
				kernel32 user32 gdi32 comdlg32 winspool winmm shell32
				comctl32 ole32 oleaut32 uuid rpcrt4 advapi32
				wsock32 odbc32)							
	
	if("${CMAKE_BUILD_TYPE}" STREQUAL "release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "tuning")
		#set(wxWidgets_INCLUDE_DIRS 	"${wxWidgets_BASEDIR}/include"
		#							"${wxWidgets_BASEDIR}/lib/vc_lib/mswu")
		set(wxWidgets_DEFINITIONS	"-D_UNICODE"
									"-DHAVE_WIN32API_H"
									"-D__WXMSW__" "-D_WINDOWS"
									"-DwxUSE_GUI"
									"-DwxUSE_THREADS"
									"-DwxUSE_BASE=1")
		# set(wxWidgets_LIBRARIES
				# wxbase30u
				# wxmsw30u_core
				# wxmsw30u_aui
				# wxmsw30u_adv
				# wxmsw30u_richtext
				# wxmsw30u_html
				# wxbase30u_xml
				# wxmsw30u_propgrid
				# wxjpeg
				# wxpng
				# wxregexu
				# wxtiff
				# wxzlib
				# wxexpat
				# # Windows related libraries
				# kernel32 user32 gdi32 comdlg32 winspool winmm shell32
				# comctl32 ole32 oleaut32 uuid rpcrt4 advapi32
				# wsock32 odbc32)

	else()

		#set(wxWidgets_INCLUDE_DIRS 	"${wxWidgets_BASEDIR}/include"
		#							"${wxWidgets_BASEDIR}/lib/vc_lib/mswud")
		Set(wxWidgets_DEFINITIONS	"-D_UNICODE"
									"-DHAVE_WIN32API_H"
									"-D__WXMSW__" "-D_WINDOWS" "-D_DEBUG"
									"-DwxUSE_GUI"
									"-DwxUSE_BASE=1"
									"-DwxUSE_THREADS"
									"-D__WXDEBUG__")
		# set(wxWidgets_LIBRARIES
				# wxbase30ud
				# wxmsw30ud_core
				# wxmsw30ud_aui
				# wxmsw30ud_adv
				# wxmsw30ud_richtext
				# wxmsw30ud_html
				# wxbase30ud_xml
				# wxmsw30ud_propgrid
				# wxjpegd
				# wxpngd
				# wxregexud
				# wxtiffd
				# wxzlibd
				# wxexpatd
				# # Windows related libraries
				# kernel32 user32 gdi32 comdlg32 winspool winmm shell32
				# comctl32 ole32 oleaut32 uuid rpcrt4 advapi32
				# wsock32 odbc32)

	endif()

	if(NOT "${IS_64BITS}" STREQUAL "")
		set(wxWidgets_DEFINITIONS	${wxWidgets_DEFINITIONS} "-DWIN64" "-D_M_X64")
	else()
		set(wxWidgets_DEFINITIONS	${wxWidgets_DEFINITIONS} "-DWIN32")
	endif()
	link_directories("${wxWidgets_LIBRARIES_DIRS}")

	# wxWidgets include directory
	include_directories(${wxWidgets_INCLUDE_DIRS})

	foreach(d ${wxWidgets_DEFINITIONS})
		string(STRIP "${d}" d)
		string(SUBSTRING "${d}" 0 2 p)
		if(NOT "${p}" STREQUAL "-D")
			add_definitions("-D${d}")
		else()
			add_definitions("${d}")
		endif()
	endforeach()

	#message(STATUS "wxWidgets: Incl: ${wxWidgets_INCLUDE_DIRS}")
	#message(STATUS "wxWidgets: Libs: ${wxWidgets_LIBRARIES_DIRS}")
	#message(STATUS "wxWidgets: Defs: ${wxWidgets_DEFINITIONS}")
	#message(STATUS "wxWidgets: Root path: ${WX_ROOT_PATH}")

else()
	set(wxWidgets_USE_UNICODE true)
	execute_process(COMMAND ${PROJECT_SOURCE_DIR}/ext/wxwidgets/build_gtk/wx-config --unicode=yes --cxxflags OUTPUT_VARIABLE WX_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND ${PROJECT_SOURCE_DIR}/ext/wxwidgets/build_gtk/wx-config --unicode=yes --libs std,richtext,propgrid,aui     OUTPUT_VARIABLE WX_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

	# Fixed some strange result from wx-config
	#string(REPLACE "-lwxregexu-3.1" "/local/partage/opt/lib/libwxregexu-3.1.a" WX_LIBS_FIX "${WX_LIBS}")
	#string(REGEX REPLACE "[^ ]+\\.a" "" WX_LIBS_CLEAN "${WX_LIBS_FIX}")
	#string(REGEX MATCHALL "[^ ]+\\.a" wxWidgets_LIBRARIES "${WX_LIBS_FIX}")

	#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${WX_LIBS_CLEAN}")
	#set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${WX_LIBS_CLEAN}")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${WX_CXXFLAGS}")
	set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${WX_CXXFLAGS}")

	set(wxWidgets_LIBRARIES "${WX_LIBS}")

endif()



# add wxWidgets definitions
add_definitions(-DXTHREADS -D_REENTRANT -DXUSE_MTSAFE_API)


if(ANTARES_GUI)
	add_definitions("-DWX_SUPPORT")
endif(ANTARES_GUI)

