#
# Build & link time profiling (opt-in, Clang only)
#
# Enable with:  -DANTARES_TIME_TRACE=ON
#
# Compile profiling (-ftime-trace):
#   Clang emits one <object>.json Chrome-trace file next to every object file,
#   describing where time went inside each translation unit (parsing, template
#   instantiation, codegen, ...). Open a single file in chrome://tracing or
#   https://ui.perfetto.dev, or aggregate the whole build with ClangBuildAnalyzer:
#       ClangBuildAnalyzer --all <build-dir> capture.bin
#       ClangBuildAnalyzer --analyze capture.bin
#
# Link profiling (LLD --time-trace):
#   When LLD is available it is selected as the linker and emits a
#   <output>.time-trace Chrome-trace file per linked target. Open the same way.
#
# NOTE: -ftime-trace is cached correctly by ccache >= 4.0, so leaving ccache
#       enabled is fine.
#
option(ANTARES_TIME_TRACE "Emit Clang -ftime-trace compile profiling data" OFF)
# Link profiling is a SEPARATE opt-in: it forces LLD as the linker, which on this
# project (non-PIC objects linked into PIE executables) requires -no-pie to avoid
# 'R_X86_64_PC32 against vtable ... recompile with -fPIC' link errors. Keeping it
# off by default means a plain ANTARES_TIME_TRACE build links with the normal
# linker and always succeeds; compile profiling never depends on the linker.
cmake_dependent_option(ANTARES_TIME_TRACE_LINK
        "Also profile the link step via LLD --time-trace (forces LLD + -no-pie)" OFF
        "ANTARES_TIME_TRACE" OFF)

if (ANTARES_TIME_TRACE)
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(WARNING "ANTARES_TIME_TRACE requires Clang; ignoring (compiler is ${CMAKE_CXX_COMPILER_ID}).")
    else ()
        set(ANTARES_TIME_TRACE_GRANULARITY "500" CACHE STRING
                "Minimum duration (us) of an event recorded by -ftime-trace")

        message(STATUS "{antares} Compile time profiling ENABLED (-ftime-trace, granularity=${ANTARES_TIME_TRACE_GRANULARITY}us)")

        # --- Compile profiling: one <object>.json per translation unit ---
        add_compile_options(
                $<$<COMPILE_LANGUAGE:C,CXX>:-ftime-trace>
                $<$<COMPILE_LANGUAGE:C,CXX>:-ftime-trace-granularity=${ANTARES_TIME_TRACE_GRANULARITY}>)

        # --- Link profiling (opt-in): requires LLD ---
        if (ANTARES_TIME_TRACE_LINK)
            # Locate ld.lld explicitly. -fuse-ld=lld only works if lld sits next to
            # the compiler or on PATH; with an /etc/alternatives clang symlink and a
            # versioned lld under /usr/lib/llvm-*/bin it is found by neither, and the
            # Clang driver then *silently* falls back to the default linker (no trace
            # file). So we resolve the full path and pass it via --ld-path=.
            get_filename_component(_clang_dir "${CMAKE_CXX_COMPILER}" REALPATH)
            get_filename_component(_clang_dir "${_clang_dir}" DIRECTORY)
            file(GLOB _llvm_bindirs "/usr/lib/llvm-*/bin" "/usr/local/lib/llvm-*/bin")
            find_program(ANTARES_LLD_PROGRAM
                    NAMES ld.lld
                    HINTS "${_clang_dir}" ${_llvm_bindirs})

            if (ANTARES_LLD_PROGRAM)
                # The Clang driver does NOT forward -ftime-trace to the linker, so we
                # pass LLD's own --time-trace straight through (LINKER: => -Wl,). LLD
                # then writes a <output>.time-trace file next to each linked target.
                # -no-pie: the objects are non-PIC; without it LLD rejects PC32
                # relocations against vtables when producing a PIE executable.
                add_link_options(--ld-path=${ANTARES_LLD_PROGRAM}
                        -no-pie
                        "LINKER:--time-trace"
                        "LINKER:--time-trace-granularity=${ANTARES_TIME_TRACE_GRANULARITY}")
                message(STATUS "{antares} Link time profiling ENABLED via ${ANTARES_LLD_PROGRAM} (<target>.time-trace files, -no-pie)")
            else ()
                message(WARNING "{antares} ld.lld not found: link profiling requested but disabled "
                                "(GNU ld/gold do not support --time-trace). Install lld to profile the link step.")
            endif ()
        endif ()
    endif ()
endif ()
