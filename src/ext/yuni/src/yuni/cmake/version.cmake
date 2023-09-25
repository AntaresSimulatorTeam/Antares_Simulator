[general]
website            = "@YUNI_URL_WEBSITE@"
devpack.repository = "@YUNI_URL_DEVPACK_REPOSITORY@"
devpack.source     = "@YUNI_URL_DEVPACK_SOURCE@"
generated.from     = "yuni/cmake/version.cmake"

[version]
version.hi         = @YUNI_VERSION_HI@
version.lo         = @YUNI_VERSION_LO@
version.rev        = @YUNI_VERSION_REV@
version.string     = "@YUNI_VERSION_HI@.@YUNI_VERSION_LO@.@YUNI_VERSION_REV@-@CMAKE_BUILD_TYPE@"
version.litestring = "@YUNI_VERSION_HI@.@YUNI_VERSION_LO@.@YUNI_VERSION_REV@"
version.target     = @CMAKE_BUILD_TYPE@

[support]
support.opengl  = @YUNI_COMPILED_WITH_SUPPORT_FOR_OPENGL@
support.directx = @YUNI_COMPILED_WITH_SUPPORT_FOR_DIRECTX@

[modules]
modules.list      = "@YUNI_MODULE_LIST@"
modules.available = "@YUNI_MODULE_AVAILABLE@"

