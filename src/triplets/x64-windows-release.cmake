set(VCPKG_TARGET_ARCHITECTURE x64)

set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

# Avoid building debug artifacts
set(VCPKG_BUILD_TYPE release)

if (PORT MATCHES "boost-test")
    set(VCPKG_LIBRARY_LINKAGE dynamic)
endif ()
