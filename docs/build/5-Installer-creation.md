# Installer creation
CPack can be used to create the installer after the build phase :

=== "Windows"
    ```
    cd _build
    cpack -GNSIS
    ```
    Currently missing in NSIS installer :
    - External libraries sources

=== "Centos"

    ### RHEL .rpm (Experimental)
    ```
    cd _build
    cpack3 -G RPM .
    ```
    Note :
    > `rpm-build` must be installed for RPM creation :  `sudo yum install rpm-build`

    ### Linux .tar.gz
    ```
    cd _build
    cpack3 -G TGZ
    ```
    
    ### Required system libraries
    There are still some system libraries that must be installed if you want to use *ANTARES*:
    
    ```
    sudo yum install epel-release
    sudo yum install wxGTK3
    ```

=== "Ubuntu"

    ### Ubuntu .deb (Experimental)
    ```
    cd _build
    cpack -G DEB .
    ```
    ### Linux .tar.gz
    ```
    cd _build
    cpack -G TGZ
    ```
    
    ### Required system libraries
    There are still some system libraries that must be installed if you want to use *ANTARES*:
    ```
    sudo apt-get install libwxgtk3.0-gtk3-0v5
    ```