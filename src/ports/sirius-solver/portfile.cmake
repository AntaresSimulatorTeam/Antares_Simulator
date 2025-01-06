vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO "rte-france/sirius-solver"
        REF "antares-integration-v1.6"
        SHA512 8d5992f036f35b73c11261e68e030c58c3ffe22b411921c7e08e62274feeed41227b59365a00a4e32e49f35cdaa733b079cfc0a7d98347825253ae67d9c69e4a
        HEAD_REF main
)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}/src"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME sirius_solver CONFIG_PATH cmake)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_copy_pdbs()

file(INSTALL "${SOURCE_PATH}/LICENSE.TXT" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
