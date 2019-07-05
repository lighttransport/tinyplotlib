cmake_minimum_required(VERSION 3.5)

set(BUILD_TARGET "tinyplotlib")

project(${BUILD_TARGET})

option(TINYPLOTLIB_USE_CCACHE "Compile with ccache(if available. Linux only)" OFF)

# cmake modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/sanitizers)
find_package(Sanitizers) # Address sanitizer (-DSANITIZE_ADDRESS=ON)

set (CMAKE_CXX_STANDARD 11)

# [ccache]
if (TINYPLOTLIB_USE_CCACHE)
  if (MSVC)
    # No ccache support
  else()
    find_program(CCACHE_EXE ccache)
    if (CCACHE_EXE)

      message(STATUS "Use ccache : " ${CCACHE_EXE})
      # CMAKE_C_COMPILER_LAUNCHER = available from cmake 3.4+)
      if (CMAKE_C_COMPILER_LAUNCHER)
        set(CMAKE_C_COMPILER_LAUNCHER "${CMAKE_C_COMPILER_LAUNCHER}" "${CCACHE_EXE}")
      else()
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_EXE}")
      endif()

      if (CMAKE_CXX_COMPILER_LAUNCHER)
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_CXX_COMPILER_LAUNCHER}" "${CCACHE_EXE}")
      else()
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXE}")
      endif()

    endif (CCACHE_EXE)
  endif()
endif()

set(TINYPLOTLIB_SOURCES) # set to empty

list(APPEND TINYPLOTLIB_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/nanovg.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/plot.cc
  )

# Increase warning level for clang.
# https://stackoverflow.com/questions/13638408/override-compile-flags-for-single-files
IF (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # Assume C++ sources
  set_source_files_properties(${TINYPLOTLIB_SOURCES} PROPERTIES COMPILE_FLAGS "-Weverything -Wno-system-headers -Werror -Wno-padded -Wno-c++98-compat-pedantic -Wno-documentation -Wno-documentation-unknown-command -Wno-reserved-id-macro")
ENDIF ()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(${BUILD_TARGET} ${TINYPLOTLIB_SOURCES})
add_sanitizers(${BUILD_TARGET})

add_executable(test_colormap
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_colormap.cc
)
add_sanitizers(test_colormap)

add_executable(test_matplot
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_matplot.cc
  )
add_sanitizers(test_matplot)


target_include_directories(test_colormap PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
target_include_directories(test_matplot PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(test_colormap ${BUILD_TARGET})
target_link_libraries(test_matplot ${BUILD_TARGET})

# [VisualStudio]
if (WIN32)
  # Set test as a startup project for VS IDE
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${TEST_APP})

  # For easier debugging in VS IDE(cmake 3.8.0 or later required)
  # Set working directory to $gltf-insight git repo root.
  if (CMAKE_VERSION VERSION_GREATER 3.8.0)
	set_target_properties(${BUILD_TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  endif()
endif ()
