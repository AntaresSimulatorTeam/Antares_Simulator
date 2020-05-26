
#wxWidget required
find_package(wxWidgets REQUIRED COMPONENTS net gl core base richtext propgrid aui adv html xml)
include(${wxWidgets_USE_FILE})

# TODO : remove ADD_DEFINITIONS Swap support
ADD_DEFINITIONS("-DANTARES_SWAP_SUPPORT=1")


add_subdirectory(common)	#All UI libraries
add_subdirectory(simulator)	#Simulator application

