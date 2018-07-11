
set(SRC_UI_SPOTLIGHT
	component/spotlight.h
	component/spotlight/spotlight.h
	component/spotlight/spotlight.hxx
	component/spotlight/spotlight.cpp
	component/spotlight/provider.h
	component/spotlight/null-provider.h
	component/spotlight/searchtoken.h
	component/spotlight/iitem.h
	component/spotlight/iitem.hxx
	component/spotlight/iitem.cpp
	component/spotlight/separator.h
	component/spotlight/separator.cpp
	component/spotlight/text.cpp
	component/spotlight/text.h
	component/spotlight/listbox-panel.h
	component/spotlight/listbox-panel.cpp
	component/spotlight/mini-frame.cpp
	component/spotlight/mini-frame.h
	component/spotlight/search-png.hxx
)
add_library(libantares-ui-component-spotlight  STATIC ${SRC_UI_SPOTLIGHT})


