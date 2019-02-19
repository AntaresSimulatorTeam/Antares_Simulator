

set(SRC_UI_WINDOWS_OUTPUT
		windows/output/output.cpp
		windows/output/output.h
		windows/output/tab.cpp
		windows/output/window.h
		windows/output/window.cpp
		windows/output/layer.h
		windows/output/layer.hxx
		windows/output/layer.cpp
		windows/output/fwd.h
		windows/output/job.h
		windows/output/job.cpp
		windows/output/spotlight-provider.h
		windows/output/spotlight-provider.cpp
		windows/output/content.h
		windows/output/content.hxx
		windows/output/mini-frame.h
		windows/output/mini-frame.cpp
		windows/output/panel.h
		windows/output/panel/panel.h
		windows/output/panel/panel.hxx
		windows/output/panel/panel.cpp
		windows/output/panel/area-link.cpp
		windows/output/panel/area-link-renderer.h
		windows/output/panel/area-link-renderer.cpp
)

set(SRC_UI_OUTPUT_PROVIDERS
		windows/output/provider/variable-collector.h
		windows/output/provider/variable-collector.cpp
		windows/output/provider/variables.h
		windows/output/provider/variables.cpp
		windows/output/provider/outputs.h
		windows/output/provider/outputs.cpp
		windows/output/provider/output-comparison.h
		windows/output/provider/output-comparison.cpp
)

add_Library(libantares-ui-windows-output STATIC
		${SRC_UI_WINDOWS_OUTPUT}
		${SRC_UI_OUTPUT_PROVIDERS})

target_link_libraries(libantares-ui-windows-output  ${wxWidgets_LIBRARIES})

 
