add_subdirectory(batchrun)
add_subdirectory(finder)
add_subdirectory(updater)
add_subdirectory(cleaner)

#yby-aggregator need swap build
if(BUILD_SWAP)
    add_subdirectory(yby-aggregator)
endif()

add_subdirectory(config)
add_subdirectory(vacuum)

