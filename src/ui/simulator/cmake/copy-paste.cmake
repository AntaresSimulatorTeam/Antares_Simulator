

set(SRC_UI_COPYPASTE
		#
		toolbox/clipboard/clipboard.h
		toolbox/clipboard/clipboard.cpp
		#
		toolbox/ext-source/handler.h
		toolbox/ext-source/handler/handler.h
		toolbox/ext-source/handler/build-context.h
		toolbox/ext-source/handler/build-context.cpp
		toolbox/ext-source/handler/com.rte-france.antares.study.cpp
		toolbox/ext-source/apply.h
		toolbox/ext-source/apply.cpp
		toolbox/ext-source/window.h
		toolbox/ext-source/window.cpp
		toolbox/ext-source/performer.h
		toolbox/ext-source/performer.cpp
		toolbox/ext-source/action-panel.h
		toolbox/ext-source/action-panel.cpp
)
add_Library(libantares-ui-copy-paste ${SRC_UI_COPYPASTE})


target_link_libraries(libantares-ui-copy-paste PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-copy-paste PRIVATE libantares-ui-common)

