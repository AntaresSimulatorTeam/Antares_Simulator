# Development requirements

## C++
--------------------
The compilation of *Antares Simulator* requires C++17 support.

=== "Windows"

    Compilation is tested on MSVC 2019

=== "Centos"

    By default, GCC version on Centos is old.
    Some external repositories must be enabled depending on your version of the OS:

    #### Centos 7
    You must enable the EPEL repository:
    ```
    sudo yum install epel-release
    sudo yum install git redhat-lsb-core make wget centos-release-scl scl-utils rpm-build
    sudo yum install devtoolset-10-gcc*
    
    scl enable devtoolset-10 bash
    source /opt/rh/rh-git227/enable
    ```
    
    #### Centos 8
    You must enable PowerTools, then install a more recent version of GCC by installing and enabling `devtoolset-9`:
    ```
    sudo yum install dnf-plugins-core
    sudo yum config-manager --set-enabled PowerTools
    sudo yum install devtoolset-10
    scl enable devtoolset-10 bash
    ```

=== "Ubuntu/Debian"

    #### Ubuntu 20.04 or 22.04 / Debian 11
    You need to install G++ 10:
    ``` 
    sudo apt install g++-10
    ```
    
    #### Ubuntu 23.04 / Debian 12
    The installation requires the following build and test dependencies:
    ```
    sudo apt install git unzip build-essentials
    ```

## CMake version
--------------------
CMake 3.x must be used.

=== "Windows"

    You can download latest Windows version directly from [CMake website](https://cmake.org/download/).
=== "Centos"

    ```
    sudo yum install epel-release
    sudo yum install cmake3
    ```
=== "Ubuntu/Debian"

    ```
    sudo apt install cmake
    ```
## Python version
--------------------
Python 3.x is used for end-to-end test.

=== "Windows"

    You can download latest Windows version directly from [Python website](https://www.python.org/downloads/windows/).

=== "Centos"

    ```
    sudo yum install python3 python3-pip
    ```

=== "Ubuntu/Debian"

    ```
    sudo apt install python3 python3-pip
    ```

Required python modules can be installed with:
```
pip install -r requirements-tests.txt
```

## Git version
--------------------
Git version must be above 2.15 for external dependencies build because `--ignore-whitespace` is not used by default and we have an issue with OR-Tools compilation of ZLib and application of patch on Windows (see https://github.com/google/or-tools/issues/1193).

=== "Windows"

    You can download latest Windows version directly from [Git website](https://gitforwindows.org/).

=== "Centos"

    ```
    sudo yum install rh-git227-git
    sudo yum install git
    ```
    
    Sometimes we need a 2.x version of git. To enable git 2.27:
    ```
    source /opt/rh/rh-git227/enable
    ```
    
=== "Ubuntu/Debian"

    ```
    sudo apt install git
    ```
