cmake_minimum_required(VERSION 3.14)
project(ns3-scratch)

# Allow including headers from this directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(target_prefix scratch_)

function(create_scratch)
  set(source_files ${ARGN})
  # Return early if no sources
  list(LENGTH source_files number_sources)
  if(number_sources EQUAL 0)
    return()
  endif()

  # Find the single source with main()
  set(scratch_src)
  foreach(source_file ${source_files})
    file(READ ${source_file} contents)
    string(REGEX MATCHALL "main[ \\(]" _match "${contents}")
    if(_match)
      list(APPEND scratch_src ${source_file})
    endif()
  endforeach()

  list(LENGTH scratch_src scratch_src_len)
  if(scr­atch_src_len EQUAL 0)
    message(FATAL_ERROR "No main() found in: ${source_files}")
  elseif(scr­atch_src_len GREATER 1)
    message(FATAL_ERROR "Multiple mains found in: ${scratch_src}")
  endif()

  # Compute executable name & path
  get_filename_component(dir ${scratch_src} DIRECTORY)
  string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" dirname "${dir}")
  string(REPLACE "/" "_" dirname "${dirname}")

  get_filename_component(name ${scratch_src} NAME_WLE)
  set(prefix scratch_)
  if(dirname)
    set(prefix scratch${dirname}_)
  endif()

  string(REPLACE "${PROJECT_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" outdir ${dir})

  build_exec(
    EXECNAME              ${name}
    EXECNAME_PREFIX       ${prefix}
    SOURCE_FILES          ${source_files}
    LIBRARIES_TO_LINK     ${ns3-libs} ${ns3-contrib-libs} wifi olsr applications internet-apps
    EXECUTABLE_DIRECTORY_PATH ${outdir}/
  )
endfunction()

# Build our main examples
create_scratch(
  "${CMAKE_CURRENT_SOURCE_DIR}/laser_mesh.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/ml-link-metric.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/throughput-tracer.cc"
)

# Auto-scan any other scratch subdirectories
file(GLOB scratch_subdirs CONFIGURE_DEPENDS
     LIST_DIRECTORIES true
     ${CMAKE_CURRENT_SOURCE_DIR}/*)
foreach(subdir ${scratch_subdirs})
  if(IS_DIRECTORY ${subdir} AND NOT EXISTS ${subdir}/CMakeLists.txt)
    file(GLOB more_sources CONFIGURE_DEPENDS ${subdir}/*.cc)
    create_scratch("${more_sources}")
  elseif(IS_DIRECTORY ${subdir})
    add_subdirectory(${subdir})
  endif()
endforeach()
