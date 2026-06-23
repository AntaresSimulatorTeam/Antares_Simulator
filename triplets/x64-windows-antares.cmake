set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

# Link these ports statically to avoid DLL conflicts.
# - zlib, minizip: historical static linkage
# - abseil, re2: OR-Tools ships its own abseil_dll.dll and re2.dll (built
#   against a different absl version than vcpkg). Making them static in vcpkg
#   lets Arrow embed its own absl/re2 symbols without conflicting at runtime.
list(APPEND STATIC_PORTS "zlib.*" "minizip.*" "abseil" "re2")
foreach (STATIC_PORT IN LISTS STATIC_PORTS)
    if(PORT MATCHES ${STATIC_PORT})
        set(VCPKG_LIBRARY_LINKAGE static)
    endif()
endforeach ()
