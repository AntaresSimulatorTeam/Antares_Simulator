

set(SRC_UI_MESSAGES
		windows/message.h
		windows/message.hxx
		windows/message.cpp
		windows/about/aboutbox.h
		windows/about/aboutbox.cpp
		windows/aboutbox.h

		windows/textinput/input.h
		windows/textinput/input.cpp

		windows/version.h
		windows/version.cpp

)
add_Library(libantares-ui-messages ${SRC_UI_MESSAGES})


target_link_libraries(libantares-ui-messages PUBLIC ${wxWidgets_LIBRARIES})
target_link_libraries(libantares-ui-messages PRIVATE libantares-core libantares-ui-common)

