# Install script for directory: /Users/ben/ns-3/src/uan

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/ben/ns-3/build/lib/libns3-dev-uan.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/ben/ns-3/build/lib"
      -add_rpath "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib:/usr/local/lib64:$ORIGIN/:$ORIGIN/../lib64"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/Users/ben/ns-3/src/uan/helper/acoustic-modem-energy-model-helper.h"
    "/Users/ben/ns-3/src/uan/helper/uan-helper.h"
    "/Users/ben/ns-3/src/uan/model/acoustic-modem-energy-model.h"
    "/Users/ben/ns-3/src/uan/model/uan-channel.h"
    "/Users/ben/ns-3/src/uan/model/uan-header-common.h"
    "/Users/ben/ns-3/src/uan/model/uan-header-rc.h"
    "/Users/ben/ns-3/src/uan/model/uan-mac-aloha.h"
    "/Users/ben/ns-3/src/uan/model/uan-mac-cw.h"
    "/Users/ben/ns-3/src/uan/model/uan-mac-rc-gw.h"
    "/Users/ben/ns-3/src/uan/model/uan-mac-rc.h"
    "/Users/ben/ns-3/src/uan/model/uan-mac.h"
    "/Users/ben/ns-3/src/uan/model/uan-net-device.h"
    "/Users/ben/ns-3/src/uan/model/uan-noise-model-default.h"
    "/Users/ben/ns-3/src/uan/model/uan-noise-model.h"
    "/Users/ben/ns-3/src/uan/model/uan-phy-dual.h"
    "/Users/ben/ns-3/src/uan/model/uan-phy-gen.h"
    "/Users/ben/ns-3/src/uan/model/uan-phy.h"
    "/Users/ben/ns-3/src/uan/model/uan-prop-model-ideal.h"
    "/Users/ben/ns-3/src/uan/model/uan-prop-model-thorp.h"
    "/Users/ben/ns-3/src/uan/model/uan-prop-model.h"
    "/Users/ben/ns-3/src/uan/model/uan-transducer-hd.h"
    "/Users/ben/ns-3/src/uan/model/uan-transducer.h"
    "/Users/ben/ns-3/src/uan/model/uan-tx-mode.h"
    "/Users/ben/ns-3/build/include/ns3/uan-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/ben/ns-3/build/src/uan/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
