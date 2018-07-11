YMESSAGE_MODULE("Core")

# Paths for yuni-config, embedded version.
LIBYUNI_CONFIG_INCLUDE_PATH("intree" "core" "${CMAKE_CURRENT_SOURCE_DIR}/..")
LIBYUNI_CONFIG_LIB_PATH("intree" "core" "${LIBRARY_OUTPUT_PATH}")

# Paths for yuni-config, installed version.
LIBYUNI_CONFIG_INCLUDE_PATH("target" "core" "${CMAKE_INSTALL_PREFIX}/include/${YUNI_VERSIONED_INST_PATH}/")
LIBYUNI_CONFIG_LIB_PATH("target" "core" "${CMAKE_INSTALL_PREFIX}/lib/${YUNI_VERSIONED_INST_PATH}/")

LIBYUNI_CONFIG_LIB("both" "core"       "yuni-static-core")


if(APPLE)
	LIBYUNI_CONFIG_CFLAG("both" "core"	"-fvisibility=hidden")
endif(APPLE)

if (NOT WIN32 AND NOT WIN64)
	LIBYUNI_CONFIG_DEFINITION("both" "core" "_FILE_OFFSET_BITS=64")
else()
	LIBYUNI_CONFIG_DEFINITION("both" "core" "UNICODE")
	LIBYUNI_CONFIG_DEFINITION("both" "core" "_UNICODE")
	LIBYUNI_CONFIG_DEFINITION("both" "core" "_USE_64BIT_TIME_T")
	if (YUNI_TARGET MATCHES "debug")
		LIBYUNI_CONFIG_CFLAG("both" "core" "-ggdb3")
	endif()
endif()
LIBYUNI_CONFIG_DEFINITION("both" "core" "_REENTRANT")

if (MINGW)
	LIBYUNI_CONFIG_CFLAG("both" "core" "-mthreads")
	if (YUNI_TARGET MATCHES "debug")
		LIBYUNI_CONFIG_CFLAG("both" "core" "-ggdb3")
	endif()
endif()



set(CMAKE_REQUIRED_FLAGS_PREVIOUS ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS "")

include("${CMAKE_CURRENT_LIST_DIR}/check-includes.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/check-cxx11.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/check-stdint.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/check-extras.cmake")

set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_PREVIOUS})













if (NOT WIN32)
	find_package(Threads REQUIRED)
	link_libraries(${CMAKE_THREAD_LIBS_INIT})
	LIBYUNI_CONFIG_LIB_RAW_COMMAND("both" "core" "${CMAKE_THREAD_LIBS_INIT}")
endif()

#
# Generating informations about the platform
#
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/platform.cmake ${CMAKE_CURRENT_SOURCE_DIR}/platform.h)




# System - Microsoft Windows
if (MSVC)
	set(yuni_additional_os_specific
		core/system/gettimeofday.cpp
		core/system/username.cpp
		core/system/windows/msinttypes/inttypes.h
		core/system/windows/msinttypes/stdint.h
		core/system/windows.hdr.h)
else()
	set(yuni_additional_os_specific "")
endif()




add_library(yuni-static-core STATIC
		yuni.h

		core/any/any.cpp
		core/any/any.h
		core/any/any.hxx
		core/any/any.private.h
		core/any.h
		core/atomic/bool.h
		core/atomic/int.h
		core/atomic/int.hxx
		core/atomic/string.hxx
		core/atomic/traits.h
		core/bind/bind.h
		core/bind/bind.hxx
		core/bind/traits.h
		core/bind/fwd.h
		core/bind.h
		core/bit/array.cpp
		core/bit/array.h
		core/bit/array.hxx
		core/bit/bit.h
		core/bit/bit.hxx
		core/bit.h
		core/boundingbox.h
		#core/charset/charset.cpp
		#core/charset/charset.h
		#core/charset/charset.hxx
		#core/charset.h
		core/color/fwd.h
		core/color/rgb.h
		core/color/rgb.hxx
		core/color/rgba.h
		core/color/rgba.hxx
		core/dictionary.h
		core/dynamiclibrary/file.cpp
		core/dynamiclibrary/file.h
		core/dynamiclibrary/file.hxx
		core/dynamiclibrary/symbol.h
		core/dynamiclibrary/symbol.hxx
		core/dynamiclibrary.h
		core/event/event.h
		core/event/event.hxx
		core/event/flow/continuous.h
		core/event/flow/timer.h
		core/event/interfaces.h
		core/event/interfaces.hxx
		core/event/loop.cpp
		core/event/loop.fwd.h
		core/event/loop.h
		core/event/loop.hxx
		core/event/observer/item.h
		core/event/observer/item.hxx
		core/event/observer/observer.h
		core/event/observer/observer.hxx
		core/event/statistics/none.h
		core/event/traits.h
		core/event.h
		core/exceptions/badcast.h
		core/exceptions.h
		core/foreach.h
		core/functional/fold.h
		core/functional/fold.hxx
		core/fwd.h
		core/getopt/option.cpp
		core/getopt/option.h
		core/getopt/parser.cpp
		core/getopt/parser.h
		core/getopt/parser.hxx
		core/getopt.h
		core/hash
		core/hash/checksum/checksum.h
		core/hash/checksum/checksum.hxx
		core/hash/checksum/md5.cpp
		core/hash/checksum/md5.h
		core/hash/checksum/md5.hxx
		core/hash/table/table.h
		core/hash/table/table.hxx
		core/hash/table.h
		core/iterator/iterator.h
		core/iterator/iterator.hxx
		core/iterator.h
		core/logs/buffer.h
		core/logs/decorators/applicationname.h
		core/logs/decorators/message.h
		core/logs/decorators/time.cpp
		core/logs/decorators/time.h
		core/logs/decorators/verbositylevel.h
		core/logs/handler/callback.h
		core/logs/handler/file.h
		core/logs/handler/file.hxx
		core/logs/handler/stdcout.h
		core/logs/logs.h
		core/logs/logs.hxx
		core/logs/null.h
		core/logs/verbosity.h
		core/logs.h
		core/math
		core/math/base.h
		core/math/complex.h
		core/math/complex.hxx
		core/math/distance.h
		core/math/distance.hxx
		core/math/exponential.h
		core/math/exponential.hxx
		core/math/geometry/boundingbox.h
		core/math/geometry/boundingbox.hxx
		core/math/geometry/edge.h
		core/math/geometry/point2D.h
		core/math/geometry/point2D.hxx
		core/math/geometry/point3D.h
		core/math/geometry/quaternion.h
		core/math/geometry/quaternion.hxx
		core/math/geometry/rect2D.h
		core/math/geometry/rect2D.hxx
		core/math/geometry/triangle.h
		core/math/geometry/vector3D.h
		core/math/geometry/vector3D.hxx
		core/math/geometry/vertex.h
		core/math/geometry.h
		core/math/geometry.hxx
		core/math/log.h
		core/math/log.hxx
		core/math/math.h
		core/math/math.hxx
		core/math/msvc.hxx
		core/math/random/constant.h
		core/math/random/default.h
		core/math/random/default.hxx
		core/math/random/distribution.h
		core/math/random/distribution.hxx
		core/math/random/range.h
		core/math/random/range.hxx
		core/math/random/table.h
		core/math/random/table.hxx
		core/math/trigonometric.h
		core/math/trigonometric.hxx
		core/math.h
		core/noncopyable.h
		core/nonmovable.h
		core/nullable/extension.hxx
		core/nullable/holder.h
		core/nullable/nullable.h
		core/nullable/nullable.hxx
		core/nullable.h
		core/point2D.h
		core/point3D.h
		core/predicate/result/and.h
		core/predicate/result/or.h
		core/predicate/result/sum.h
		core/preprocessor/capabilities.h
		core/preprocessor/enum.h
		core/preprocessor/math.h
		core/preprocessor/std.h
		core/preprocessor/unixes.h
		core/preprocessor/vaargs.h
		core/preprocessor/windows.h
		core/process/program/program.cpp
		core/process/program/program.h
		core/process/program/program.hxx
		core/process/program/process-info.h
		core/process/program/process-info.hxx
		core/process/program/windows.inc.hpp
		core/process/program/unix.inc.hpp
		core/process/rename.cpp
		core/process/rename.h
		core/process.h
		core/quaternion.h
		core/rect2D.h
		core/singleton/policies/creation.h
		core/singleton/policies/lifetime.h
		core/singleton/singleton.h
		core/singleton/singleton.hxx
		core/singleton.h
		core/slist/slist.h
		core/slist/slist.hxx
		core/slist.h
		core/smartptr/intrusive.h
		core/smartptr/intrusive.hxx
		core/smartptr/policies/checking.h
		core/smartptr/policies/constness.h
		core/smartptr/policies/conversions.h
		core/smartptr/policies/ownership.h
		core/smartptr/policies/policies.h
		core/smartptr/policies/storage.h
		core/smartptr/smartptr.h
		core/smartptr/smartptr.hxx
		core/smartptr/stl.hxx
		core/smartptr.h
		core/static/assert.h
		core/static/dynamiccast.h
		core/static/if.h
		core/static/inherit.h
		core/static/method.h
		core/static/moveconstructor.h
		core/static/remove.h
		core/static/typedef.h
		core/static/types.h
		core/stl/algorithm.h
		core/stl/hash-combine.h
		core/string/escape.h
		core/string/escape.hxx
		core/string/iterator.hxx
		core/string/operators.hxx
		core/string/string.cpp
		core/string/string.h
		core/string/string.hxx
		core/string/traits/append.h
		core/string/traits/assign.h
		core/string/traits/fill.h
		core/string/traits/integer.h
		core/string/traits/into.h
		core/string/traits/traits.cpp
		core/string/traits/traits.h
		core/string/traits/traits.hxx
		core/string/traits/vnsprintf.h
		core/string/utf8char.h
		core/string/utf8char.hxx
		core/string/wstring.h
		core/string/wstring.cpp
		core/string/wstring.hxx
		core/string.h
		core/suspend.h
		core/system/capabilities.h
		core/system/console/console.cpp
		core/system/console/console.h
		core/system/console/console.hxx
		core/system/console.h
		core/system/cpu.cpp
		core/system/cpu.h
		core/system/endian.h
		core/system/environment.h
		core/system/environment.cpp
		core/system/gettimeofday.h
		core/system/main.h
		core/system/memory.cpp
		core/system/memory.h
		core/system/memory.hxx
		core/system/nullptr.h
		core/system/process.cpp
		core/system/process.h
		core/system/stdint.h
		core/system/suspend.cpp
		core/system/suspend.h
		core/system/username.h
		core/system/username.hxx
		core/system/vs-banned.h
		core/system/x11.hdr.h
		core/traits/cstring.h
		core/traits/extension/into-cstring.h
		core/traits/extension/length.h
		core/traits/length.h
		core/tree/n/iterator/childiterator.h
		core/tree/n/iterator/childiterator.hxx
		core/tree/n/iterator/depthinfixiterator.h
		core/tree/n/iterator/depthinfixiterator.hxx
		core/tree/n/iterator/depthprefixiterator.h
		core/tree/n/iterator/depthprefixiterator.hxx
		core/tree/n/iterator/depthsuffixiterator.h
		core/tree/n/iterator/depthsuffixiterator.hxx
		core/tree/n/iterator/iterator.h
		core/tree/n/treeN.h
		core/tree/n/treeN.hxx
		core/tree/treeN.h
		core/tribool.h
		core/tribool.hxx
		core/unit/angle.h
		core/unit/data.h
		core/unit/define.h
		core/unit/length
		core/unit/length/extra.h
		core/unit/length/length.h
		core/unit/length/metric.h
		core/unit/length.h
		core/unit/luminance.h
		core/unit/time.h
		core/unit/undef.h
		core/unit/unit.h
		core/unit/unit.hxx
		core/unit.h
		core/uri/uri.cpp
		core/uri/uri.h
		core/uri/uri.hxx
		core/uri/uri.private.cpp
		core/uri/uri.private.h
		core/uri.h
		core/utils/hexdump.cpp
		core/utils/hexdump.h
		core/utils/hexdump.hxx
		core/utils/utils.h
		core/utils.h
		core/validator/text/default.h
		core/validator/text/default.hxx
		core/validator/text/default.private.h
		core/validator/text/validator.h
		core/validator/validator.h
		core/variant/dataholder/array.h
		core/variant/dataholder/dataholder.cpp
		core/variant/dataholder/dataholder.h
		core/variant/dataholder/dataholder.hxx
		core/variant/dataholder/string.h
		core/variant/fwd.h
		core/variant/types.h
		core/variant/variant.cpp
		core/variant/variant.h
		core/variant/variant.hxx
		core/variant.h
		core/vector3D.h
		core/version/version.cpp
		core/version/version.h
		core/version/version.hxx


		datetime/timestamp.h
		datetime/timestamp.hxx
		datetime/timestamp.cpp


		io/constants.cpp
		io/constants.h
		io/directory/commons.h
		io/directory/copy.cpp
		io/directory/create.cpp
		io/directory/current.cpp
		io/directory/directory.h
		io/directory/directory.hxx
		io/directory/info/info.cpp
		io/directory/info/info.h
		io/directory/info/info.hxx
		io/directory/info/platform.cpp
		io/directory/info/platform.h
		io/directory/info.h
		io/directory/iterator/detachedthread.h
		io/directory/iterator/iterator.cpp
		io/directory/iterator/iterator.h
		io/directory/iterator/iterator.hxx
		io/directory/iterator.h
		io/directory/remove.cpp
		io/directory/system.cpp
		io/directory/system.h
		io/directory.h
		io/error.h
		io/exists.cpp
		io/file/copy.cpp
		io/file/file.cpp
		io/file/file.h
		io/file/file.hxx
		io/file/openmode.cpp
		io/file/openmode.h
		io/file/stream.cpp
		io/file/stream.h
		io/file/stream.hxx
		io/file.h
		io/filename-manipulation.cpp
		io/filename-manipulation.h
		io/filename-manipulation.hxx
		io/io.h
		io/io.hxx
		io/searchpath
		io/searchpath/searchpath.cpp
		io/searchpath/searchpath.h
		io/searchpath/searchpath.hxx
		io/searchpath.h


		job/enum.h
		job/fwd.h
		job/job.cpp
		job/job.h
		job/job.hxx
		job/queue/service.cpp
		job/queue/service.h
		job/queue/service.hxx
		job/queue/q-event.h
		job/queue/waitingroom.cpp
		job/queue/waitingroom.h
		job/queue/waitingroom.hxx
		job/taskgroup.h
		job/taskgroup.hxx
		job/taskgroup.cpp
		private/jobs/queue/thread.h
		private/jobs/queue/thread.cpp
		private/jobs/queue/thread.hxx


		thread/array.h
		thread/array.hxx
		thread/async-sync.cpp
		thread/every.cpp
		thread/fwd.h
		thread/id.cpp
		thread/id.h
		thread/mutex.cpp
		thread/mutex.h
		thread/mutex.hxx
		thread/policy.h
		thread/pthread.h
		thread/rwmutex.cpp
		thread/rwmutex.h
		thread/rwmutex.hxx
		thread/semaphore.cpp
		thread/semaphore.h
		thread/semaphore.hxx
		thread/signal.cpp
		thread/signal.h
		thread/signal.hxx
		thread/spawn.cpp
		thread/thread.cpp
		thread/thread.h
		thread/thread.hxx
		thread/timer.cpp
		thread/timer.h
		thread/timer.hxx
		thread/utility.h

		${yuni_additional_os_specific}
	)

if (WIN32 OR WIN64)
	add_definitions("-DUNICODE")
	add_definitions("-D_UNICODE")
	add_definitions("-D_USE_64BIT_TIME_T")
endif ()

# Setting output path
set_target_properties(yuni-static-core PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${YUNI_OUTPUT_DIRECTORY}/lib")

# Installation
install(TARGETS yuni-static-core COMPONENT ${YUNICOMPONENT_CORE}
	ARCHIVE DESTINATION lib/${YUNI_VERSIONED_INST_PATH})

# Install Core-related headers
install(
	DIRECTORY core job thread io
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
	FILES_MATCHING
		PATTERN "*.h"
		PATTERN "*.hxx"
	PATTERN ".svn" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "cmake" EXCLUDE
)
# Install other core-related base files
install(FILES
	doxygen.txt
	doxygen.h
	platform.h
	yuni.h
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni
)

install(FILES yuni.version
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/ RENAME "yuni.version")

install(FILES core/string/iterator.inc.hpp
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni/core/string)

install(FILES core/slist/iterator.inc.hpp
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni/core/slist)

install(FILES io/directory/info/iterator.inc.hpp
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/yuni/io/directory/info)

install(FILES "${YUNI_LIBYUNI_CONFIG_TARGET_INIFILE}"
	COMPONENT ${YUNICOMPONENT_CORE}
	DESTINATION include/${YUNI_VERSIONED_INST_PATH}/ RENAME "yuni.config.${YUNI_LIBYUNI_CONFIG_COMPILER}")
