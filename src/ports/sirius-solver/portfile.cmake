vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO "rte-france/sirius-solver"
        REF "antares-integration-v1.5"
        SHA512 19c6c156861bdeb58c2f17f703124d52020c79f9b81734057bf1bc5dff3dbc464179f99aeab6c8c44a84de1f84ed8f4929f9a919d2bf8bd49ac737f656088e19
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
