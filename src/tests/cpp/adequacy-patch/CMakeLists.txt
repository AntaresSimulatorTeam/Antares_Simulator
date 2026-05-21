include(${CMAKE_SOURCE_DIR}/tests/macros.cmake)

add_boost_test(test-gems-csr-adapter
        SRC test-gems-csr-adapter.cpp
        LIBS
        Antares::gems-csr-adapter
        Antares::expressions
        Antares::logs
)

add_boost_test(test-csr-builder-gems-injection
        SRC test-csr-builder-gems-injection.cpp
        LIBS
        Antares::gems-csr-adapter
        Antares::expressions
        Antares::logs
)