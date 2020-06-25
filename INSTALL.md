# Antares Simulator CMake Build Instructions

| OS     | System librairies | VCPKG | Built in libraries |
|:-------|-----|--------|------|------|
| Linux  | [![Status][cpp_linux_svg]][cpp_linux_link] | [![Status][python_linux_svg]][python_linux_link] | [![Status][java_linux_svg]][java_linux_link] |
| Windows  | [![Status][cpp_win_svg]][cpp_win_link] | [![Status][python_win_svg]][python_win_link] | [![Status][java_win_svg]][java_win_link] |

## [Environment](#env)
ANTARES Solver/Simulator is a cross-platform project using components compatible
with many 32 bits and 64 bits OS (Windows, Linux, Unix) 

The source distribution accompanying this file has been tested for successful build on:

- Windows 7
- Windows 10
- Linux CentOS  6
- RHEL 7.3
- Ubuntu 18.04

[linux_system]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/workflows/Linux CI%20(system%20libs)/badge.svg?branch=feature%2Fcmake_build_dependency_option

[cpp_linux_link]: https://github.com/AntaresSimulatorTeam/Antares_Simulator/actions?query=workflow%3A"Linux CI%20(system%20libs)"

## Note on End-Of-Line convention
If the origin of the source folder accompanying this file is a check-out 
from a Git repository, end-of-line should be in the style of the local OS

If the source folder was distributed along with the binary form of the code
within an Antares_Simulator installer package, the end-of-line style is that of
the OS the package is designed for

To build on Windows system, please change end-of-line style to appropriate standard 
Note that Visual Studio may carry out auto-reformating. 
 

## [Dependencies](#deps)
 ANTARES depends on severals mandatory libraries. 
 - [OpenSSL](https://github.com/openssl/openssl)
 - [CURL](https://github.com/curl/curl)
 - [wxWidgets](https://github.com/wxWidgets/wxWidgets)
 (Only for Antares Simulator)

This section describes install procedures for the third-party Open source libraries used by ANTARES :
- Using VCPKG (Only tested on windows)
- Using a package manager
- Automatic librairies compilation from git


### [Using VCPKG](#vcpkg)

For Windows we will use [vcpkg](https://github.com/microsoft/vcpkg) to download and compile the librairies.

You must install the corresponding [vcpkg-triplet](https://vcpkg.readthedocs.io/en/latest/users/integration/#triplet-selection) depending on Antares version and libraries load:

- x64-windows        : 64 bits version with dynamic librairies load
- x86-windows        : 32 bits version with dynamic librairies load
- x64-windows-static : 64 bits version with static librairies load
- x86-windows-static : 32 bits version with static librairies load

The vcpkg-triplet used will be named [vcpg-triplet] later in this document.

#### Install vcpkg 

vcpkg can be installed anywhere on your computer :

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Note : all vcpkg command further described must be launch from vcpkg folder. This folder will be named [vcpkg_root] later in this document.

#### Install dependencies
```
cd [vcpkg_root]
vcpkg install openssl:[vcpg-triplet] 
vcpkg install curl:[vcpg-triplet] 
vcpkg install wxwidgets:[vcpg-triplet] 
```
### [Using a package manager](#unix_manager)
On linux you can use a package manger to download the precompiled librairies.

#### Ubuntu

```
sudo yum install openssl
sudo yum install curl
sudo yum install wxwidgets
```

#### RHEL / Centos

```
sudo apt-get install openssl
sudo apt-get install curl
sudo apt-get install wxwidgets
```
### [Automatic librairies compilation from git](#git_compil)
Dependency can be built  at configure time using the option `-DBUILD_DEPS=ON` (`OFF` by default) or you can compile few of them using the options below.

* OPENSSL (`BUILD_OPENSSL`)
* CURL (`BUILD_CURL`)
* wxWidgets (`BUILD_wxWidgets`)

Librairies are compiled with static option. When `BUILD_CURL` option is used, `BUILD_OPENSSL` option is added.

## Building Antares Solution
 
2.2.1 32 bits version
---------------------

cmake -G "Visual Studio 10" -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<vcpkg-triplet> .

or

cmake -G "Visual Studio 14 2015" -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<vcpkg-triplet> .


2.2.2 64 bits version
---------------------

cmake -G "Visual Studio 10 Win64" -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<vcpkg-triplet> .


or

cmake -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<vcpkg-triplet> .

or

cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=release -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<vcpkg-triplet> .

2.3 Compiling for Windows : Running the compiler
------------------------------------------------

# Open the solution @src/antares.sln@ with Visual Studio 10 or Visual Studio 15
# Choose the target "Release"
# Generate the solution 

# Or compile from command line
cmake --build . --config Release

3. Building Antares for Linux
=============================

3.1 Preliminary step : get a clean environment
----------------------------------------------

Do not hesitate to clean up your local repository before a checkout

Cleaning the Git repository can be achieved by runing the following command, which resets the local
repository to zero (no cached or temporary files remaining):

git clean -xdf

If the version tag (X.Y.z) needs to be updated, this should be made in the file :

 @src/CMakeLists.txt@ :


# Version
set(ANTARES_VERSION_HI	      5) # major index 		X
set(ANTARES_VERSION_LO	      0) # minor index 		Y
set(ANTARES_VERSION_REVISION  0) # revision number 	z

3.2 Compiling for Linux
-----------------------

3.2.1 Set the path
------------------

$ cd ${ANTARES}

* Set the compiler to use if the default one is not compatible with C++11
* For instance, to use gcc-4.8, use :

export CC=/usr/local/gcc/4.8/bin/gcc-4.8
export CXX=/usr/local/gcc/4.8/bin/g++-4.8

3.2.2 Generate the makefiles
----------------------------

In debug mode:
$ cmake .

Or in release mode:
$ cmake . -DCMAKE_BUILD_TYPE=release

3.2.3 Compile Antares
---------------------

$ make

Tips : If you have more than one processor/core, you can run more than one job :
  $ make -j X
  where X is the number of jobs to run simultaneously.

Tips : `make VERBOSE=1` to see the real commands