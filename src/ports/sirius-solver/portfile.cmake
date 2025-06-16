vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO "rte-france/sirius-solver"
        REF "antares-integration-v1.8"
        SHA512 da3b4af4c3bf3dfb03e10630a06dd9a7368226b1eec6248930213cbde54072601b98556c00a39f8232011bafb7f9e2bc7ca8c56e97ff771990350f79a8cad06a
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
