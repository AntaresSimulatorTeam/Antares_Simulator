vcpkg_from_github(
        OUT_SOURCE_PATH SOURCE_PATH
        REPO "cucumber/cucumber-cpp"
        REF "v0.7.0"
        SHA512 533aefdb083bf9b301c43feb6cdc7ef3e697eaa21539e2e51e349f9c4ce6b2770671fd9d8e41952e65d0e3bfa99b6157335ceecc574a1e1e402a713e91ff6ebd
        PATCHES patch_git_version.patch
)

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
        OPTIONS -DCUKE_ENABLE_QT=OFF -DCUKE_ENABLE_GTEST=ON -DCUKE_ENABLE_BOOST_TEST=OFF -DCUKE_ENABLE_EXAMPLES=OFF -DCUKE_TESTS_UNIT=OFF
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(PACKAGE_NAME CucumberCpp CONFIG_PATH "lib/cmake")

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_copy_pdbs()

file(INSTALL "${SOURCE_PATH}/LICENSE.txt" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
