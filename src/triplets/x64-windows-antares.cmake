set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

# Link zlib and minizip statically
list(APPEND STATIC_PORTS "zlib.*" "minizip.*")
foreach (STATIC_PORT IN LISTS STATIC_PORTS)
    if(PORT MATCHES ${STATIC_PORT})
        set(VCPKG_LIBRARY_LINKAGE static)
    endif()
endforeach ()
