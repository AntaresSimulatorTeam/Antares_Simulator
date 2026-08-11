set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

# Build both configs: some ports (e.g. rapidjson) only generate their
# CMake config files during the release pass, so a debug-only
# VCPKG_BUILD_TYPE breaks them. Building both also gives us a populated
# IMPORTED_LOCATION_DEBUG for dependencies like sirius_solver.
