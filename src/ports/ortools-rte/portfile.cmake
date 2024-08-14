vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO "JasonMarechal25/or-tools-rte"
        REF "feature/port"
        SHA512 a0b0c75523f2aa6867936fd52d23a0016bc5c50484c55214d279bafb46d269c4c8d7fed008dd1bddb64bc62d5b059e9d3c689ba2d44549e8187d7c73884890e7
        HEAD_REF feature/port
)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS
        -Dortools_REPO="https://github.com/google/or-tools"
        -Dortools_REF="v9.10"
        -DFETCHCONTENT_FULLY_DISCONNECTED=OFF
        -DUSE_HIGHS=OFF
)

vcpkg_cmake_build(TARGET ortools-rte)
vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME ortools-rte CONFIG_PATH cmake)

#file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

#vcpkg_copy_pdbs()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
