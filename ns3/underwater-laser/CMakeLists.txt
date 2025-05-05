# src/underwater-laser/CMakeLists.txt

# 1) List all of your .cc files in model/ and helper/
set(source_files
  model/underwater-laser-channel.cc
  model/underwater-laser-propagation-loss-model.cc
  model/underwater-laser-error-rate-model.cc
  model/underwater-laser-rate-table.cc
  model/underwater-laser-net-device.cc
  helper/underwater-laser-channel-helper.cc
  helper/underwater-laser-phy-helper.cc
  helper/underwater-laser-mac-helper.cc
  helper/underwater-laser-helper.cc
)

# 2) List all of your public headers in model/ and helper/
set(header_files
  model/underwater-laser-channel.h
  model/underwater-laser-propagation-loss-model.h
  model/underwater-laser-error-rate-model.h
  model/underwater-laser-rate-table.h
  model/underwater-laser-net-device.h
  helper/underwater-laser-channel-helper.h
  helper/underwater-laser-phy-helper.h
  helper/underwater-laser-mac-helper.h
  helper/underwater-laser-helper.h
)

# 3) Ensure the compiler sees your subdirectories so #include "underwater-laser-channel.h" works
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/model
  ${CMAKE_CURRENT_SOURCE_DIR}/helper
)

# 4) Use ns-3's provided macro to build a module library named "underwater-laser"
build_lib(
  LIBNAME underwater-laser
  SOURCE_FILES ${source_files}
  HEADER_FILES ${header_files}
  LIBRARIES_TO_LINK
    ${libcore}
    ${libnetwork}
    ${libinternet}
    ${libmobility}
    ${libpropagation}
)
