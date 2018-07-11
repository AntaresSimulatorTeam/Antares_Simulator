

set(SRC_COMMON_DISPATCHER
		dispatcher.h
		dispatcher/dispatcher.h
		dispatcher/action.h
		dispatcher/action.hxx
		dispatcher/action.cpp
		#dispatcher/study.cpp
		dispatcher/gui.h
		dispatcher/gui.hxx
		dispatcher/gui.cpp
		dispatcher/settings.h
		dispatcher/settings.cpp
		)
source_group("dispatcher" FILES ${SRC_COMMON_DISPATCHER})


add_library(libantares-ui-dispatcher STATIC
	${SRC_COMMON_DISPATCHER})
