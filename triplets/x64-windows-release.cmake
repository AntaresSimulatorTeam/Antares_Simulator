set(VCPKG_TARGET_ARCHITECTURE x64)

set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

# Avoid building debug artifacts
set(VCPKG_BUILD_TYPE release)

# Link these ports statically to avoid DLL conflicts.
# - zlib, minizip: historical static linkage
# - abseil, re2: OR-Tools ships its own abseil_dll.dll and re2.dll (built
#   against a different absl version than vcpkg). If vcpkg also builds them
#   as DLLs, only one version of abseil_dll.dll can be loaded per process,
#   causing STATUS_ENTRYPOINT_NOT_FOUND (0xC0000139) at runtime.
#   Making them static in vcpkg lets Arrow embed its own absl/re2 symbols
#   while OR-Tools keeps using its own abseil_dll.dll without conflict.
list(APPEND STATIC_PORTS "zlib.*" "minizip.*" "abseil" "re2")
foreach (STATIC_PORT IN LISTS STATIC_PORTS)
    if(PORT MATCHES ${STATIC_PORT})
        set(VCPKG_LIBRARY_LINKAGE static)
    endif()
endforeach ()
