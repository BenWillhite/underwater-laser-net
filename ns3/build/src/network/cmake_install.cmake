# Install script for directory: /Users/ben/ns-3/src/network

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/ben/ns-3/build/lib/libns3-dev-network.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/ben/ns-3/build/lib"
      -add_rpath "/usr/local/lib:$ORIGIN/:$ORIGIN/../lib:/usr/local/lib64:$ORIGIN/:$ORIGIN/../lib64"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "/Users/ben/ns-3/src/network/helper/application-container.h"
    "/Users/ben/ns-3/src/network/helper/application-helper.h"
    "/Users/ben/ns-3/src/network/helper/delay-jitter-estimation.h"
    "/Users/ben/ns-3/src/network/helper/net-device-container.h"
    "/Users/ben/ns-3/src/network/helper/node-container.h"
    "/Users/ben/ns-3/src/network/helper/packet-socket-helper.h"
    "/Users/ben/ns-3/src/network/helper/simple-net-device-helper.h"
    "/Users/ben/ns-3/src/network/helper/trace-helper.h"
    "/Users/ben/ns-3/src/network/model/address.h"
    "/Users/ben/ns-3/src/network/model/application.h"
    "/Users/ben/ns-3/src/network/model/buffer.h"
    "/Users/ben/ns-3/src/network/model/byte-tag-list.h"
    "/Users/ben/ns-3/src/network/model/channel-list.h"
    "/Users/ben/ns-3/src/network/model/channel.h"
    "/Users/ben/ns-3/src/network/model/chunk.h"
    "/Users/ben/ns-3/src/network/model/header.h"
    "/Users/ben/ns-3/src/network/model/net-device.h"
    "/Users/ben/ns-3/src/network/model/nix-vector.h"
    "/Users/ben/ns-3/src/network/model/node-list.h"
    "/Users/ben/ns-3/src/network/model/node.h"
    "/Users/ben/ns-3/src/network/model/packet-metadata.h"
    "/Users/ben/ns-3/src/network/model/packet-tag-list.h"
    "/Users/ben/ns-3/src/network/model/packet.h"
    "/Users/ben/ns-3/src/network/model/socket-factory.h"
    "/Users/ben/ns-3/src/network/model/socket.h"
    "/Users/ben/ns-3/src/network/model/tag-buffer.h"
    "/Users/ben/ns-3/src/network/model/tag.h"
    "/Users/ben/ns-3/src/network/model/trailer.h"
    "/Users/ben/ns-3/src/network/test/header-serialization-test.h"
    "/Users/ben/ns-3/src/network/utils/address-utils.h"
    "/Users/ben/ns-3/src/network/utils/bit-deserializer.h"
    "/Users/ben/ns-3/src/network/utils/bit-serializer.h"
    "/Users/ben/ns-3/src/network/utils/crc32.h"
    "/Users/ben/ns-3/src/network/utils/data-rate.h"
    "/Users/ben/ns-3/src/network/utils/drop-tail-queue.h"
    "/Users/ben/ns-3/src/network/utils/dynamic-queue-limits.h"
    "/Users/ben/ns-3/src/network/utils/error-channel.h"
    "/Users/ben/ns-3/src/network/utils/error-model.h"
    "/Users/ben/ns-3/src/network/utils/ethernet-header.h"
    "/Users/ben/ns-3/src/network/utils/ethernet-trailer.h"
    "/Users/ben/ns-3/src/network/utils/flow-id-tag.h"
    "/Users/ben/ns-3/src/network/utils/generic-phy.h"
    "/Users/ben/ns-3/src/network/utils/inet-socket-address.h"
    "/Users/ben/ns-3/src/network/utils/inet6-socket-address.h"
    "/Users/ben/ns-3/src/network/utils/ipv4-address.h"
    "/Users/ben/ns-3/src/network/utils/ipv6-address.h"
    "/Users/ben/ns-3/src/network/utils/llc-snap-header.h"
    "/Users/ben/ns-3/src/network/utils/lollipop-counter.h"
    "/Users/ben/ns-3/src/network/utils/mac16-address.h"
    "/Users/ben/ns-3/src/network/utils/mac48-address.h"
    "/Users/ben/ns-3/src/network/utils/mac64-address.h"
    "/Users/ben/ns-3/src/network/utils/mac8-address.h"
    "/Users/ben/ns-3/src/network/utils/net-device-queue-interface.h"
    "/Users/ben/ns-3/src/network/utils/output-stream-wrapper.h"
    "/Users/ben/ns-3/src/network/utils/packet-burst.h"
    "/Users/ben/ns-3/src/network/utils/packet-data-calculators.h"
    "/Users/ben/ns-3/src/network/utils/packet-probe.h"
    "/Users/ben/ns-3/src/network/utils/packet-socket-address.h"
    "/Users/ben/ns-3/src/network/utils/packet-socket-client.h"
    "/Users/ben/ns-3/src/network/utils/packet-socket-factory.h"
    "/Users/ben/ns-3/src/network/utils/packet-socket-server.h"
    "/Users/ben/ns-3/src/network/utils/packet-socket.h"
    "/Users/ben/ns-3/src/network/utils/packetbb.h"
    "/Users/ben/ns-3/src/network/utils/pcap-file-wrapper.h"
    "/Users/ben/ns-3/src/network/utils/pcap-file.h"
    "/Users/ben/ns-3/src/network/utils/pcap-test.h"
    "/Users/ben/ns-3/src/network/utils/queue-fwd.h"
    "/Users/ben/ns-3/src/network/utils/queue-item.h"
    "/Users/ben/ns-3/src/network/utils/queue-limits.h"
    "/Users/ben/ns-3/src/network/utils/queue-size.h"
    "/Users/ben/ns-3/src/network/utils/queue.h"
    "/Users/ben/ns-3/src/network/utils/radiotap-header.h"
    "/Users/ben/ns-3/src/network/utils/sequence-number.h"
    "/Users/ben/ns-3/src/network/utils/simple-channel.h"
    "/Users/ben/ns-3/src/network/utils/simple-net-device.h"
    "/Users/ben/ns-3/src/network/utils/sll-header.h"
    "/Users/ben/ns-3/src/network/utils/timestamp-tag.h"
    "/Users/ben/ns-3/build/include/ns3/network-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/ben/ns-3/build/src/network/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
