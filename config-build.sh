#!/bin/bash
cmake -S src/ -B _build  \
        -DCMAKE_TOOLCHAIN_FILE="/c/Users/bari1/Desktop/work/Antares_Simulator/vcpkg/scripts/buildsystems/vcpkg.cmake"   \
        -DBUILD_TESTING=ON \
        -DVCPKG_TARGET_TRIPLET=x64-windows \
        -DCMAKE_INSTALL_PREFIX=_install \
        -DCMAKE_BUILD_TYPE=Release \
        -DDEPS_INSTALL_DIR="/c/Users/bari1/Desktop/work/Simulator-rte-antares-deps-Release/" \
        -DBUILD_UI=OFF \
        -DBUILD_not_system=OFF \
        -DBUILD_ortools=ON \
        -DCMAKE_MSVC_DEBUG_INFORMATION_FORMAT="ProgramDatabase" \
        -DCMAKE_PREFIX_PATH="/c/Users/bari1/Desktop/work/sirius_install/;/c/Users/bari1/Desktop/work/ortools-install/" \
        -DCMAKE_EXE_LINKER_FLAGS_RELEASE="/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"  && cmake --build _build  --config Release -j 2 --target all 

